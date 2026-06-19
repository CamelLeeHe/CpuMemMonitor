#ifndef _GET_THREADS_H
#define _GET_THREADS_H
#include <dirent.h>
#include <sys/stat.h>

int getPid(char *processName);
int is_pid_folder(const struct dirent *entry);


#endif

