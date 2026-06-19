#include<stdlib.h>                                                                                                                                                                                     
#include<string.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/time.h>
#include <time.h>

#include "getCpuOccupy.h"
#include "getProcessPid.h"
#include "getMemInfo.h"

/***************************************************
* @fun   get_items
* @brief   以空格为切割，取目标字符串
* @param 
*		(in) 1. buffer 源字符串  2. item 第几项
* @ret   目标字符串
****************************************************/
const char* get_items(const char*buffer, unsigned int item){
    const char*p = buffer;
    int len = strlen(buffer);
    int count = 0;
	int i = 0;
    for (i = 0; i < len; i++){
        if(' ' == *p){
            count++;
            if(count==item-1){
                p++;
                break;
            }
        }
        p++;
    }
    return p;
}

/***************************************************
* @fun   get_cpu_total_occupy
* @brief   获取此刻整个CPU的使用时间
* @param   无
* @ret   整个CPU使用时间
cpu  279819 0 39647 165647 1297 0 4026 0 0 0
user、nice、system、idle、iowait、irq、softirq、stealstolen、guest)

****************************************************/
Total_Cpu_Occupy_t get_cpu_total_occupy(void){
    FILE *fp;
    char buff[1024] = {0};
    Total_Cpu_Occupy_t t;

    fp=fopen("/proc/stat","r");
	if (!fp) {
		perror("/proc/stat");
		return t;//^^
	}


    fgets(buff,sizeof(buff),fp);
    char name[64]={0};
    sscanf(buff, "%s %ld %ld %ld %ld %ld %ld %ld %ld %ld", name, &t.user, &t.nice, &t.system, &t.idle, 
													&t.iowait, &t.irq, &t.softirq, &t.stealstolen, &t.guest);

	t.total = t.user + t.nice + t.system + t.idle + t.iowait + 
		      t.irq + t.softirq + t.stealstolen + t.guest;
    fclose(fp);
    
    return t;
}

/***************************************************
* @fun   get_cpu_proc_occupy
* @brief   获取此刻指定进程的CPU的使用时间
* @param   (in) pid;进程pid
* @ret   该进程的CPU使用时间
****************************************************/
long get_cpu_proc_occupy(unsigned int pid){
    
    char file_name[64] = {0};
    Proc_Cpu_Occupy_t t;
    FILE *fp;
    char line_buff[1024] = {0};
    sprintf(file_name, "/proc/%d/stat", pid);
    
    fp = fopen(file_name, "r");
	if (!fp) {
		perror(file_name);
		return FP_ERROR;
	}

    
    fgets(line_buff,sizeof(line_buff), fp);
    
    sscanf(line_buff,"%u", &t.pid);
    const char *q =get_items(line_buff, PROCESS_TIME);
    sscanf(q,"%ld %ld %ld %ld", &t.utime, &t.stime, &t.cutime, &t.cstime);
    fclose(fp);
                                                                                                                                                                                                       
    return (t.utime + t.stime + t.cutime + t.cstime);
}

Total_Cpu_Occupy_rate_t cal_cpu_all_rate(Total_Cpu_Occupy_t v1, Total_Cpu_Occupy_t v2){
	Total_Cpu_Occupy_rate_t ret_rate;
	float diff_total = (float)v2.total - (float)v1.total;
	
	ret_rate.user_ocu    = 100.0 * ((float)v2.user - (float)v1.user) / diff_total;
	ret_rate.nice_ocu    = 100.0 * ((float)v2.nice - (float)v1.nice) / diff_total;
	ret_rate.system_ocu  = 100.0 * ((float)v2.system - (float)v1.system) / diff_total;
	ret_rate.iowait_ocu  = 100.0 * ((float)v2.iowait - (float)v1.iowait) / diff_total;
	ret_rate.irq_ocu     = 100.0 * ((float)v2.irq - (float)v1.irq) / diff_total;
	ret_rate.softirq_ocu = 100.0 * ((float)v2.softirq - (float)v1.softirq) / diff_total;
	ret_rate.idle_ocu    = 100.0 * ((float)v2.idle - (float)v1.idle) / diff_total;
	
	return ret_rate;
}

/***************************************************
* @fun   priAllCpuMem
* @brief   获取系统所有进程的CPU占用率及内存使用，并打印
* @param   (in) delay_us;统计CPU占用率的时间差
* @ret   成功返回1，失败返回0
%10.2f

****************************************************/

int priAllCpuMem(unsigned int delay_us, const char *log_path){
	DIR *procdir;
	struct dirent *entry;
	char path[256 + 5 + 5]; // /proc/pid/comm
	char log_line_buf[128] = {0};
	//unsigned int loop_num = 24 * 2000;//24小时
	//unsigned int loop_num = 2 * 1000;//2小时48分 12M sleep(4);
	//unsigned int loop_num = 20 * 1000;
	//unsigned int loop_num = 18 * 1000;//mpsoc 18*1000=35h
	unsigned int loop_num = 10 * 1000;
	//unsigned int loop_num = 10 * 2;
	FILE *fp_log;
	
	int i = 0;
	int j = 0;
	FILE *fp;
	unsigned long totalcputime1, totalcputime2;
	Total_Cpu_Occupy_t v_cpu1, v_cpu2;//CPU总共使用时间，计时的开始时刻和结束时刻
	Total_Cpu_Occupy_rate_t v_cpu_rate;
	Total_Mem_Info_t v_mem;
	float tt1 = 0.0;
	float tt2 = 0.0;
	float ct1 = 0.0;
	float ct2 = 0.0;

	
	//_pu test_unit[100];
	_pu test_unit[200];

	time_t current_time;
	struct tm *local_t;
	char time_str[64];

	char err_buf[128] = {0};

	fp_log = fopen(log_path,"a+b");//追加
	while(loop_num--){
		//start
		i = 0;
		j = 0;
	    procdir = opendir("/proc"); // Open /proc directory.
	    if (!procdir) {
	        perror("opendir failed");
	        return 1;
	    }
//计算此刻(start)每个进程的CPU时间
	    while ((entry = readdir(procdir))) {
	        // Skip anything that is not a PID folder.
	        if (!is_pid_folder(entry))
	            continue;

			snprintf(path, sizeof(path), "/proc/%s/comm", entry->d_name);//获取系统每个进程名字，并存储
	        fp = fopen(path, "r");
	        if (!fp) {
	            perror(path);
	            continue;
	        }
			//fgets(comm_name, sizeof(comm_name), fp);
			fscanf(fp, "%s",test_unit[i].comm);
	        test_unit[i].pid = atoi(entry->d_name);
			test_unit[i].proc_cpu_time1 = get_cpu_proc_occupy(test_unit[i].pid);//获取CPU使用时间
			i++;//累计多少个进程了
	        fclose(fp);	
	    }
//计算此刻(start)CPU总共使用时间
		v_cpu1 = get_cpu_total_occupy();
		totalcputime1 =v_cpu1.total;
		tt1 = (float)totalcputime1;//

	    closedir(procdir);
		usleep(delay_us);//延迟，为了计算CPU使用占比
		//end
		time(&current_time);
		local_t = localtime(&current_time);
		strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", local_t);
		fwrite(time_str, 1, strlen(time_str), fp_log);
		fputc('\n', fp_log);
		v_cpu2 = get_cpu_total_occupy();
		v_cpu_rate = cal_cpu_all_rate(v_cpu1, v_cpu2);//计算各个项目cpu占用率
		v_mem = get_os_mem();

#ifdef NO_PRI

		sprintf(log_line_buf,"[CPU] usr : %5.1f%c ,   sys : %5.1f%c ,   nic : %5.1f%c ,   idle: %5.1f%c ,     io  : %5.1f%c,   irq : %5.1f%c,   sirq: %5.1f%c\n",
												v_cpu_rate.user_ocu, 37, v_cpu_rate.system_ocu, 37, v_cpu_rate.nice_ocu, 37,
												v_cpu_rate.idle_ocu, 37, v_cpu_rate.iowait_ocu, 37, v_cpu_rate.irq_ocu, 37,
												v_cpu_rate.softirq_ocu, 37);

		fwrite(log_line_buf, 1, strlen(log_line_buf), fp_log);


		sprintf(log_line_buf,"[Mem] used:%7d KB, free:%7d KB, shrd:%7d KB, buff:%7d KB, cached: %7d KB\n",
												v_mem.memuse, v_mem.memfree, v_mem.shared, v_mem.buffers,
												v_mem.cached);

		fwrite(log_line_buf, 1, strlen(log_line_buf), fp_log);

#else


		printf("[CPU] usr : %5.1f%c ,   sys : %5.1f%c ,   nic : %5.1f%c ,   idle: %5.1f%c ,     io  : %5.1f%c,   irq : %5.1f%c,   sirq: %5.1f%c\n",
												v_cpu_rate.user_ocu, 37, v_cpu_rate.system_ocu, 37, v_cpu_rate.nice_ocu, 37,
												v_cpu_rate.idle_ocu, 37, v_cpu_rate.iowait_ocu, 37, v_cpu_rate.irq_ocu, 37,
												v_cpu_rate.softirq_ocu, 37);


		printf("[Mem] used:%7d KB, free:%7d KB, shrd:%7d KB, buff:%7d KB, cached: %7d KB\n",
												v_mem.memuse, v_mem.memfree, v_mem.shared, v_mem.buffers,
												v_mem.cached);

#endif
		//totalcputime2 =v_cpu2.total;
		//tt2 = (float)totalcputime2;
		for(j = 0; j < i; j++){//0开始到最后一个进程i

			test_unit[j].proc_cpu_time2 = get_cpu_proc_occupy(test_unit[j].pid);//此进程结束时刻的CPU使用时间
			if(test_unit[j].proc_cpu_time2 < 0){
				sprintf(err_buf,"[error] %s is not exist !!\n",test_unit[j].comm);
				fwrite(err_buf, 1, strlen(err_buf), fp_log);
				//break;//失败退出此次循环

			}
			ct1 = (float)test_unit[j].proc_cpu_time1;//转换成浮点，才可以计算百分率(开始时刻)
			ct2 = (float)test_unit[j].proc_cpu_time2;//转换成浮点(结束时刻的)
//add cpu占比更精准，更新总时间,位置
			v_cpu2 = get_cpu_total_occupy();//CPU使用情况(各个模块，如用户态、系统态、IO、中断)
			totalcputime2 =v_cpu2.total;//总的时间
			tt2 = (float)totalcputime2;//浮点转换

			if(0 != totalcputime2 - totalcputime1){ 
				test_unit[j].cpu_ocu  = 100.0 * (ct2 - ct1) / (tt2 - tt1);//计算
			}

			test_unit[j].vmrss = get_proc_mem(test_unit[j].pid);//获得物理内存
			test_unit[j].vmsize = get_proc_virtualmem(test_unit[j].pid);//获得虚拟内存		


		}
		while(j--){
#ifdef NO_PRI
//			sprintf(log_line_buf,"pid: %5d  name: %-20s vmrss: %7d KB  vmsize: %7d KB  cpu_rate: %.1f %c\n",test_unit[j].pid, test_unit[j].comm,
//																		test_unit[j].vmrss, test_unit[j].vmsize, test_unit[j].cpu_ocu,37);
			sprintf(log_line_buf,"pid: %5d  %-36s vmrss: %7d KB  vmsize: %7d KB  cpu: %.1f %c\n",test_unit[j].pid, test_unit[j].comm,
																		test_unit[j].vmrss, test_unit[j].vmsize, test_unit[j].cpu_ocu,37);

			fwrite(log_line_buf, 1, strlen(log_line_buf), fp_log);
#else
//		printf("pid: %5d	name: %-20s vmrss: %7d KB  vmsize: %7d KB  cpu_rate: %.1f %c\n",test_unit[j].pid, test_unit[j].comm,
//																	test_unit[j].vmrss, test_unit[j].vmsize, test_unit[j].cpu_ocu,37);
		printf("pid: %5d  %-36s vmrss: %7d KB  vmsize: %7d KB  cpu: %.1f %c\n",test_unit[j].pid, test_unit[j].comm,
																	test_unit[j].vmrss, test_unit[j].vmsize, test_unit[j].cpu_ocu,37);


#endif
		}
		fputc('\n', fp_log);
		fputc('\n', fp_log);
		fputc('\n', fp_log);

		//sleep(4);
		sleep(6);
	}

	fclose(fp_log);
	return 1;
}

