#ifndef _GETMEMINFO_H
#define _GETMEMINFO_H

//#define VMRSS_LINE 17
//#define VMSIZE_LINE 13

//MPSoC
#define VMRSS_LINE 19
#define VMSIZE_LINE 15


#define MEMINFO_LINE 24

typedef struct{
    unsigned int memtotal;
	unsigned int memuse;
    unsigned int memfree;
    unsigned int buffers;
	unsigned int shared;
    unsigned int cached;
}Total_Mem_Info_t;

enum OsMem_T{
	TOTAL,
	FREE,
	BUFFERS,
	CACHED,
	SHARED = 23
};


unsigned int get_proc_mem(unsigned int pid);
unsigned int get_proc_virtualmem(unsigned int pid);
Total_Mem_Info_t get_os_mem(void);



#endif

