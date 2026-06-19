/**************************************************************
*  @file    getThreads.c
*  @brief     获取线程信息
*  @author  
*  @date    
*  @version v0.01
***************************************************************/


#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <ctype.h>

#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>


//判断是否为文件夹
int is_pid_folder(const struct dirent *entry) {
    const char *p;

    for (p = entry->d_name; *p; p++) {
        if (!isdigit(*p))//是否为10进制的字符
            return 0;
    }

    return 1;
}

//通过名字获取进程pid
int getPid(char *processName){
    unsigned long maj_faults;
	struct dirent *entry;
	char path[256 + 5 + 5]; // d_name + /proc + /stat
	char targetProcessNameBuff[256 + 5 + 5];
	char ProcessNameBuff[256 + 5 + 5];
	FILE *fp;
	int pid = -1;
	DIR *procdir;
	//char *ps_processName;
	
	sprintf(ProcessNameBuff, "(%s)", processName);

	int get_sccess_flag = 0;
    // Open /proc directory.
    procdir = opendir("/proc");
    if (!procdir) {
        perror("opendir failed");
        return 1;
    }
    while ((entry = readdir(procdir))) {
        // Skip anything that is not a PID folder.
        if (!is_pid_folder(entry))
            continue;

        // Try to open /proc/<PID>/stat.
        snprintf(path, sizeof(path), "/proc/%s/stat", entry->d_name);
        fp = fopen(path, "r");
        if (!fp) {
            perror(path);
            continue;
        }
        // Get PID, process name and number of faults.
        fscanf(fp, "%d %s %*c %*d %*d %*d %*d %*d %*u %*lu %*lu %lu",
            &pid, &targetProcessNameBuff, &maj_faults
        );

        // Pretty print.
        //printf("%5d %-20s: %lu\n", pid, path, maj_faults);
		if(strcmp(targetProcessNameBuff,ProcessNameBuff) == 0){
			
			//sprintf(target_path_task, "/proc/%s/task", entry->d_name);
			get_sccess_flag = 1;
			break;
		}
        fclose(fp);
    }

    closedir(procdir);
	if(get_sccess_flag){
		return pid;
	}else{
		return -1;
	}
	


}







