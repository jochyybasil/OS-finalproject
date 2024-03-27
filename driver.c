// #include <stdio.h>
// #include <pthread.h>
// #include <semaphore.h>
// #include "processes.h"

// // Define shared resources or variables
// int shared_variable;
// sem_t semaphore;

#include <stdio.h>
#include <stdlib.h>
#include "processes.h"
#include "edf.h"

int timer = 0;

int main()
{

    processes *t;

    int n, hyper_period, active_process_id;
    float cpu_utilization;
    printf("Enter number of processes\n");
    scanf("%d", &n);
    t = malloc(n * sizeof(processes));
    get_processes(t, n);
    cpu_utilization = cpu_util(t, n);
    printf("CPU Utilization %f\n", cpu_utilization);

    if (cpu_utilization < 1)
        printf("Processes can be scheduled\n");
    else
        printf("Schedule is not feasible\n");

    hyper_period = hyperperiod_calc(t, n);
    copy_execution_time(t, n, ALL);
    update_abs_arrival(t, n, 0, ALL);
    update_abs_deadline(t, n, ALL);

    while (timer <= hyper_period)
    {

        if (sp_interrupt(t, timer, n))
        {
            active_process_id = min(t, n, ABS_DEADLINE);
        }

        if (active_process_id == IDLE_PROCESS_ID)
        {
            printf("%d  Idle\n", timer);
        }

        if (active_process_id != IDLE_PROCESS_ID)
        {

            if (t[active_process_id].P[EXECUTION_COPY] != 0)
            {
                t[active_process_id].P[EXECUTION_COPY]--;
                printf("%d  Process %d\n", timer, active_process_id + 1);
            }

            if (t[active_process_id].P[EXECUTION_COPY] == 0)
            {
                t[active_process_id].instance++;
                t[active_process_id].process_state = 0;
                copy_execution_time(t, active_process_id, CURRENT);
                update_abs_arrival(t, active_process_id, t[active_process_id].instance, CURRENT);
                update_abs_deadline(t, active_process_id, CURRENT);
                active_process_id = min(t, n, ABS_DEADLINE);
            }
        }
        ++timer;
    }
    free(t);
    return 0;
}
