#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "../process_management.h"
#include "../inter_process_communication.h"
#include "../memory_management.h"
#include "../context_switching.h"
#include "../utils.h" Include scheduling algorithm functions

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





// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <sys/types.h>
// #include <sys/wait.h>
// #include "../process_management.h"
// #include "../inter_process_communication.h"
// #include "../memory_management.h"
// #include "../context_switching.h"

// #define NUM_PROCESSES 4


// int main() {
//     // Initialize memory manager
//     int total_mem = 1024 * 2 * NUM_PROCESSES;
//     init_memory_manager(total_mem);

//     // Create processes
//     struct Process* processes = (struct Process*)allocate_memory(NUM_PROCESSES * sizeof(struct Process));
//     if (processes == NULL) {
//         printf("Error creating processes. Exiting.\n");
//         return 1;
//     }

//     for (int i = 0; i < NUM_PROCESSES; i++) {
//         processes[i].process_id = i; // Assign unique process ID
//         pid_t pid = create_processes(processes, i);
//         if (pid < 0) {
//             printf("Error creating process %d. Exiting.\n", i);
//             return 1;
//         }
//     }

//     // Initialize tasks
//     init_ipc();

//     // Parent process waits for child processes to finish
//     for (int i = 0; i < NUM_PROCESSES; i++) {
//         wait(NULL);
//     }

//     // Cleanup
//     free_memory(processes);
//     cleanup_ipc();

//     return 0;
// }



// // #include <stdio.h>
// // #include <stdlib.h>
// // #include "../process_management.h"
// // #include "../utils.h"
// // #include "../inter_process_communication.h"
// // #include "../memory_management.h"
// // #include <semaphore.h>
// // #include <unistd.h>

// // #define NUM_PROCESSES 4
// // #define TEMPERATURE_MEASUREMENT_INTERVAL 1000 // Interval for temperature measurement (in microseconds)
// // #define TIME_UNIT 1000 // Time unit for simulation (in microseconds)
// // #define TEMPERATURE_MEASUREMENT_ID 0 // Process ID for temperature measurement

// // int main() {




// //     // Simulate real-time scheduling
// //     int timer = 0;
// //     int hyper_period, active_process_id;
// //     float cpu_utilization;
// //     int algorithm_choice;

// //     printf("Choose scheduling algorithm (1 for EDF, 2 for RMS): ");
// //     scanf("%d", &algorithm_choice);

// //     switch (algorithm_choice) {
// //         case 1:
// //             get_processes(processes, NUM_PROCESSES);
// //             cpu_utilization = cpu_util(processes, NUM_PROCESSES);
// //             printf("CPU Utilization: %f\n", cpu_utilization);

// //             if (cpu_utilization < 1)
// //                 printf("Processes can be scheduled\n");
// //             else
// //                 printf("Schedule is not feasible\n");

// //             hyper_period = hyperperiod_calc(processes, NUM_PROCESSES);
// //             copy_execution_time(processes, NUM_PROCESSES, ALL);
// //             update_abs_arrival(processes, NUM_PROCESSES, 0, ALL);
// //             update_abs_deadline(processes, NUM_PROCESSES, ALL);

// //             while (timer <= hyper_period) {
// //                 // Simulate inter-process communication
// //                 send_message(0, "Temperature data");

// //                 // Receive messages in all processes
// //                 for (int i = 0; i < NUM_PROCESSES; i++) {
// //                     struct Message message = receive_message();
// //                     printf("Process %d received message: %s\n", i, message.content);
// //                 }

// //                 if (sp_interrupt(processes, timer, NUM_PROCESSES)) {
// //                     active_process_id = min(processes, NUM_PROCESSES, ABS_DEADLINE);
// //                 }

// //                 if (active_process_id == IDLE_PROCESS_ID) {
// //                     printf("%d  Idle\n", timer);
// //                 }

// //                 if (active_process_id != IDLE_PROCESS_ID) {
// //                     if (processes[active_process_id].P[EXECUTION_COPY] != 0) {
// //                         processes[active_process_id].P[EXECUTION_COPY]--;
// //                         printf("%d  Process %d\n", timer, active_process_id + 1);
// //                     }

// //                     if (processes[active_process_id].P[EXECUTION_COPY] == 0) {
// //                         processes[active_process_id].instance++;
// //                         processes[active_process_id].process_state = 0;
// //                         copy_execution_time(processes, active_process_id, CURRENT);
// //                         update_abs_arrival(processes, active_process_id, processes[active_process_id].instance, CURRENT);
// //                         update_abs_deadline(processes, active_process_id, CURRENT);
// //                         active_process_id = min(processes, NUM_PROCESSES, ABS_DEADLINE);
// //                     }
// //                 }
// //                 ++timer;
// //             }
// //             break;

// //         case 2:
// //             rms_get_processes(processes, NUM_PROCESSES);
// //             hyper_period = processes[0].P[PERIOD];

// //             for (int i = 1; i < NUM_PROCESSES; i++) {
// //                 hyper_period = (hyper_period * processes[i].P[PERIOD]) / gcd(hyper_period, processes[i].P[PERIOD]);
// //             }

// //             rms_scheduler(processes, NUM_PROCESSES, hyper_period);
// //             break;

// //         default:
// //             printf("Invalid choice. Exiting. \n");
// //             break;
// //     }

// //     // Terminate processes
// //     }

// //     // Free memory allocated for processes
// //     free_memory(processes);

// //     // Cleanup inter-process communication
// //     cleanup_ipc();

// //     return 0;
// // }

