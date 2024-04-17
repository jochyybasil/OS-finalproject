#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../process_management.h"
#include "../inter_process_communication.h"
#include "../memory_management.h"
#include "../context_switching.h"

#define NUM_PROCESSES 4

int main() {
    // Initialize memory manager
    int total_mem = 1024 * 2 * NUM_PROCESSES;
    init_memory_manager(total_mem);

    // Initialize inter-process communication
    //init_ipc();

    // Create processes
    struct Process* processes = create_processes();
    if (processes == NULL) {
        printf("Error creating processes. Exiting.\n");
        return 1;
    }

    // Start processes
    for (int i = 0; i < NUM_PROCESSES; i++) {
        start_process(&processes[i]);
    }

    // Wait for processes to finish
    sleep(5); // Simulated time

    // Terminate processes
    for (int i = 0; i < NUM_PROCESSES; i++) {
        terminate_process(&processes[i]);
    }

    // Cleanup
    free_memory(processes);
    cleanup_ipc();

    return 0;
}



// #include <stdio.h>
// #include <stdlib.h>
// #include "../process_management.h"
// #include "../utils.h"
// #include "../inter_process_communication.h"
// #include "../memory_management.h"
// #include <semaphore.h>
// #include <unistd.h>

// #define NUM_PROCESSES 4
// #define TEMPERATURE_MEASUREMENT_INTERVAL 1000 // Interval for temperature measurement (in microseconds)
// #define TIME_UNIT 1000 // Time unit for simulation (in microseconds)
// #define TEMPERATURE_MEASUREMENT_ID 0 // Process ID for temperature measurement

// int main() {

//     // Initialize memory manager
//     int total_mem = 1024 * 2 * NUM_PROCESSES;
//     init_memory_manager(total_mem);
    

//     // Create processes for EDF or RMS scheduling
//     struct Process* processes = create_processes(NUM_PROCESSES);
//     if (processes == NULL) {
//         printf("Error creating processes. Exiting.\n");
//         return 1;
//     }

//     // Initialize inter-process communication
//     init_ipc();

//     // Start the processes
//     for (int i = 0; i < NUM_PROCESSES; i++) {
//         start_process(&processes[i]);
//     }


//     // Simulate real-time scheduling
//     int timer = 0;
//     int hyper_period, active_process_id;
//     float cpu_utilization;
//     int algorithm_choice;

//     printf("Choose scheduling algorithm (1 for EDF, 2 for RMS): ");
//     scanf("%d", &algorithm_choice);

//     switch (algorithm_choice) {
//         case 1:
//             get_processes(processes, NUM_PROCESSES);
//             cpu_utilization = cpu_util(processes, NUM_PROCESSES);
//             printf("CPU Utilization: %f\n", cpu_utilization);

//             if (cpu_utilization < 1)
//                 printf("Processes can be scheduled\n");
//             else
//                 printf("Schedule is not feasible\n");

//             hyper_period = hyperperiod_calc(processes, NUM_PROCESSES);
//             copy_execution_time(processes, NUM_PROCESSES, ALL);
//             update_abs_arrival(processes, NUM_PROCESSES, 0, ALL);
//             update_abs_deadline(processes, NUM_PROCESSES, ALL);

//             while (timer <= hyper_period) {
//                 // Simulate inter-process communication
//                 send_message(0, "Temperature data");

//                 // Receive messages in all processes
//                 for (int i = 0; i < NUM_PROCESSES; i++) {
//                     struct Message message = receive_message();
//                     printf("Process %d received message: %s\n", i, message.content);
//                 }

//                 if (sp_interrupt(processes, timer, NUM_PROCESSES)) {
//                     active_process_id = min(processes, NUM_PROCESSES, ABS_DEADLINE);
//                 }

//                 if (active_process_id == IDLE_PROCESS_ID) {
//                     printf("%d  Idle\n", timer);
//                 }

//                 if (active_process_id != IDLE_PROCESS_ID) {
//                     if (processes[active_process_id].P[EXECUTION_COPY] != 0) {
//                         processes[active_process_id].P[EXECUTION_COPY]--;
//                         printf("%d  Process %d\n", timer, active_process_id + 1);
//                     }

//                     if (processes[active_process_id].P[EXECUTION_COPY] == 0) {
//                         processes[active_process_id].instance++;
//                         processes[active_process_id].process_state = 0;
//                         copy_execution_time(processes, active_process_id, CURRENT);
//                         update_abs_arrival(processes, active_process_id, processes[active_process_id].instance, CURRENT);
//                         update_abs_deadline(processes, active_process_id, CURRENT);
//                         active_process_id = min(processes, NUM_PROCESSES, ABS_DEADLINE);
//                     }
//                 }
//                 ++timer;
//             }
//             break;

//         case 2:
//             rms_get_processes(processes, NUM_PROCESSES);
//             hyper_period = processes[0].P[PERIOD];

//             for (int i = 1; i < NUM_PROCESSES; i++) {
//                 hyper_period = (hyper_period * processes[i].P[PERIOD]) / gcd(hyper_period, processes[i].P[PERIOD]);
//             }

//             rms_scheduler(processes, NUM_PROCESSES, hyper_period);
//             break;

//         default:
//             printf("Invalid choice. Exiting. \n");
//             break;
//     }

//     // Terminate processes
//     for (int i = 0; i < NUM_PROCESSES; i++) {
//         terminate_process(&processes[i]);
//     }

//     // Free memory allocated for processes
//     free_memory(processes);

//     // Cleanup inter-process communication
//     cleanup_ipc();

//     return 0;
// }

