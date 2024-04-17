# OS-finalproject: Project Readme

## Overview
This project simulates simple concepts for a real-time operating system focusing on real-time scheduling, inter-process communication, and memory management for tasks in a temperature monitoring device. The 4 taks of the system are temperature measurement, data logging, user interface, and alarm handling. It consists of several modules including process management, inter-process communication, memory management, and context switching to simulate the microkernel of the rtos.

## Design: Modules

### Process Management
- **process_management.h**: Defines the process control block (PCB) structure and functions for managing processes, such as creating processes, starting, suspending, resuming, and terminating processes.

### Inter-process Communication
- **inter_process_communication.h**: Implements functions for inter-process communication using shared memory and semaphores. It includes functions for sending, receiving, and broadcasting messages, as well as reading from and writing to shared memory.

### Memory Management
- **memory_management.h**: Provides functions for managing memory allocation and deallocation. It includes functions for initializing the memory manager, allocating memory, freeing memory, and checking for memory leaks.

### Context Switching
- **context_switching.h**: Defines structures and functions for managing task contexts and performing context switches between tasks. It includes functions for initializing task contexts and performing context switches.

## Usage
To use this project, include the necessary header files in the source code and link the corresponding implementation files during compilation. For example:

```bash
gcc -o main src/main.c src/process_management.c src/inter_process_communication.c src/memory_management.c src/context_switching.c -Isrc -pthread
```

Ensure that you initialize the memory manager and inter-process communication module before using them in your application.

## Examples
The `main.c` file provides an example usage of the modules included in this project. It demonstrates how to create processes, perform real-time scheduling, and communicate between processes using inter-process communication.


---

