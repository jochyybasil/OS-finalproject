#include <stdio.h>
#include <stdlib.h>
#include "processes.h"
#include "edf.h"

#define MAX_PROCESS 10

int timer = 0;

int main()
{

    processes tasks[MAX_PROCESS];
    processes *p;

    int n, hyper_period, active_process_id;
    float cpu_utilization;
    int algorithm_choice;

    printf("Enter number of processes\n");
    scanf("%d", &n);

    printf("Choose scheduling algorithm (1 for EDF, 2 for RMS): ");
    scanf("%d", &algorithm_choice);

    p = malloc(n * sizeof(processes));



    switch (algorithm_choice) {
    case 1:
        get_processes(p, n);
        cpu_utilization = cpu_util(p, n);
        printf("CPU Utilization %f\n", cpu_utilization);

        if (cpu_utilization < 1)
            printf("Processes can be scheduled\n");
        else
            printf("Schedule is not feasible\n");

        hyper_period = hyperperiod_calc(p, n);
        copy_execution_time(p, n, ALL);
        update_abs_arrival(p, n, 0, ALL);
        update_abs_deadline(p, n, ALL);

        while (timer <= hyper_period)
        {

            if (sp_interrupt(p, timer, n))
            {
                active_process_id = min(p, n, ABS_DEADLINE);
            }

            if (active_process_id == IDLE_PROCESS_ID)
            {
                printf("%d  Idle\n", timer);
            }

            if (active_process_id != IDLE_PROCESS_ID)
            {

                if (p[active_process_id].P[EXECUTION_COPY] != 0)
                {
                    p[active_process_id].P[EXECUTION_COPY]--;
                    printf("%d  Process %d\n", timer, active_process_id + 1);
                }

                if (p[active_process_id].P[EXECUTION_COPY] == 0)
                {
                    p[active_process_id].instance++;
                    p[active_process_id].process_state = 0;
                    copy_execution_time(p, active_process_id, CURRENT);
                    update_abs_arrival(p, active_process_id, p[active_process_id].instance, CURRENT);
                    update_abs_deadline(p, active_process_id, CURRENT);
                    active_process_id = min(p, n, ABS_DEADLINE);
                }
            } 
            ++timer;
            break;
        }


    case 2:
        //RATE MONOATOMIC ALGORITHM
        rms_get_processes(tasks, n);

        hyper_period = tasks[0].P[PERIOD];
        printf("hyper period is", hyper_period);
        for (int i = 1; i < n; i++) {
            hyper_period = (hyper_period * tasks[i].P[PERIOD]) / gcd(hyper_period, tasks[i].P[PERIOD]);
            printf("\n");
        }

        rms_scheduler(tasks, n, hyper_period);
    }

    free(p);
    return 0;
}