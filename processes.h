#ifndef PROCESS_H
#define PROCESS_H

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

struct processes{
    int process_id;
    int process_size;
    int process_state;
    pthread_t thread;
    int thread_id;
    int T[7],instance;
};


void* process_function(void* arg);
void* thread_function(void* arg);
void process_management();

#endif