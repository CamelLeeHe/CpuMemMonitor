#ifndef _GETCPUOCCUPY_H
#define _GETCPUOCCUPY_H

typedef struct{
	unsigned long total;
    unsigned long user;
    unsigned long nice;
    unsigned long system;
    unsigned long idle;
	unsigned long iowait;
	unsigned long irq;
	unsigned long softirq;
	unsigned long stealstolen;
	unsigned long guest;
}Total_Cpu_Occupy_t;

typedef struct{
    float user_ocu;
    float nice_ocu;
    float system_ocu;
    float idle_ocu;
	float iowait_ocu;
	float irq_ocu;
	float softirq_ocu;
}Total_Cpu_Occupy_rate_t;


typedef struct{
    unsigned int pid;
    unsigned long utime;
    unsigned long stime;
    unsigned long cutime;
    unsigned long cstime;
}Proc_Cpu_Occupy_t;

typedef struct proc_unit{
	char comm[64];
	unsigned int pid;
	long proc_cpu_time1;
	long proc_cpu_time2;
	unsigned int vmsize;
	unsigned int vmrss;
	float cpu_ocu;
}_pu;

#define PROCESS_TIME 14
#define FP_ERROR -1

#define NO_PRI

Total_Cpu_Occupy_t get_cpu_total_occupy(void);
long get_cpu_proc_occupy(unsigned int pid);
float get_proc_cpu(unsigned int pid);
int priAllCpuMem(unsigned int delay_us, const char *log_path);



#endif
