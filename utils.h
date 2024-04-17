#include "process_management.h"

#define arrival 0
#define execution 1
#define deadline 2
#define period 3
#define abs_arrival 4
#define execution_copy 5
#define abs_deadline 6

#define IDLE_TASK_ID 1023
#define ALL 1
#define CURRENT 0



void get_processes(struct Process *process,int n);
void rms_get_processes(struct Process p1[], int n);				
int hyperperiod_calc(struct Process *process,int n);			
float cpu_util(struct Process *process,int n);				
int gcd(int a, int b);					
int lcm(int *a, int n);					
int sp_interrupt(struct Process *process,int tmr,int n);		
int min(struct Process *process,int n,int p);				
void update_abs_arrival(struct Process *process,int n,int k,int all);	
void update_abs_deadline(struct Process *process,int n,int all);	
void copy_execution_time(struct Process *process,int n,int all);
void rms_scheduler(const struct Process pro[], int num_processes, int hyper_period);


// #include <pthread.h>


// #define ARRIVAL 4
// #define EXECUTION 5
// #define DEADLINE 6
// #define PERIOD 7
// #define ABS_ARRIVAL 8
// #define EXECUTION_COPY 9
// #define ABS_DEADLINE 10



// #define IDLE_PROCESS_ID 1023  
// #define ALL 1
// #define CURRENT 0

