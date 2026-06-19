#include<stdlib.h>                                                                                                                                                                                     
#include<string.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/time.h>
#include "getMemInfo.h"


//系统的内存信息
Total_Mem_Info_t get_os_mem(void){

    FILE *fp;
    char line_buff[512] = {0};
	Total_Mem_Info_t ret_m;
    fp=fopen("/proc/meminfo","r");
	if (!fp) {
		perror("/proc/meminfo");
		return ret_m;//^^
	} 
    char name[64];
    unsigned int temp;
	int i = 0;
    for (i = 0; i < MEMINFO_LINE; i++){
		switch (i)
		{
			case TOTAL:
				fgets(line_buff, sizeof(line_buff), fp);
				sscanf(line_buff, "%s %d", name, &ret_m.memtotal);
				break;
			case FREE:
				fgets(line_buff, sizeof(line_buff), fp);	
				sscanf(line_buff, "%s %d", name, &ret_m.memfree);
				break;
			case BUFFERS:
				fgets(line_buff, sizeof(line_buff), fp);
				sscanf(line_buff, "%s %d", name, &ret_m.buffers);
				break;
			case CACHED:
				fgets(line_buff, sizeof(line_buff), fp);
				sscanf(line_buff, "%s %d", name, &ret_m.cached);
				break;
			case SHARED:
				fgets(line_buff, sizeof(line_buff), fp);
				sscanf(line_buff, "%s %d", name, &ret_m.shared);
				break;

			default:
				fgets(line_buff, sizeof(line_buff), fp);
				break;

		}
        
    }  
	ret_m.memuse = ret_m.memtotal - ret_m.memfree;
    fclose(fp);
    return ret_m;
}
 
//

//获取进程占用内存
unsigned int get_proc_mem(unsigned int pid){
    
    char file_name[64] = {0};
    FILE *fp;
    char line_buff[512] = {0};
    sprintf(file_name,"/proc/%d/status", pid);
    
    fp =fopen(file_name, "r");
	if (!fp) {
		perror(file_name);
		return 0;
	}

    
    char name[64];
    int vmrss;
	int i = 0;
    for (i = 0; i < VMRSS_LINE - 2; i++){
        fgets(line_buff, sizeof(line_buff), fp);
    }                                                                                                                                                                                                  

    fgets(line_buff,sizeof(line_buff),fp);
    sscanf(line_buff,"%s %d",name,&vmrss);
    fclose(fp);
    return vmrss;
}
 
 
//获取进程占用虚拟内存
unsigned int get_proc_virtualmem(unsigned int pid){
    
    char file_name[64] = {0};
    FILE *fp;
    char line_buff[512] = {0};
    sprintf(file_name, "/proc/%d/status", pid);
    
    fp =fopen(file_name, "r");
	if (!fp) {
		perror(file_name);
		return 0;
	}

    
    char name[64];
    int vmsize;
	int i = 0;
    for (i = 0; i < VMSIZE_LINE - 2; i++){
        fgets(line_buff, sizeof(line_buff), fp);
    }
    
    fgets(line_buff, sizeof(line_buff), fp);
    sscanf(line_buff, "%s %d", name, &vmsize);
    fclose(fp);
 
    return vmsize;
}

