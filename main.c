#include<stdlib.h>                                                                                                                                                                                     
#include<string.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/time.h>
#include <time.h>


#include "getProcessPid.h"
#include "getCpuOccupy.h"
#include "getMemInfo.h"

void func1_print(void){

	printf("##func1_win# [%s %s]line: %d  \n",__DATE__,__TIME__,__LINE__);

}

int main(int argc, char *argv[])
{
	time_t current_time;
	struct tm *local_t;
	char time_str[64];
	//char log_bak_buf[64] = "OS_record.log.old";
	const char *log_bak = "/usr/codesys/hsc3_app/log/OS_record/OS_record.log.old";
	if(argc != 2){
		printf("Usage: %s <log_path_file> \n",argv[0]);
		return 0;
	}
	unsigned int delay_us = 1000 * 1000;
	printf("##window 222# [%s %s]line: %d  \n",__DATE__,__TIME__,__LINE__);

	while(1){
		priAllCpuMem(delay_us,argv[1]);
		
		time(&current_time);
		local_t = localtime(&current_time);
		strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", local_t);
		//printf("当前时间为: %s\n", time_str);
		//printf("delete %s \n",argv[1]);
		//remove(argv[1]);
		rename(argv[1],log_bak);

	}
	

	
    return 0;
}     

