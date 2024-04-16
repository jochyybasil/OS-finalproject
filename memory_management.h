#pragma once

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
