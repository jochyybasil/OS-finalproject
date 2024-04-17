#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "../process_management.h"
#include "../inter_process_communication.h"
#include "../memory_management.h"
#include "../context_switching.h"
#include "../utils.h" // Include scheduling algorithm functions

#define NUM_PROCESSES 4
#define MAX_HYPERPERIOD 1000

int main() {
    // Initialize memory manager
    int total_mem = 1024 * 2 * NUM_PROCESSES;
    init_memory_manager(total_mem);


    // Create processes
    struct Process* processes = (struct Process*)allocate_memory(NUM_PROCESSES * sizeof(struct Process));
    if (processes == NULL) {
        printf("Error creating processes. Exiting.\n");
        return 1;
    }

    // Create processes and pass them to the scheduling algorithm
    for (int i = 0; i < NUM_PROCESSES; i++) {
        processes[i].process_id = i; // Assign unique process ID
        pid_t pid = create_processes(processes, i);
        if (pid < 0) {
            printf("Error creating process %d. Exiting.\n", i);
            return 1;
        }
    }

    // Initialize tasks
    init_ipc();

    // Parent process waits for child processes to finish
    for (int i = 0; i < NUM_PROCESSES; i++) {
        wait(NULL);
    }

    int timer = 0;
    get_processes(processes, NUM_PROCESSES);

    // Perform scheduling
    int hyper_period;
    int active_process_id;
    float cpu_utilization;

    // Use the processes created in the first main
    cpu_utilization = cpu_util(processes, NUM_PROCESSES);
    printf("CPU Utilization %f\n", cpu_utilization);

    if (cpu_utilization < 1) {
        printf("Processes can be scheduled\n");
        hyper_period = hyperperiod_calc(processes, NUM_PROCESSES);
        if (hyper_period > MAX_HYPERPERIOD) {
            printf("Hyperperiod exceeds maximum allowed value\n");
            free_memory(processes);
            cleanup_ipc();
            return 1;
        }

        copy_execution_time(processes, NUM_PROCESSES, ALL);
        update_abs_arrival(processes, NUM_PROCESSES, 0, ALL);
        update_abs_deadline(processes, NUM_PROCESSES, ALL);

        while (timer <= hyper_period) {
            if (sp_interrupt(processes, timer, NUM_PROCESSES)) {
                active_process_id = min(processes, NUM_PROCESSES, abs_deadline);
            }

            if (active_process_id == IDLE_TASK_ID) {
                printf("%d  Idle\n", timer);
            }

            if (active_process_id != IDLE_TASK_ID) {
                if (processes[active_process_id].T[execution_copy] != 0) {
                    processes[active_process_id].T[execution_copy]--;
                    printf("%d  Process %d\n", timer, active_process_id + 1);
                }

                if (processes[active_process_id].T[execution_copy] == 0) {
                    processes[active_process_id].instance++;
                    processes[active_process_id].alive = 0;
                    copy_execution_time(processes, active_process_id, CURRENT);
                    update_abs_arrival(processes, active_process_id, processes[active_process_id].instance, CURRENT);
                    update_abs_deadline(processes, active_process_id, CURRENT);
                    active_process_id = min(processes, NUM_PROCESSES, abs_deadline);
                }
            }
            ++timer;
        }
        free_memory(processes);
        cleanup_ipc();
        return 0;
    } else {
        printf("Schedule is not feasible\n");
        free_memory(processes);
        cleanup_ipc();
        return 1;
    }
}
