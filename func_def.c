#include <stdio.h>
#include "processes.h"
#include "edf.h"

void get_processes(processes *p1, int n)
{
    int i = 0;
    while (i < n)
    {
        printf("Enter process %d parameters\n", i + 1);
        printf("Arrival time: ");
        scanf("%d", &p1->P[ARRIVAL]);
        printf("Execution time: ");
        scanf("%d", &p1->P[EXECUTION]);
        printf("Deadline time: ");
        scanf("%d", &p1->P[DEADLINE]);
        printf("Period: ");
        scanf("%d", &p1->P[PERIOD]);
        p1->P[ABS_ARRIVAL] = 0;
        p1->P[EXECUTION_COPY] = 0;
        p1->P[ABS_DEADLINE] = 0;
        p1->instance = 0;
        p1->process_state = 0;
        p1++;
        i++;
        p1[i].process_id = i + 1;
    }
}



void rms_get_processes(processes p1[], int n) {
    int i = 0;
    while (i < n) {
        printf("Enter process %d parameters\n", i + 1);
        printf("Arrival time: ");
        scanf("%d", &p1[i].P[ARRIVAL]);
        printf("Execution time: ");
        scanf("%d", &p1[i].P[EXECUTION]);
        printf("Deadline time: ");
        scanf("%d", &p1[i].P[DEADLINE]);
        printf("Period: ");
        scanf("%d", &p1[i].P[PERIOD]);
        p1[i].P[ABS_ARRIVAL] = 0;
        p1[i].P[EXECUTION_COPY] = 0;
        p1[i].P[ABS_DEADLINE] = 0;
        p1[i].instance = 0;
        p1[i].process_state = 0;
        p1[i].process_id = i + 1;
        i++;
    }
}




int hyperperiod_calc(processes *p1, int n)
{
    int i = 0, ht, a[10];
    while (i < n)

    {
        a[i] = p1->P[PERIOD];
        p1++;
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







int sp_interrupt(processes *p1, int tmr, int n)
{
    int i = 0, n1 = 0, a = 0;
    processes *p1_copy;
    p1_copy = p1;
    while (i < n)
    {
        if (tmr == p1->P[ABS_ARRIVAL])
        {
            p1->process_state = 1;
            a++;
        }
        p1++;
        i++;
    }

    p1 = p1_copy;
    i = 0;

    while (i < n)
    {
        if (p1->process_state == 0)
            n1++;
        p1++;
        i++;
    }

    if (n1 == n || a != 0)
    {
        return 1;
    }

    return 0;
}

void update_abs_deadline(processes *p1, int n, int all)
{
    int i = 0;
    if (all)
    {
        while (i < n)
        {
            p1->P[ABS_DEADLINE] = p1->P[DEADLINE] + p1->P[ABS_ARRIVAL];
            p1++;
            i++;
        }
    }
    else
    {
        p1 += n;
        p1->P[ABS_DEADLINE] = p1->P[DEADLINE] + p1->P[ABS_ARRIVAL];
    }
}

void update_abs_arrival(processes *p1, int n, int k, int all)
{
    int i = 0;
    if (all)
    {
        while (i < n)
        {
            p1->P[ABS_ARRIVAL] = p1->P[ARRIVAL] + k * (p1->P[PERIOD]);
            p1++;
            i++;
        }
    }
    else
    {
        p1 += n;
        p1->P[ABS_ARRIVAL] = p1->P[ARRIVAL] + k * (p1->P[PERIOD]);
    }
}

void copy_execution_time(processes *p1, int n, int all)
{
    int i = 0;
    if (all)
    {
        while (i < n)
        {
            p1->P[EXECUTION_COPY] = p1->P[EXECUTION];
            p1++;
            i++;
        }
    }
    else
    {
        p1 += n;
        p1->P[EXECUTION_COPY] = p1->P[EXECUTION];
    }
}

int min(processes *p1, int n, int p)
{
    int i = 0, min = 0x7FFF, process_id = IDLE_PROCESS_ID;
    while (i < n)
    {
        if (min > p1->P[p] && p1->process_state == 1)
        {
            min = p1->P[p];
            process_id = i;
        }
        p1++;
        i++;
    }
    return process_id;
}

float cpu_util(processes *p1, int n)
{
    int i = 0;
    float cu = 0;
    while (i < n)
    {
        cu = cu + (float)p1->P[EXECUTION] / (float)p1->P[DEADLINE];
        p1++;
        i++;
    }
    return cu;
}



int compare_periods(const void *a, const void *b) {
    const processes *p1 = (const processes *)a;
    const processes *p2 = (const processes *)b;
    return p1->P[PERIOD] - p2->P[PERIOD];
}



void rms_scheduler(processes pro[], int num_processes, int hyper_period) {
    int timer = 0;
    int i, j;

    printf("i am here in the algorithm\n");

    // Sort tasks based on periods
    qsort(pro, num_processes, sizeof(processes), compare_periods);
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

