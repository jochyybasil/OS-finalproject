#include <stdio.h>
#include "../memory_management.h"
#include "../process_management.h"
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
