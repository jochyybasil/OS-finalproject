#include <stdio.h>
#include "processes.h"
#include <unistd.h>
#include <stdlib.h>

#define NUM_PROCESSES 4
//temperature monitoring device

// create and manage process
struct Process* create_processes() {
    struct Process* processes = (struct Process*)malloc(NUM_PROCESSES * sizeof(struct Process));
    
    if (processes == NULL) {
        printf("Error: Memory allocation failed for processes\n");
        return NULL;
    }
    for (int i = 0; i < NUM_PROCESSES; i++){
        processes[i].process_id = i;
        processes[i].process_size = 1024; //byte -- to be changed to be randomly generated
        processes[i].process_state = READY;
        processes[i].process_request_limit = rand() % 5 + 1; // should be based on size of the memory
        processes[i].requested_memory_size = 1;
        pthread_create(&(processes[i].thread), NULL, i == 0 ? temperature_measurement : i == 1 ? user_interface : i == 2 ? data_logging : alarm_handling, &(processes[i].process_id));
    }

    return processes;
}

// Start a process
void start_process(struct Process *process) {
    process->process_state = RUNNING;
    printf("Starting process %d\n", process->process_id);
    pthread_join(process->thread, NULL);
}

// Temperature measurement task
void* temperature_measurement(void* arg) {
    int process_id = *((int *)arg);
    printf("Process %d is performing temperature measurement.\n", process_id);
    sleep(2); // Simulate temperature measurement task
    printf("Temperature measurement performed by process %d.\n", process_id);
    return NULL;
}

// User interface task
void* user_interface(void* arg) {
    int process_id = *((int *)arg);
    printf("Process %d is handling user interface.\n", process_id);
    sleep(1); // Simulate user interface task
    printf("User interface handled by process %d.\n", process_id);
    return NULL;
}

// Data logging task
void* data_logging(void* arg) {
    int process_id = *((int *)arg);
    printf("Process %d is performing data logging.\n", process_id);
    sleep(1); // Simulate data logging task
    printf("Data logging performed by process %d.\n", process_id);
    return NULL;
}

// Alarm handling task
void* alarm_handling(void* arg) {
    int process_id = *((int *)arg);
    printf("Process %d is handling alarms.\n", process_id);
    sleep(1); // Simulate alarm handling task
    printf("Alarms handled by process %d.\n", process_id);
    return NULL;
}

void terminate_process(struct Process *process){
    process->process_state = TERMINATED;
    printf("Terminating process %d\n", process->process_id);
    //free(process->memory);
}



