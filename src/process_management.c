#include <stdio.h>
#include "../process_management.h"
#include "../inter_process_communication.h"
#include "../memory_management.h"
#include "../context_switching.h"
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

#define NUM_PROCESSES 4

// Define process states
#define READY 0
#define RUNNING 1
#define TERMINATED 3

// Process IDs
#define TEMPERATURE_MEASUREMENT_ID 0
#define DATA_LOGGING_ID 1
#define USER_INTERFACE_ID 2
#define ALARM_HANDLING_ID 3

#define MESSAGE_SIZE 50

// Global variables for synchronization
static int temperature_measurement_completed = 0;
static int temperature_data_received = 0;

// Function to initialize a task's context
void initialize_process_context(struct Process *process, void *(*task_function)(void*), void *arg) {    
    // Initialize the task's context
    initialize_context((struct TaskContext *)&(process->context), task_function, arg);
}

// Function to create processes using fork
pid_t create_processes(struct Process *processes, int id) {
    pid_t pid;
    
    pid = fork();
    if (pid < 0) {
        // Error handling
        perror("Fork failed");
        return -1;
    } else if (pid == 0) {
        // Child process
        switch (id) {
            case TEMPERATURE_MEASUREMENT_ID:
                initialize_process_context(&(processes[id]), temperature_measurement, &(processes[id].process_id));
                break;
            case DATA_LOGGING_ID:
                initialize_process_context(&(processes[id]), data_logging, &(processes[id].process_id));
                break;
            case USER_INTERFACE_ID:
                initialize_process_context(&(processes[id]), user_interface, &(processes[id].process_id));
                break;
            case ALARM_HANDLING_ID:
                initialize_process_context(&(processes[id]), alarm_handling, &(processes[id].process_id));
                break;
            default:
                break;
        }
        start_process(&processes[id]); // Start the child process
        exit(0); // Exit the child process after execution
    }

    // Parent process returns
    return pid;
}

// Function to start a process
void start_process(struct Process *process) {
    process->process_state = RUNNING;
    printf("Starting process %d\n", process->process_id);
    // Start the task by switching to its context
    context_switch(NULL, (struct TaskContext *)&(process->context));
}

// Function to terminate a process
void terminate_process(struct Process *process) {
    process->process_state = TERMINATED;
    printf("Terminating process %d\n", process->process_id);
    // Send termination signal or perform cleanup
}


// Temperature measurement task
void* temperature_measurement(void* arg) {
    int process_id = *((int *)arg);
    printf("Process %d is performing temperature measurement.\n", process_id);

    // Simulate temperature measurement
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
    temperature_data_received = 1;

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