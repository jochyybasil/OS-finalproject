#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <signal.h>
#include "../inter_process_communication.h"

// Constants
#define SHARED_MEMORY_SIZE 1024
#define SEMAPHORE_NAME "/message_semaphore"


// Global variables
static struct MessageQueue *message_queue;
static sem_t *message_semaphore;
static char *shared_memory;

// Function to initialize the IPC module
void init_ipc() {
    // Create or open the semaphore
    message_semaphore = sem_open(SEMAPHORE_NAME, O_CREAT, 0644, 1);
    if (message_semaphore == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    // Create or open the shared memory
    int shm_fd = shm_open("/message_shared_memory", O_CREAT | O_RDWR, 0644);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    // Resize shared memory to fit MessageQueue struct
    if (ftruncate(shm_fd, sizeof(struct MessageQueue)) == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    // Map the shared memory object into the current address space
    message_queue = mmap(NULL, sizeof(struct MessageQueue), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (message_queue == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    // Initialize MessageQueue
    message_queue->front = 0;
    message_queue->rear = -1;
    message_queue->count = 0;

    // Map the shared memory for shared content
    shared_memory = mmap(NULL, SHARED_MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_memory == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    // Close shared memory file descriptor
    close(shm_fd);
}

// Function to send a message
void send_message(int sender_id, const char* content) {
    sem_wait(message_semaphore); // Wait for semaphore

    if (message_queue->count < MAX_MESSAGE_QUEUE_SIZE) {
        // Increment the rear pointer (with wrap-around)
        message_queue->rear = (message_queue->rear + 1) % MAX_MESSAGE_QUEUE_SIZE;
        // Add the message to the queue
        message_queue->messages[message_queue->rear].sender_id = sender_id;
        strncpy(message_queue->messages[message_queue->rear].content, content, MAX_MESSAGE_SIZE);
        // Increment the queue size
        message_queue->count++;
    } else {
        printf("Message queue is full. Message not sent.\n");
    }

    sem_post(message_semaphore); // Release semaphore
}

// Function to receive a message
struct Message receive_message() {
    struct Message received_message;

    sem_wait(message_semaphore); // Wait for semaphore

    if (message_queue->count > 0) {
        // Get the message from the front of the queue
        received_message = message_queue->messages[message_queue->front];
        // Move the front pointer (with wrap-around)
        message_queue->front = (message_queue->front + 1) % MAX_MESSAGE_QUEUE_SIZE;
        // Decrement the queue size
        message_queue->count--;
    } else {
        printf("Message queue is empty. No message received.\n");
        // Return an empty message
        received_message.sender_id = -1;
        strncpy(received_message.content, "", MAX_MESSAGE_SIZE);
    }

    sem_post(message_semaphore); // Release semaphore

    return received_message;
}

// Function to broadcast a message to all processes
void broadcast_message(int sender_id, const char* content) {
    for (int i = 0; i < MAX_MESSAGE_QUEUE_SIZE; i++) {
        send_message(sender_id, content); // Sending message to all processes
    }
}

// Function to write to shared memory
void write_to_shared_memory(const char* data) {
    sem_wait(message_semaphore); // Wait for semaphore
    strncpy(shared_memory, data, SHARED_MEMORY_SIZE);
    sem_post(message_semaphore); // Release semaphore
}

// Function to read from shared memory
char* read_from_shared_memory() {
    char* data = malloc(SHARED_MEMORY_SIZE);
    sem_wait(message_semaphore); // Wait for semaphore
    strncpy(data, shared_memory, SHARED_MEMORY_SIZE);
    sem_post(message_semaphore); // Release semaphore
    return data;
}

// Function to clean up IPC resources
void cleanup_ipc() {
    // Close and unlink the semaphore
    sem_close(message_semaphore);
    sem_unlink(SEMAPHORE_NAME);

    // Unmap shared memory
    munmap(message_queue, sizeof(struct MessageQueue));
    munmap(shared_memory, SHARED_MEMORY_SIZE);

    // Unlink shared memory object
    shm_unlink("/message_shared_memory");
}
