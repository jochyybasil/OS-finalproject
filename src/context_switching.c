#include <stdio.h>
#include <stdlib.h>

#include "../context_switching.h"
#include "../memory_management.h"

// Function to initialize a task's context
void initialize_context(struct TaskContext *context, void *(*task_function)(void*), void *arg) {
    pthread_create(&(context->thread), NULL, task_function, arg);
}

// Function to perform a context switch
void context_switch(struct TaskContext *current_task, struct TaskContext *next_task) {
    pthread_join(current_task->thread, NULL); // Wait for current task to finish
}
