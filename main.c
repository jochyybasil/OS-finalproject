#include <stdio.h>
#include <stdlib.h>
#include "processes.h"
#include "edf.h"
#include <semaphore.h>

#define MAX_PROCESS 10
#define NUM_PROCESSES 4
int timer = 0;

sem_t semaphore;

int main()
{

    // Open a named semaphore
    sem_t* semaphore = sem_open("/my_semaphore", O_CREAT, 0644, 1);
    if (semaphore == SEM_FAILED) {
        perror("Semaphore initialization failed");
        return 1;
    }


    // Create processes
    struct Process* processes = create_processes(NUM_PROCESSES);
    if (processes == NULL) {
        printf("Error creating processes. Exiting.\n");
        sem_close(&semaphore); // Destroy semaphore
        return 1;
    }

    int n = NUM_PROCESSES;
    int hyper_period, active_process_id;
    float cpu_utilization;
    int algorithm_choice;

    printf("Choose scheduling algorithm (1 for EDF, 2 for RMS): ");
    scanf("%d", &algorithm_choice);



    switch (algorithm_choice) {
    case 1:
        get_processes(processes, n);
        cpu_utilization = cpu_util(processes, n);
        printf("CPU Utilization %f\n", cpu_utilization);

        if (cpu_utilization < 1)
            printf("Processes can be scheduled\n");
        else
            printf("Schedule is not feasible\n");

        hyper_period = hyperperiod_calc(processes, n);
        copy_execution_time(processes, n, ALL);
        update_abs_arrival(processes, n, 0, ALL);
        update_abs_deadline(processes, n, ALL);

        while (timer <= hyper_period)
        {

            if (sp_interrupt(processes, timer, n))
            {
                active_process_id = min(processes, n, ABS_DEADLINE);
            }

            if (active_process_id == IDLE_PROCESS_ID)
            {
                printf("%d  Idle\n", timer);
            }

            if (active_process_id != IDLE_PROCESS_ID)
            {

                if (processes[active_process_id].P[EXECUTION_COPY] != 0)
                {
                    processes[active_process_id].P[EXECUTION_COPY]--;
                    printf("%d  Process %d\n", timer, active_process_id + 1);
                }

                if (processes[active_process_id].P[EXECUTION_COPY] == 0)
                {
                    processes[active_process_id].instance++;
                    processes[active_process_id].process_state = 0;
                    copy_execution_time(processes, active_process_id, CURRENT);
                    update_abs_arrival(processes, active_process_id, processes[active_process_id].instance, CURRENT);
                    update_abs_deadline(processes, active_process_id, CURRENT);
                    active_process_id = min(processes, n, ABS_DEADLINE);
                }
            } 
            ++timer;
            break;
        }
        break;

    case 2: //RATE MONOATOMIC ALGORITHM
        rms_get_processes(processes, n);

        hyper_period = processes[0].P[PERIOD];
        printf("hyper period is", hyper_period);
        for (int i = 1; i < n; i++) {
            hyper_period = (hyper_period * processes[i].P[PERIOD]) / gcd(hyper_period, processes[i].P[PERIOD]);
            printf("\n");
        }

        rms_scheduler(processes, n, hyper_period);
        break;

    default:
        printf("Invalid choice. Exiting. \n");
        break;
    }

    free(processes);

    // Destroy semaphore
    sem_destroy(&semaphore);

    return 0;
}