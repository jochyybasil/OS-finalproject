#include <stdio.h>
#include "../process_management.h"
#include "../utils.h"

void get_processes(struct Process *process, int n)
{
    int i = 0;
    while (i < n)
    {
        printf("Enter Task %d parameters\n", i + 1);
        printf("Arrival time: ");
        scanf("%d", &process->T[arrival]);
        printf("Execution time: ");
        scanf("%d", &process->T[execution]);
        printf("Deadline time: ");
        scanf("%d", &process->T[deadline]);
        printf("Period: ");
        scanf("%d", &process->T[period]);
        process->T[abs_arrival] = 0;
        process->T[execution_copy] = 0;
        process->T[abs_deadline] = 0;
        process->instance = 0;
        process->alive = 0;
        process++;
        i++;
    }
}

int hyperperiod_calc(struct Process *process, int n)
{
    int i = 0, ht, a[10];
    while (i < n)

    {
        a[i] = process->T[period];
        process++;
        i++;
    }
    ht = lcm(a, n);

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
        if (tmr == process->T[abs_arrival])
        {
            process->alive = 1;
            a++;
        }
        process++;
        i++;
    }

    process = process_copy;
    i = 0;

    while (i < n)
    {
        if (process->alive == 0)
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
            process->T[abs_deadline] = process->T[deadline] + process->T[abs_arrival];
            process++;
            i++;
        }
    }
    else
    {
        process += n;
        process->T[abs_deadline] = process->T[deadline] + process->T[abs_arrival];
    }
}

void update_abs_arrival(struct Process *process, int n, int k, int all)
{
    int i = 0;
    if (all)
    {
        while (i < n)
        {
            process->T[abs_arrival] = process->T[arrival] + k * (process->T[period]);
            process++;
            i++;
        }
    }
    else
    {
        process += n;
        process->T[abs_arrival] = process->T[arrival] + k * (process->T[period]);
    }
}

void copy_execution_time(struct Process *process, int n, int all)
{
    int i = 0;
    if (all)
    {
        while (i < n)
        {
            process->T[execution_copy] = process->T[execution];
            process++;
            i++;
        }
    }
    else
    {
        process += n;
        process->T[execution_copy] = process->T[execution];
    }
}

int min(struct Process *process, int n, int p)
{
    int i = 0, min = 0x7FFF, task_id = IDLE_TASK_ID;
    while (i < n)
    {
        if (min > process->T[p] && process->alive == 1)
        {
            min = process->T[p];
            task_id = i;
        }
        process++;
        i++;
    }
    return task_id;
}

float cpu_util(struct Process *process, int n)
{
    int i = 0;
    float cu = 0;
    while (i < n)
    {
        cu = cu + (float)process->T[execution] / (float)process->T[deadline];
        process++;
        i++;
    }
    return cu;
}


