#pragma once

#include <pthread.h>

#define MAX_STACK_SIZE 0

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
