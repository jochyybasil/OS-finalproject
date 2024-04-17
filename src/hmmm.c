#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <signal.h>
#include <errno.h> 

#define MAX_MESSAGE_SIZE 50
#define MAX_MESSAGE_QUEUE_SIZE 10
#define SHARED_MEMORY_SIZE 1024
#define SEMAPHORE_NAME "/message_semaphore"
#define NUM_PROCESSES 4

#define TEMPERATURE_MEASUREMENT_ID 0
#define DATA_LOGGING_ID 1
#define USER_INTERFACE_ID 2
#define ALARM_HANDLING_ID 3
#define READY 0
#define RUNNING 1
#define BLOCKED 2
#define TERMINATED 3

// Define a structure for a message
struct Message {
    int sender_id;
    char content[MAX_MESSAGE_SIZE];
};

// Define a structure for the message queue
struct MessageQueue {
    struct Message messages[MAX_MESSAGE_QUEUE_SIZE];
    int front;
    int rear;
    int count;
};

// Define a structure for a task context
struct TaskContext {
    pthread_t thread;
    // Add any other necessary fields for the task context
};

// Define a structure for a process
struct Process {
    int process_id;
    int process_size;
    int process_state;
    struct TaskContext context;
    // Add any other necessary fields for your processes
};

// Global variables for synchronization
static int temperature_measurement_completed = 0;
static int temperature_data_received = 0;
static sem_t *message_semaphore;
static struct MessageQueue *message_queue;
static char *shared_memory;

// Function prototypes
void init_message_queue();
void send_message(int sender_id, const char* content);
struct Message receive_message();
void* temperature_measurement(void* arg);
void* data_logging(void* arg);
void* user_interface(void* arg);
void* alarm_handling(void* arg);
void initialize_process_context(struct Process *process, void *(*task_function)(void*), void *arg);
struct Process* create_processes(int num_processes);
void start_process(struct Process *process);
void cleanup_ipc();

// Function to initialize the message queue
void init_message_queue() {
    message_queue->front = 0;
    message_queue->rear = -1;
    message_queue->count = 0;
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

// Temperature measurement task
void* temperature_measurement(void* arg) {
    int process_id = *((int *)arg);
    printf("Process %d is performing temperature measurement.\n", process_id);

    // Simulate temperature measurement (replace this with actual measurement logic)
    int measured_temperature = rand() % 50; // Random temperature between 0 and 49

    // Convert the measured temperature to a string
    char temp_str[MAX_MESSAGE_SIZE];
    snprintf(temp_str, MAX_MESSAGE_SIZE, "%d", measured_temperature);

    // Send the measured temperature to the data logging task
    send_message(DATA_LOGGING_ID, temp_str);

    // Send the measured temperature to the alarm handling task
    send_message(ALARM_HANDLING_ID, temp_str);

    printf("Temperature measurement performed by process %d.\n", process_id);

    // Notify that temperature measurement is completed
    temperature_measurement_completed = 1;

    return NULL;
}

// Data logging task
void* data_logging(void* arg) {
    int process_id = *((int *)arg);
    printf("Process %d is performing data logging.\n", process_id);

    // Wait until temperature data is received
    while (!temperature_data_received) {
        usleep(100); // Sleep for 100 microseconds
    }

    // Receive temperature data from the temperature measurement task
    struct Message temp_message = receive_message();
    if (temp_message.sender_id == TEMPERATURE_MEASUREMENT_ID) {
        // Log the temperature data (for demonstration, just print)
        printf("Temperature data logged by process %d: %s\n", process_id, temp_message.content);

        // Send the temperature data to the user interface task
        send_message(USER_INTERFACE_ID, temp_message.content);
    }

    sleep(1); // Simulate data logging task

    printf("Data logging performed by process %d.\n", process_id);
    return NULL;
}

// User interface task
void* user_interface(void* arg) {
    int process_id = *((int *)arg);
    printf("Process %d is handling user interface.\n", process_id);

    // Wait until temperature data is received
    while (!temperature_data_received) {
        usleep(100); // Sleep for 100 microseconds
    }

    // Receive and display messages
    struct Message received_message;
    while (1) {
        received_message = receive_message();
        if (received_message.sender_id != -1) {
            printf("User Interface: Received message from process %d: %s\n", received_message.sender_id, received_message.content);
        } else {
            break; // Break the loop if no message is received
        }
    }

    sleep(1); // Simulate user interface task

    printf("User interface handled by process %d.\n", process_id);
    return NULL;
}

// Alarm handling task
void* alarm_handling(void* arg) {
    int process_id = *((int *)arg);
    printf("Process %d is handling alarms.\n", process_id);

    // Wait until temperature data is received
    while (!temperature_data_received) {
        usleep(100); // Sleep for 100 microseconds
    }

    // Receive temperature data from the temperature measurement task
    struct Message temp_message = receive_message();
    if (temp_message.sender_id == TEMPERATURE_MEASUREMENT_ID) {
        int temperature = atoi(temp_message.content);
        if (temperature > 25) {
            // Send alarm notification message to the user interface task
            send_message(USER_INTERFACE_ID, "High temperature alarm detected!");
        }
    }

    sleep(1); // Simulate alarm handling task

    printf("Alarms handled by process %d.\n", process_id);
    return NULL;
}

// Function to initialize a task's context
void initialize_process_context(struct Process *process, void *(*task_function)(void*), void *arg) {    
    // Initialize the task's context
    // This function is assumed to be provided by the user or implemented elsewhere
}

// Create and manage processes
// Updated function definition with the number of processes as an argument
struct Process* create_processes(int num_processes) {
    // Allocate memory based on the number of processes
    struct Process* processes = (struct Process*)malloc(num_processes * sizeof(struct Process));
    
    if (processes == NULL) {
        printf("Error: Memory allocation failed for processes\n");
        return NULL;
    }

    for (int i = 0; i < num_processes; i++) {
        // Initialize each process based on its ID
        processes[i].process_id = i;
        processes[i].process_size = 1024;
        processes[i].process_state = READY;

        // Create different threads based on process ID
        switch (i) {
            case TEMPERATURE_MEASUREMENT_ID:
                // Initialize process context for temperature measurement task
                initialize_process_context(&(processes[i]), temperature_measurement, &(processes[i].process_id));
                break;
            case DATA_LOGGING_ID:
                // Initialize process context for data logging task
                initialize_process_context(&(processes[i]), data_logging, &(processes[i].process_id));
                break;
            case USER_INTERFACE_ID:
                // Initialize process context for user interface task
                initialize_process_context(&(processes[i]), user_interface, &(processes[i].process_id));
                break;
            case ALARM_HANDLING_ID:
                // Initialize process context for alarm handling task
                initialize_process_context(&(processes[i]), alarm_handling, &(processes[i].process_id));
                break;
            default:
                break;
        }
    }

    return processes;
}


void initialize_context(struct TaskContext *context, void *(*task_function)(void*), void *arg) {
    pthread_create(&(context->thread), NULL, task_function, arg);
}

// Start a process
void start_process(struct Process *process) {
    process->process_state = RUNNING;
    printf("Starting process %d\n", process->process_id);
    initialize_context(&(process->context), (void *(*)(void *))temperature_measurement, &(process->process_id));
    // Start the task by initializing its context
}


// Function to clean up IPC resources
void cleanup_ipc() {
    // Close and unlink the semaphore
    sem_close(message_semaphore);
    sem_unlink(SEMAPHORE_NAME);

    // Unmap shared memory for the message queue
    if (munmap(message_queue, sizeof(struct MessageQueue)) == -1) {
        perror("Error unmapping message queue shared memory");
    }

    // Unmap shared memory for other purposes
    if (munmap(shared_memory, SHARED_MEMORY_SIZE) == -1) {
        perror("Error unmapping shared memory");
    }

    // Unlink shared memory objects
    if (shm_unlink("/message_shared_memory") == -1) {
        perror("Error unlinking message shared memory");
    }

    if (shm_unlink("/shared_memory") == -1) {
        perror("Error unlinking shared memory");
    }
}

// Main function
int main() {
    // Initialize semaphore
    message_semaphore = sem_open(SEMAPHORE_NAME, O_CREAT, 0666, 1);
    if (message_semaphore == SEM_FAILED) {
        perror("Semaphore initialization failed");
        return EXIT_FAILURE;
    }

    // Initialize shared memory for the message queue
    int fd = shm_open("/message_shared_memory", O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("Shared memory creation failed");
        return EXIT_FAILURE;
    }
    ftruncate(fd, sizeof(struct MessageQueue));
    message_queue = mmap(NULL, sizeof(struct MessageQueue), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (message_queue == MAP_FAILED) {
        perror("Memory mapping failed");
        return EXIT_FAILURE;
    }
    init_message_queue(); // Initialize the message queue

    // Initialize shared memory for other purposes (e.g., data sharing)
    fd = shm_open("/shared_memory", O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("Shared memory creation failed");
        return EXIT_FAILURE;
    }
    ftruncate(fd, SHARED_MEMORY_SIZE);
    shared_memory = mmap(NULL, SHARED_MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared_memory == MAP_FAILED) {
        perror("Memory mapping failed");
        return EXIT_FAILURE;
    }

    // Create and start processes
    struct Process *processes = create_processes(NUM_PROCESSES);
    if (processes == NULL) {
        perror("Process creation failed");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < NUM_PROCESSES; i++) {
        start_process(&processes[i]);
    }

    // Wait for processes to finish
    for (int i = 0; i < NUM_PROCESSES; i++) {
        pthread_join(processes[i].context.thread, NULL);
    }

    // Cleanup resources
    cleanup_ipc();

    return EXIT_SUCCESS;
}
