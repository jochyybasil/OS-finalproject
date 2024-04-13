
#pragma once

#include <pthread.h>

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

// process control block - PCB
struct Process
{
    int process_id;
    int process_size;
    int process_state;
    pthread_t thread;
    int thread_id;

    int process_request_limit;          /**< The request limit of the process. */
    int requested_memory_size; 

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
