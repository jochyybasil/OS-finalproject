#pragma once

#include <pthread.h>
// Structure to represent a task's context
struct TaskContext {
    pthread_t thread;
};

// Function to initialize a task's context
void initialize_context(struct TaskContext *context, void *(*task_function)(void*), void *arg);

// Function to perform a context switch
void context_switch(struct TaskContext *current_task, struct TaskContext *next_task);
