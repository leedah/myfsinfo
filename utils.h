/* utils.h */

#ifndef UTILS_H_
#define UTILS_H_

#include <limits.h> 		/* PATH_MAX */
#include <sys/stat.h>		/* To access inode information of files */
#include "list.h"

enum mode { LR=1, LW };
enum function { CNT=1, TIME, PS, FT, IO, NET };
enum type { REGFILE, DIRECTORY, LINK, PIPE, CHAR, BLOCK, SOCKET} ;


typedef struct ProcessInfo {

    pid_t pid;								/* (1)  Process id */
	char comm[100];							/* (2)  Filename of the executable */
	char state;								/* (3)  State of process(R,S,Z,..)	*/
	pid_t ppid;								/* (4)  Parent PID */
	int pgrp;								/* (5)  Group of process */
	int session;							/* (6)  Session id */
	int tty_nr;								/* (7)  TTY of process */
	int tpgid;								/* (8)  Foreground process group id */
	unsigned int flags;						/* (9)  Kernel Flags */
	unsigned long int minflt;				/* (10) Minor faults */
	unsigned long int cminflt;				/* (11) Minor faults with childs */
    unsigned long int majflt;				/* (12) Major faults */
 	unsigned long int cmajflt;				/* (13) Major faults with childs */
 	unsigned long int utime;				/* (14) user mode jiffies */
    unsigned long int stime;				/* (15) Kernel mode jiffies */
    long int cutime;						/* (16) User mode jiffies with childs */
    long int cstime;						/* (17) Kernel mode jiffies with childs */
    long int priority;                 		/* (18) Process priority */
    long int nice;							/* (19) Priority range 19 (low) to -20 (high) */
    long int num_threads;					/* (20) Number of threads in the process */
    long int itrealvalue;              		/* (21) The time before the next SIGALRM is sent to the process */
  	unsigned long long int starttime;		/* (22) (was %lu before Linux 2.6) Time started after system boot in jiffies */
  	unsigned long int vsize;				/* (23) Virtual memory size */
 	long int rss;                      		/* (24) Resident Set Size */
		
} ProcessInfo;


/* Utilities */
int checkDir(char* directory,struct stat* mybuf);
int checkRange(struct stat* mybuf,time_t start,time_t end, int mode);

void list(char *name, int function);
int  listProc(char *name, int uid, int function );

int findProcessInfo(char *name);
int findFileInfo(char *name);
int findIO(char* name, int pid);

#endif