
#include <pthread.h>

#define MAX_STACK_SIZE 1024

// process control block - PCB
struct Process
{
    int process_id;
    size_t process_size;
    int process_state;
    pthread_t thread;
    int thread_id;

    char process_name[20];
    unsigned int stack[MAX_STACK_SIZE];
    pthread_t context;

    int P[7], instance;
};

struct Process* create_processes();
void start_process(struct Process *process);
void* temperature_measurement(void* arg);
void* user_interface(void* arg);
void* data_logging(void* arg);
void* alarm_handling(void* arg);
void suspend_process(struct Process *process);
void resume_suspended(struct Process *process);
void terminate_process(struct Process *process);


#include <stdio.h>
#include ""process_management.h""
#include ""inter_process_communication.h""
#include ""memory_management.h""
#include ""context_switching.h""
#include <unistd.h>
#include <stdlib.h>

#define NUM_PROCESSES 4

// Define process states
#define READY 0
#define RUNNING 1
#define BLOCKED 2
#define TERMINATED 3

#define ARRIVAL 4
#define EXECUTION 5
#define DEADLINE 6
#define PERIOD 7
#define ABS_ARRIVAL 8
#define EXECUTION_COPY 9
#define ABS_DEADLINE 10

// Process IDs
#define TEMPERATURE_MEASUREMENT_ID 0
#define DATA_LOGGING_ID 1
#define USER_INTERFACE_ID 2
#define ALARM_HANDLING_ID 3

#define MESSAGE_SIZE 50
// Using mutex for thread-safe access to global variables
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


// Global variables for synchronization
static int temperature_measurement_completed = 0;
static int temperature_data_received = 0;


// Function to initialize a task's context
void initialize_process_context(struct Process *process, void *(*task_function)(void*), void *arg) {    
    // Initialize the task's context
    initialize_context((struct TaskContext *)&(process->context), task_function, arg);
}


// Ensure all resources are properly released in error cases
// Proper error handling and resource cleanup is essential for robustness
// Create and manage processes
struct Process* create_processes() {
    struct Process* processes = (struct Process*)allocate_memory(NUM_PROCESSES * sizeof(struct Process));
    
    \
if (processes == NULL) {
    perror("Memory allocation failed for processes");
    return NULL;
}


    for (int i = 0; i < NUM_PROCESSES; i++) {
        processes[i].process_id = i;
        processes[i].process_size = 1024;
        processes[i].process_state = READY;

        // Create different threads based on process ID
        switch (i) {
            case TEMPERATURE_MEASUREMENT_ID:
                initialize_process_context(&(processes[i]), temperature_measurement, &(processes[i].process_id));
                break;
            case DATA_LOGGING_ID:
                initialize_process_context(&(processes[i]), data_logging, &(processes[i].process_id));
                break;
            case USER_INTERFACE_ID:
                initialize_process_context(&(processes[i]), user_interface, &(processes[i].process_id));
                break;
            case ALARM_HANDLING_ID:
                initialize_process_context(&(processes[i]), alarm_handling, &(processes[i].process_id));
                break;
            default:
                break;
        }
    }

    return processes;
}


// Start a process
void start_process(struct Process *process) {
    process->process_state = RUNNING;
    printf("Starting process %d\n", process->process_id);
    // Start the task by switching to its context
    context_switch(NULL, (struct TaskContext *)&(process->context));
}

// Temperature measurement task
void* temperature_measurement(void* arg) {
    int process_id = *((int *)arg);
    printf("Process %d is performing temperature measurement.\n", process_id);

    // Simulate temperature measurement (replace this with actual measurement logic)
    int measured_temperature = rand() % 50; // Random temperature between 0 and 49

    // Convert the measured temperature to a string
    char temp_str[MESSAGE_SIZE];
    snprintf(temp_str, MESSAGE_SIZE, "%d", measured_temperature);

    // Send the measured temperature to the data logging task
    send_message(DATA_LOGGING_ID, temp_str);

    // Send the measured temperature to the alarm handling task
    send_message(ALARM_HANDLING_ID, temp_str);

    printf("Temperature measurement performed by process %d.\n", process_id);

    // Notify that temperature measurement is completed
    pthread_mutex_lock(&mutex);
    temperature_measurement_completed = 1;
    pthread_mutex_unlock(&mutex);

    return NULL;
}

// Data logging task
void* data_logging(void* arg) {
    int process_id = *((int *)arg);
    printf("Process %d is performing data logging.\n", process_id);

    // Wait until temperature data is received
    while (pthread_mutex_lock(&mutex);
    int temp = !temperature_data_received;
    pthread_mutex_unlock(&mutex);
    temp) {
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
    while (pthread_mutex_lock(&mutex);
    int temp = !temperature_data_received;
    pthread_mutex_unlock(&mutex);
    temp) {
        usleep(100); // Sleep for 100 microseconds
    }

    // Receive and display messages
    struct Message received_message;
    while (1) {
        struct Message received_message;

        received_message = receive_message();
        if (received_message.sender_id != -1) {
    pthread_mutex_lock(&mutex);
    received_message = receive_message();
    pthread_mutex_unlock(&mutex);
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
    while (pthread_mutex_lock(&mutex);
    int temp = !temperature_data_received;
    pthread_mutex_unlock(&mutex);
    temp) {
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

void suspend_process(struct Process *process){
    process->process_state = BLOCKED;
    printf("Suspending process %d.\n", process->process_id);
    // Save execution context and perform other suspension tasks
}


void resume_suspended(struct Process *process){
    process->process_state = READY;
    printf("Resuming process %d.\n", process->process_id);
    // Restore execution context and resume execution

}

void terminate_process(struct Process *process){
    process->process_state = TERMINATED;
    printf("Terminating process %d\n", process->process_id);
    free_memory(process);
}


#define MAX_MESSAGES 10
#define MESSAGE_SIZE 50
// Using mutex for thread-safe access to global variables
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


#define MAX_MESSAGE_QUEUE_SIZE 10
#define MAX_MESSAGE_SIZE 5


// Data structures
struct Message{
    int sender_id;
    char content[MAX_MESSAGE_SIZE];
};

struct MessageQueue {
    struct Message messages[MAX_MESSAGE_QUEUE_SIZE];
    int front;
    int rear;
    int count;
};


void init_ipc();
void send_message(int sender_id, const char* content);
struct Message receive_message();
void broadcast_message(int sender_id, const char* content);
void write_to_shared_memory(const char* data);
char* read_from_shared_memory();
void cleanup_ipc();

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h> 
#include ""inter_process_communication.h""

// Constants
#define SHARED_MEMORY_SIZE 1024
#define SEMAPHORE_NAME "/message_semaphore"

// Global variables
static struct MessageQueue *message_queue;
static sem_t *message_semaphore;
static char *shared_memory;

// Function to initialize interprocess communication
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
        sem_close(message_semaphore);
        sem_unlink(SEMAPHORE_NAME);
        exit(EXIT_FAILURE);
    }

    // Resize shared memory to fit MessageQueue struct
    if (ftruncate(shm_fd, SHARED_MEMORY_SIZE) == -1) {
        perror("ftruncate");
        close(shm_fd);
        sem_close(message_semaphore);
        sem_unlink(SEMAPHORE_NAME);
        exit(EXIT_FAILURE);
    }

    // Map the shared memory object into the current address space
    message_queue = mmap(NULL, sizeof(struct MessageQueue), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (message_queue == MAP_FAILED) {
        perror("mmap for message_queue");
        close(shm_fd);
        sem_close(message_semaphore);
        sem_unlink(SEMAPHORE_NAME);
        exit(EXIT_FAILURE);
    }

    close(shm_fd); // Close shared memory file descriptor after mapping
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
    if (data == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
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
    munmap(message_queue, sizeof(struct MessageQueue) + SHARED_MEMORY_SIZE);
}


#include <stdbool.h>
#include <stddef.h> // Include for size_t definition

// Define memory block structure
struct MemoryBlock {
    void* address;
    size_t size;
    int status;
};


void init_memory_manager(size_t total_memory);
void* allocate_memory(size_t size);
void free_memory(void* ptr);
bool is_valid_address(void* ptr);
void print_memory_status();
void detect_memory_leaks();


#include <stdio.h>
#include ""memory_management.h""
#include ""process_management.h""
#include <unistd.h>
#include <stdlib.h>

// Define memory allocation status
#define NUM_PROCESSES 4

#define ALLOCATED 1
#define FREE 0

#define MIN_BLOCK_SIZE 16
#define MAX_BLOCKS 100

// Global variables
static struct MemoryBlock memory_blocks[MAX_BLOCKS];
static size_t total_memory;
static size_t used_memory;



// Initialize memory management module
void init_memory_manager(size_t total_mem) {
    total_memory = total_mem;
    used_memory = 0;
    // Initialize memory blocks
    for (int i = 0; i < MAX_BLOCKS; i++) {
        memory_blocks[i].address = NULL;
        memory_blocks[i].size = 0;
        memory_blocks[i].status = FREE;
    }
}

void* allocate_memory(size_t size) {
    // Check if memory allocation will exceed total memory
    if (used_memory + (size * NUM_PROCESSES) > total_memory) {
        printf("Error: Insufficient memory\n");
        return NULL;
    }

    // Allocate memory for NUM_PROCESSES processes
    void* address = malloc(size * NUM_PROCESSES);
    if (address == NULL) {
        printf("Error: Memory allocation failed\n");
        return NULL;
    }

    // Update memory blocks for each process
    for (int i = 0; i < NUM_PROCESSES; i++) {
        // Find a free memory block
        for (int j = 0; j < MAX_BLOCKS; j++) {
            if (memory_blocks[j].status == FREE) {
                // Allocate memory block for the process
                memory_blocks[j].address = address + (i * size);
                memory_blocks[j].size = size;
                memory_blocks[j].status = ALLOCATED;
                used_memory += size;
                break;
            }
        }
    }

    return address;
}



// Free memory block
void free_memory(void* ptr) {
    // Find the memory block corresponding to the address
    for (int i = 0; i < MAX_BLOCKS; i++) {
        if (memory_blocks[i].address == ptr) {
            // Free memory
            free(ptr);
            memory_blocks[i].address = NULL;
            memory_blocks[i].size = 0;
            memory_blocks[i].status = FREE;
            return;
        }
    }
    printf("Error: Invalid memory address\n");
}

// Check if memory block is valid
bool is_valid_address(void* ptr) {
    // Check if the address is within any allocated memory block
    for (int i = 0; i < MAX_BLOCKS; i++) {
        if (memory_blocks[i].status == ALLOCATED && ptr >= memory_blocks[i].address && ptr < memory_blocks[i].address + memory_blocks[i].size) {
            return true;
        }
    }
    return false;
}

// Print memory allocation status
void print_memory_status() {
    printf("Memory Status:\n");
    for (int i = 0; i < MAX_BLOCKS; i++) {
        printf("Block %d: Address=%p, Size=%lu, Status=%s\n", i, memory_blocks[i].address, memory_blocks[i].size, memory_blocks[i].status == ALLOCATED ? "Allocated" : "Free");
    }
}

// Retrieve total available memory
size_t get_total_memory() {
    return total_memory;
}

// Retrieve used memory
size_t get_used_memory() {
    return used_memory;
}

// Retrieve free memory
size_t get_free_memory() {
    return total_memory - used_memory;
}

// Detect memory leaks
void detect_memory_leaks() {
    size_t leaked_memory = 0;
    for (int i = 0; i < MAX_BLOCKS; i++) {
        if (memory_blocks[i].status == ALLOCATED) {
            leaked_memory += memory_blocks[i].size;
        }
    }
    if (leaked_memory > 0) {
        printf("Memory Leak Detected: %lu bytes\n", leaked_memory);
    } else {
        printf("No Memory Leaks Detected\n");
    }
}

#include <stdio.h>
#include ""memory_management.h""
#include ""process_management.h""
#include <unistd.h>
#include <stdlib.h>

// Define memory allocation status
#define NUM_PROCESSES 4

#define ALLOCATED 1
#define FREE 0

#define MIN_BLOCK_SIZE 16
#define MAX_BLOCKS 100

// Global variables
static struct MemoryBlock memory_blocks[MAX_BLOCKS];
static size_t total_memory;
static size_t used_memory;



// Initialize memory management module
void init_memory_manager(size_t total_mem) {
    total_memory = total_mem;
    used_memory = 0;
    // Initialize memory blocks
    for (int i = 0; i < MAX_BLOCKS; i++) {
        memory_blocks[i].address = NULL;
        memory_blocks[i].size = 0;
        memory_blocks[i].status = FREE;
    }
}

void* allocate_memory(size_t size) {
    // Check if memory allocation will exceed total memory
    if (used_memory + (size * NUM_PROCESSES) > total_memory) {
        printf("Error: Insufficient memory\n");
        return NULL;
    }

    // Allocate memory for NUM_PROCESSES processes
    void* address = malloc(size * NUM_PROCESSES);
    if (address == NULL) {
        printf("Error: Memory allocation failed\n");
        return NULL;
    }

    // Update memory blocks for each process
    for (int i = 0; i < NUM_PROCESSES; i++) {
        // Find a free memory block
        for (int j = 0; j < MAX_BLOCKS; j++) {
            if (memory_blocks[j].status == FREE) {
                // Allocate memory block for the process
                memory_blocks[j].address = address + (i * size);
                memory_blocks[j].size = size;
                memory_blocks[j].status = ALLOCATED;
                used_memory += size;
                break;
            }
        }
    }

    return address;
}



// Free memory block
void free_memory(void* ptr) {
    // Find the memory block corresponding to the address
    for (int i = 0; i < MAX_BLOCKS; i++) {
        if (memory_blocks[i].address == ptr) {
            // Free memory
            free(ptr);
            memory_blocks[i].address = NULL;
            memory_blocks[i].size = 0;
            memory_blocks[i].status = FREE;
            return;
        }
    }
    printf("Error: Invalid memory address\n");
}

// Check if memory block is valid
bool is_valid_address(void* ptr) {
    // Check if the address is within any allocated memory block
    for (int i = 0; i < MAX_BLOCKS; i++) {
        if (memory_blocks[i].status == ALLOCATED && ptr >= memory_blocks[i].address && ptr < memory_blocks[i].address + memory_blocks[i].size) {
            return true;
        }
    }
    return false;
}

// Print memory allocation status
void print_memory_status() {
    printf("Memory Status:\n");
    for (int i = 0; i < MAX_BLOCKS; i++) {
        printf("Block %d: Address=%p, Size=%lu, Status=%s\n", i, memory_blocks[i].address, memory_blocks[i].size, memory_blocks[i].status == ALLOCATED ? "Allocated" : "Free");
    }
}

// Retrieve total available memory
size_t get_total_memory() {
    return total_memory;
}

// Retrieve used memory
size_t get_used_memory() {
    return used_memory;
}

// Retrieve free memory
size_t get_free_memory() {
    return total_memory - used_memory;
}

// Detect memory leaks
void detect_memory_leaks() {
    size_t leaked_memory = 0;
    for (int i = 0; i < MAX_BLOCKS; i++) {
        if (memory_blocks[i].status == ALLOCATED) {
            leaked_memory += memory_blocks[i].size;
        }
    }
    if (leaked_memory > 0) {
        printf("Memory Leak Detected: %lu bytes\n", leaked_memory);
    } else {
        printf("No Memory Leaks Detected\n");
    }
}


#include <pthread.h>
// Structure to represent a task's context
struct TaskContext {
    pthread_t thread;
};

// Function to initialize a task's context
void initialize_context(struct TaskContext *context, void *(*task_function)(void*), void *arg);

// Function to perform a context switch
void context_switch(struct TaskContext *current_task, struct TaskContext *next_task);

#include <stdio.h>
#include <stdlib.h>

#include ""context_switching.h""
#include ""memory_management.h""

// Function to initialize a task's context
void initialize_context(struct TaskContext *context, void *(*task_function)(void*), void *arg) {
    \
if (pthread_create(&\(context->thread\), NULL, task_function, arg) != 0) {
    perror("Failed to create thread");
    return;
}

}

// Function to perform a context switch
void context_switch(struct TaskContext *current_task, struct TaskContext *next_task) {
    pthread_join(current_task->thread, NULL); // Wait for current task to finish
}

#include <pthread.h>
#include "process_management.h"

#define ARRIVAL 4
#define EXECUTION 5
#define DEADLINE 6
#define PERIOD 7
#define ABS_ARRIVAL 8
#define EXECUTION_COPY 9
#define ABS_DEADLINE 10



#define IDLE_PROCESS_ID 1023  
#define ALL 1
#define CURRENT 0

void get_processes(struct Process *process,int n);
void rms_get_processes(struct Process p1[], int n);				
int hyperperiod_calc(struct Process *process,int n);			
float cpu_util(struct Process *process,int n);				
int gcd(int a, int b);					
int lcm(int *a, int n);					
int sp_interrupt(struct Process *process,int tmr,int n);		
int min(struct Process *process,int n,int p);				
void update_abs_arrival(struct Process *process,int n,int k,int all);	
void update_abs_deadline(struct Process *process,int n,int all);	
void copy_execution_time(struct Process *process,int n,int all);
void rms_scheduler(const struct Process pro[], int num_processes, int hyper_period);


#include <stdio.h>
#include ""process_management.h""
#include "../utils.h"

void get_processes(struct Process *process, int n)
{
    int i = 0;
    while (i < n)
    {
        printf("Enter process %d parameters\n", i + 1);
        printf("Arrival time: ");
        scanf("%d", &process->P[ARRIVAL]);
        printf("Execution time: ");
        scanf("%d", &process->P[EXECUTION]);
        printf("Deadline time: ");
        scanf("%d", &process->P[DEADLINE]);
        printf("Period: ");
        scanf("%d", &process->P[PERIOD]);
        process->P[ABS_ARRIVAL] = 0;
        process->P[EXECUTION_COPY] = 0;
        process->P[ABS_DEADLINE] = 0;
        process->instance = 0;
        process->process_state = 0;
        process++;
        i++;
        process[i].process_id = i + 1;
    }
}



void rms_get_processes(struct Process process[], int n) {
    int i = 0;
    while (i < n) {
        printf("Enter process %d parameters\n", i + 1);
        printf("Arrival time: ");
        scanf("%d", &process[i].P[ARRIVAL]);
        printf("Execution time: ");
        scanf("%d", &process[i].P[EXECUTION]);
        printf("Deadline time: ");
        scanf("%d", &process[i].P[DEADLINE]);
        printf("Period: ");
        scanf("%d", &process[i].P[PERIOD]);
        process[i].P[ABS_ARRIVAL] = 0;
        process[i].P[EXECUTION_COPY] = 0;
        process[i].P[ABS_DEADLINE] = 0;
        process[i].instance = 0;
        process[i].process_state = 0;
        process[i].process_id = i + 1;
        i++;
    }
}




int hyperperiod_calc(struct Process *process, int n)
{
    int i = 0, ht, a[10];
    while (i < n)

    {
        a[i] = process->P[PERIOD];
        process++;
        i++;
    }
    ht = lcm(a, n);
    printf(ht);

    return ht;
}

int gcd(int a, int b)
{
    if (b == 0)
        return a;
    else
        return gcd(b, a % b);
}



int lcm(int *a, int n)
{
    int res = 1, i;
    for (i = 0; i < n; i++)
    
    {
        if (a[i] == 0) {
            // Skip zero values
            continue;
        }
        res = res * a[i] / gcd(res, a[i]);
    }
    return res;
}







int sp_interrupt(struct Process *process, int tmr, int n)
{
    int i = 0, n1 = 0, a = 0;
    struct Process *process_copy;
    process_copy = process;
    while (i < n)
    {
        if (tmr == process->P[ABS_ARRIVAL])
        {
            process->process_state = 1;
            a++;
        }
        process++;
        i++;
    }

    process = process_copy;
    i = 0;

    while (i < n)
    {
        if (process->process_state == 0)
            n1++;
        process++;
        i++;
    }

    if (n1 == n || a != 0)
    {
        return 1;
    }

    return 0;
}

void update_abs_deadline(struct Process *process, int n, int all)
{
    int i = 0;
    if (all)
    {
        while (i < n)
        {
            process->P[ABS_DEADLINE] = process->P[DEADLINE] + process->P[ABS_ARRIVAL];
            process++;
            i++;
        }
    }
    else
    {
        process += n;
        process->P[ABS_DEADLINE] = process->P[DEADLINE] + process->P[ABS_ARRIVAL];
    }
}

void update_abs_arrival(struct Process *process, int n, int k, int all)
{
    int i = 0;
    if (all)
    {
        while (i < n)
        {
            process->P[ABS_ARRIVAL] = process->P[ARRIVAL] + k * (process->P[PERIOD]);
            process++;
            i++;
        }
    }
    else
    {
        process += n;
        process->P[ABS_ARRIVAL] = process->P[ARRIVAL] + k * (process->P[PERIOD]);
    }
}

void copy_execution_time(struct Process *process, int n, int all)
{
    int i = 0;
    if (all)
    {
        while (i < n)
        {
            process->P[EXECUTION_COPY] = process->P[EXECUTION];
            process++;
            i++;
        }
    }
    else
    {
        process += n;
        process->P[EXECUTION_COPY] = process->P[EXECUTION];
    }
}

int min(struct Process *process, int n, int p)
{
    int i = 0, min = 0x7FFF, process_id = IDLE_PROCESS_ID;
    while (i < n)
    {
        if (min > process->P[p] && process->process_state == 1)
        {
            min = process->P[p];
            process_id = i;
        }
        process++;
        i++;
    }
    return process_id;
}

float cpu_util(struct Process *process, int n)
{
    int i = 0;
    float cu = 0;
    while (i < n)
    {
        cu = cu + (float)process->P[EXECUTION] / (float)process->P[DEADLINE];
        process++;
        i++;
    }
    return cu;
}



int compare_periods(const void *a, const void *b) {
    const struct Process *process = (const struct Process *)a;
    const struct Process *p2 = (const struct Process *)b;
    return process->P[PERIOD] - p2->P[PERIOD];
}



void rms_scheduler(const struct Process pro[], int num_processes, int hyper_period) {
    int timer = 0;
    int i, j;

    printf("i am here in the algorithm\n");

    // Sort tasks based on periods
    qsort(pro, num_processes, sizeof(const struct Process ), compare_periods);
    printf("I have sorted\n");

    printf("I am not entering the while loop\n");

    while (timer < hyper_period) {
        printf("Time: %d\n", timer);

        // Check for task arrivals and deadlines
        for (i = 0; i < num_processes; i++) {
            if (timer % pro[i].P[PERIOD] == 0) {
                printf("Task %d arrived\n", pro[i].process_id);
            }

            if (timer % pro[i].P[DEADLINE] == 0) {
                printf("Task %d missed deadline\n", pro[i].process_id);
            }
        }

        // Execute tasks based on priorities (shorter period -> higher priority)
        for (i = 0; i < num_processes; i++) {
            if (timer % pro[i].P[PERIOD] == 0) {
                printf("Executing Task %d\n", pro[i].process_id);
                // Simulate task execution
                for (j = 0; j < pro[i].P[EXECUTION]; j++) {
                    // Task execution
                    timer++;
                }
            }
        }

        timer++;
    }
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include ""process_management.h""
#include ""inter_process_communication.h""
#include ""memory_management.h""
#include ""context_switching.h""

#define NUM_PROCESSES 4

int main() {
    // Initialize memory manager
    int total_mem = 1024 * 2 * NUM_PROCESSES;
    init_memory_manager(total_mem);

    // Initialize inter-process communication
    init_ipc();

    // Create processes
    struct Process* processes = create_processes();
    if (processes == NULL) {
        printf("Error creating processes. Exiting.\n");
        return 1;
    }

    // Start processes
    for (int i = 0; i < NUM_PROCESSES; i++) {
        start_process(&processes[i]);
    }

    // Wait for processes to finish
    sleep(5); // Simulated time

    // Terminate processes
    for (int i = 0; i < NUM_PROCESSES; i++) {
        terminate_process(&processes[i]);
    }

    // Cleanup
    free_memory(processes);
    cleanup_ipc();

    return 0;
}



// #include <stdio.h>
// #include <stdlib.h>
// #include ""process_management.h""
// #include "../utils.h"
// #include ""inter_process_communication.h""
// #include ""memory_management.h""
// #include <semaphore.h>
// #include <unistd.h>

// #define NUM_PROCESSES 4
// #define TEMPERATURE_MEASUREMENT_INTERVAL 1000 // Interval for temperature measurement (in microseconds)
// #define TIME_UNIT 1000 // Time unit for simulation (in microseconds)
// #define TEMPERATURE_MEASUREMENT_ID 0 // Process ID for temperature measurement

// int main() {

//     // Initialize memory manager
//     int total_mem = 1024 * 2 * NUM_PROCESSES;
//     init_memory_manager(total_mem);
    

//     // Create processes for EDF or RMS scheduling
//     struct Process* processes = create_processes(NUM_PROCESSES);
//     if (processes == NULL) {
//         printf("Error creating processes. Exiting.\n");
//         return 1;
//     }

//     // Initialize inter-process communication
//     init_ipc();

//     // Start the processes
//     for (int i = 0; i < NUM_PROCESSES; i++) {
//         start_process(&processes[i]);
//     }


//     // Simulate real-time scheduling
//     int timer = 0;
//     int hyper_period, active_process_id;
//     float cpu_utilization;
//     int algorithm_choice;

//     printf("Choose scheduling algorithm (1 for EDF, 2 for RMS): ");
//     scanf("%d", &algorithm_choice);

//     switch (algorithm_choice) {
//         case 1:
//             get_processes(processes, NUM_PROCESSES);
//             cpu_utilization = cpu_util(processes, NUM_PROCESSES);
//             printf("CPU Utilization: %f\n", cpu_utilization);

//             if (cpu_utilization < 1)
//                 printf("Processes can be scheduled\n");
//             else
//                 printf("Schedule is not feasible\n");

//             hyper_period = hyperperiod_calc(processes, NUM_PROCESSES);
//             copy_execution_time(processes, NUM_PROCESSES, ALL);
//             update_abs_arrival(processes, NUM_PROCESSES, 0, ALL);
//             update_abs_deadline(processes, NUM_PROCESSES, ALL);

//             while (timer <= hyper_period) {
//                 // Simulate inter-process communication
//                 send_message(0, "Temperature data");

//                 // Receive messages in all processes
//                 for (int i = 0; i < NUM_PROCESSES; i++) {
//                     struct Message message = receive_message();
//                     printf("Process %d received message: %s\n", i, message.content);
//                 }

//                 if (sp_interrupt(processes, timer, NUM_PROCESSES)) {
//                     active_process_id = min(processes, NUM_PROCESSES, ABS_DEADLINE);
//                 }

//                 if (active_process_id == IDLE_PROCESS_ID) {
//                     printf("%d  Idle\n", timer);
//                 }

//                 if (active_process_id != IDLE_PROCESS_ID) {
//                     if (processes[active_process_id].P[EXECUTION_COPY] != 0) {
//                         processes[active_process_id].P[EXECUTION_COPY]--;
//                         printf("%d  Process %d\n", timer, active_process_id + 1);
//                     }

//                     if (processes[active_process_id].P[EXECUTION_COPY] == 0) {
//                         processes[active_process_id].instance++;
//                         processes[active_process_id].process_state = 0;
//                         copy_execution_time(processes, active_process_id, CURRENT);
//                         update_abs_arrival(processes, active_process_id, processes[active_process_id].instance, CURRENT);
//                         update_abs_deadline(processes, active_process_id, CURRENT);
//                         active_process_id = min(processes, NUM_PROCESSES, ABS_DEADLINE);
//                     }
//                 }
//                 ++timer;
//             }
//             break;

//         case 2:
//             rms_get_processes(processes, NUM_PROCESSES);
//             hyper_period = processes[0].P[PERIOD];

//             for (int i = 1; i < NUM_PROCESSES; i++) {
//                 hyper_period = (hyper_period * processes[i].P[PERIOD]) / gcd(hyper_period, processes[i].P[PERIOD]);
//             }

//             rms_scheduler(processes, NUM_PROCESSES, hyper_period);
//             break;

//         default:
//             printf("Invalid choice. Exiting. \n");
//             break;
//     }

//     // Terminate processes
//     for (int i = 0; i < NUM_PROCESSES; i++) {
//         terminate_process(&processes[i]);
//     }

//     // Free memory allocated for processes
//     free_memory(processes);

//     // Cleanup inter-process communication
//     cleanup_ipc();

//     return 0;
// }


