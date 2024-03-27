#include <pthread.h>
#include "processes.h"

#define ARRIVAL 4
#define EXECUTION 5
#define DEADLINE 6
#define PERIOD 7
#define ABS_ARRIVAL 8
#define EXECUTION_COPY 9
#define ABS_DEADLINE 10



#define IDLE_PROCESS_ID 1023  
#define ALL 1
#define CURRENT 0

void get_processes(processes *p1,int n);				
int hyperperiod_calc(processes *p1,int n);			
float cpu_util(processes *p1,int n);				
int gcd(int a, int b);					
int lcm(int *a, int n);					
int sp_interrupt(processes *p1,int tmr,int n);		
int min(processes *p1,int n,int p);				
void update_abs_arrival(processes *p1,int n,int k,int all);	
void update_abs_deadline(processes *p1,int n,int all);	
void copy_execution_time(processes *p1,int n,int all);	