#pragma once

#include <stdbool.h>
#include <stddef.h> // Include for size_t definition

// Define memory block structure
struct MemoryBlock {
    void* address;
    size_t size;
    int status;
};

// Initialize memory management module
void init_memory_manager(size_t total_memory);

// Allocate memory block
void* allocate_memory(size_t size);

// Free memory block
void free_memory(void* ptr);

// Check if memory block is valid
bool is_valid_address(void* ptr);

// Print memory allocation status
void print_memory_status();

void detect_memory_leaks();
