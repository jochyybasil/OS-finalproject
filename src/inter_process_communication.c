#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include "../inter_process_communication.h"
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h> 
#include "../inter_process_communication.h"

// Constants
#define MAX_MESSAGE_QUEUE_SIZE 10
#define MAX_MESSAGE_SIZE 50


// Global variables
static struct MessageQueue message_queue;
static sem_t message_semaphore;

// Function to initialize interprocess communication
void init_ipc() {
    // Initialize semaphore
    sem_init(&message_semaphore, 0, 1);

    // Initialize message queue
    message_queue.front = 0;
    message_queue.rear = -1;
    message_queue.count = 0;
}

// Function to send a message
void send_message(int sender_id, const char* content) {
    sem_wait(&message_semaphore); // Wait for semaphore

    if (message_queue.count < MAX_MESSAGE_QUEUE_SIZE) {
        // Increment the rear pointer (with wrap-around)
        message_queue.rear = (message_queue.rear + 1) % MAX_MESSAGE_QUEUE_SIZE;
        // Add the message to the queue
        message_queue.messages[message_queue.rear].sender_id = sender_id;
        strncpy(message_queue.messages[message_queue.rear].content, content, MAX_MESSAGE_SIZE);
        // Increment the queue size
        message_queue.count++;
    } else {
        printf("Message queue is full. Message not sent.\n");
    }

    sem_post(&message_semaphore); // Release semaphore
}

// Function to receive a message
struct Message receive_message() {
    struct Message received_message;

    sem_wait(&message_semaphore); // Wait for semaphore

    if (message_queue.count > 0) {
        // Get the message from the front of the queue
        received_message = message_queue.messages[message_queue.front];
        // Move the front pointer (with wrap-around)
        message_queue.front = (message_queue.front + 1) % MAX_MESSAGE_QUEUE_SIZE;
        // Decrement the queue size
        message_queue.count--;
    } else {
        printf("Message queue is empty. No message received.\n");
        // Return an empty message
        received_message.sender_id = -1;
        strncpy(received_message.content, "", MAX_MESSAGE_SIZE);
    }

    sem_post(&message_semaphore); // Release semaphore

    return received_message;
}

// Function to broadcast a message to all processes
void broadcast_message(int sender_id, const char* content) {
    for (int i = 0; i < MAX_MESSAGE_QUEUE_SIZE; i++) {
        send_message(sender_id, content); // Sending message to all processes
    }
}

// Function to clean up IPC resources
void cleanup_ipc() {
    // Destroy semaphore
    sem_destroy(&message_semaphore);
}