#include <stdio.h>
#include "../process_management.h"
#include "../utils.h"

void get_processes(struct Process *process, int n)
{
    int i = 0;
    while (i < n)
    {
        printf("Enter process %d parameters\n", i + 1);
        printf("Arrival time: ");
        scanf("%d", &process->P[ARRIVAL]);
        printf("Execution time: ");
        scanf("%d", &process->P[EXECUTION]);
        printf("Deadline time: ");
        scanf("%d", &process->P[DEADLINE]);
        printf("Period: ");
        scanf("%d", &process->P[PERIOD]);
        process->P[ABS_ARRIVAL] = 0;
        process->P[EXECUTION_COPY] = 0;
        process->P[ABS_DEADLINE] = 0;
        process->instance = 0;
        process->process_state = 0;
        process++;
        i++;
        process[i].process_id = i + 1;
    }
}



void rms_get_processes(struct Process process[], int n) {
    int i = 0;
    while (i < n) {
        printf("Enter process %d parameters\n", i + 1);
        printf("Arrival time: ");
        scanf("%d", &process[i].P[ARRIVAL]);
        printf("Execution time: ");
        scanf("%d", &process[i].P[EXECUTION]);
        printf("Deadline time: ");
        scanf("%d", &process[i].P[DEADLINE]);
        printf("Period: ");
        scanf("%d", &process[i].P[PERIOD]);
        process[i].P[ABS_ARRIVAL] = 0;
        process[i].P[EXECUTION_COPY] = 0;
        process[i].P[ABS_DEADLINE] = 0;
        process[i].instance = 0;
        process[i].process_state = 0;
        process[i].process_id = i + 1;
        i++;
    }
}




int hyperperiod_calc(struct Process *process, int n)
{
    int i = 0, ht, a[10];
    while (i < n)

    {
        a[i] = process->P[PERIOD];
        process++;
        i++;
    }
    ht = lcm(a, n);
    printf(ht);

    return ht;
}

int gcd(int a, int b)
{
    if (b == 0)
        return a;
    else
        return gcd(b, a % b);
}



int lcm(int *a, int n)
{
    int res = 1, i;
    for (i = 0; i < n; i++)
    
    {
        if (a[i] == 0) {
            // Skip zero values
            continue;
        }
        res = res * a[i] / gcd(res, a[i]);
    }
    return res;
}







int sp_interrupt(struct Process *process, int tmr, int n)
{
    int i = 0, n1 = 0, a = 0;
    struct Process *process_copy;
    process_copy = process;
    while (i < n)
    {
        if (tmr == process->P[ABS_ARRIVAL])
        {
            process->process_state = 1;
            a++;
        }
        process++;
        i++;
    }

    process = process_copy;
    i = 0;

    while (i < n)
    {
        if (process->process_state == 0)
            n1++;
        process++;
        i++;
    }

    if (n1 == n || a != 0)
    {
        return 1;
    }

    return 0;
}

void update_abs_deadline(struct Process *process, int n, int all)
{
    int i = 0;
    if (all)
    {
        while (i < n)
        {
            process->P[ABS_DEADLINE] = process->P[DEADLINE] + process->P[ABS_ARRIVAL];
            process++;
            i++;
        }
    }
    else
    {
        process += n;
        process->P[ABS_DEADLINE] = process->P[DEADLINE] + process->P[ABS_ARRIVAL];
    }
}

void update_abs_arrival(struct Process *process, int n, int k, int all)
{
    int i = 0;
    if (all)
    {
        while (i < n)
        {
            process->P[ABS_ARRIVAL] = process->P[ARRIVAL] + k * (process->P[PERIOD]);
            process++;
            i++;
        }
    }
    else
    {
        process += n;
        process->P[ABS_ARRIVAL] = process->P[ARRIVAL] + k * (process->P[PERIOD]);
    }
}

void copy_execution_time(struct Process *process, int n, int all)
{
    int i = 0;
    if (all)
    {
        while (i < n)
        {
            process->P[EXECUTION_COPY] = process->P[EXECUTION];
            process++;
            i++;
        }
    }
    else
    {
        process += n;
        process->P[EXECUTION_COPY] = process->P[EXECUTION];
    }
}

int min(struct Process *process, int n, int p)
{
    int i = 0, min = 0x7FFF, process_id = IDLE_PROCESS_ID;
    while (i < n)
    {
        if (min > process->P[p] && process->process_state == 1)
        {
            min = process->P[p];
            process_id = i;
        }
        process++;
        i++;
    }
    return process_id;
}

float cpu_util(struct Process *process, int n)
{
    int i = 0;
    float cu = 0;
    while (i < n)
    {
        cu = cu + (float)process->P[EXECUTION] / (float)process->P[DEADLINE];
        process++;
        i++;
    }
    return cu;
}



int compare_periods(const void *a, const void *b) {
    const struct Process *process = (const struct Process *)a;
    const struct Process *p2 = (const struct Process *)b;
    return process->P[PERIOD] - p2->P[PERIOD];
}



void rms_scheduler(const struct Process pro[], int num_processes, int hyper_period) {
    int timer = 0;
    int i, j;

    printf("i am here in the algorithm\n");

    // Sort tasks based on periods
    qsort(pro, num_processes, sizeof(const struct Process ), compare_periods);
    printf("I have sorted\n");

    printf("I am not entering the while loop\n");

    while (timer < hyper_period) {
        printf("Time: %d\n", timer);

        // Check for task arrivals and deadlines
        for (i = 0; i < num_processes; i++) {
            if (timer % pro[i].P[PERIOD] == 0) {
                printf("Task %d arrived\n", pro[i].process_id);
            }

            if (timer % pro[i].P[DEADLINE] == 0) {
                printf("Task %d missed deadline\n", pro[i].process_id);
            }
        }

        // Execute tasks based on priorities (shorter period -> higher priority)
        for (i = 0; i < num_processes; i++) {
            if (timer % pro[i].P[PERIOD] == 0) {
                printf("Executing Task %d\n", pro[i].process_id);
                // Simulate task execution
                for (j = 0; j < pro[i].P[EXECUTION]; j++) {
                    // Task execution
                    timer++;
                }
            }
        }

        timer++;
    }
}

