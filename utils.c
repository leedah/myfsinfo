/* utils.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <dirent.h>         /* To access directories */
#include <unistd.h>         /* For sysconf */
#include <linux/kdev_t.h>   /* To show tty minor device number */
#include "utils.h"


int clocksPerSec;
long int pagesize;
int tty;

long  int CPUcount;
long long int VSZcount; 


extern List *myList;
extern int field;


int findProcessInfo(char* name) {

    DIR *dp;
    struct dirent *dir;
    char *newname, *newname2, *newname3;
    static char* originalDir;
    struct stat mybuf ;

    char linebuf[4096];
    FILE* fd, *fd3;
    ProcessInfo p;

    long  int rss;          /* Size of rss (number of pages * pagesize) in Kbytes */
    long int clockTicks;
    float cpu;              /* Total CPU time in secs */
    struct tm *t;
    time_t timerunning, starttime, timestarted, uptime;

    /* Open proc/[pid] directory */
    if(( dp= opendir(name))== NULL ){
        perror("opendir"); return ;
    }

    if((dir = readdir(dp)) == NULL ){
        perror("readdir"); return;
    }

    /* Open proc/[pid]/stat file */
    newname =(char *)malloc(strlen(name)+ strlen("/stat")) ;
    strcpy(newname,name);
    strcat(newname,"/stat");
    fd = fopen(newname,"r");


    /* Read info (1)(2)(3)(4)(5)(6)(7)(8)(9)(10)(11)(12)(13)(14)(15)(16)(17)(18)(19)(20)(21)(22) (23)(24) */
    fscanf( fd, "%d %s %c %d %d %d %d %d %u %lu %lu %lu %lu %lu %lu %ld %ld %ld %ld %ld %ld %llu %lu %ld",
    &p.pid,p.comm,&p.state, &p.ppid, &p.pgrp, &p.session, &p.tty_nr, &p.tpgid, &p.flags, &p.minflt, &p.cminflt,
    &p.majflt, &p.cmajflt, &p.utime, &p.stime, &p.cutime, &p.cstime, &p.priority, &p.nice, &p.num_threads, &p.itrealvalue,
    &p.starttime, &p.vsize, &p.rss);
    ////////////////////////////////

    if((tty == p.tty_nr)) { //id ppid =ppid or pid (invoking process)
    /* Find RSS */
    //printf("PAGESIZE:%ld p.ss %ld\n",pagesize,p.rss);
    rss = pagesize*p.rss/1000;
    //printf("rss %ld\n",rss);

    /* Find CPU */
    clockTicks = p.utime + p.stime + p.cutime+ p.cstime;
    cpu = clockTicks / (((float)clocksPerSec)) ;
    CPUcount += clockTicks;
    //printf("Total CPU is %ld / %ld = %.1fs\n",clockTicks,sysconf(_SC_CLK_TCK),cpu);

    /* Find start time */
    starttime = p.starttime / clocksPerSec ; /* secs after boot did the process started */
    //printf("starttime in secs %ld\n",starttime);
    //how many secs ago did the system boot?
        //uptime..
        newname3 =(char *)malloc(strlen("/proc/uptime")+1) ; //TODO oxi uptime synexeia!!
        strcpy(newname3,"/proc/uptime");
        fd3 = fopen(newname3,"r");
        //printf("newname3 : %s\n",newname3);
        fscanf(fd3,"%ld",&uptime);
        //printf("uptime in secs %ld\n",uptime);
        fclose(fd3);
        free(newname3);
    //so for how many secs is the process running?
    timerunning = uptime-starttime; 
    //printf("timerunning= uptime-starttime : %ld\n",timerunning);
    timestarted = time(0)-timerunning; //now -secs
    //printf("timestarted = time(0)-timerunning : %ld\n",timestarted);

    t= localtime(&timestarted);
    //printf("Timestarted : %02d:%02d\n",t->tm_hour, t->tm_min);
    ///////////////////////////////////////////////
    VSZcount += p.vsize;

    printf("%5d %5d %8.1fs\t    %c\t %02d:%02d  %9lu   %9ld\t   %3ld\t  ",
    p.pid, p.ppid, cpu, p.state, t->tm_hour, t->tm_min, p.vsize/1000, rss, p.priority);


    fclose(fd);

    /* Find cmdline */
    newname2 =(char *)malloc(strlen(name)+ strlen("/cmdline")+1) ;
    strcpy(newname2,name);
    strcat(newname2,"/cmdline");
    //printf("newname2 %s\n",newname2);
    fd = fopen(newname2,"r");
    fread(linebuf,4096,1,fd);
    printf(" %s %d \n",linebuf,MINOR(p.tty_nr));
    fclose(fd);
    free(newname2);

    
    

    }//ppid same as invoking process
    free(newname); //TODO = NULL too
    closedir(dp);
}


int findFileInfo(char* name) {

    DIR *dp, *dp2;
    struct dirent *dir, *dir2;
    char *newname, *newname2, *newname3;
    static char* originalDir;
    struct stat mybuf ;
    char linebuf[4096];
    FILE* fd, *fd3;
    ProcessInfo p;

    if (( dp= opendir(name))== NULL ) {
        perror("opendir"); printf("%s\n",name); return ;
    }

    //read proc/pid
    if( (dir = readdir(dp)) == NULL ){
        perror("readdir"); return;
    }


    //open proc/pid/stat file to see tty
    newname =(char *)malloc(strlen(name)+ strlen("/stat")+1) ;
    strcpy(newname,name);
    strcat(newname,"/stat");
    fd = fopen(newname,"r");
          /*   (1)(2)(3)(4)(5)(6)(7) */
    fscanf(fd,"%d %s %c %d %d %d %d ",&p.pid,p.comm,&p.state, &p.ppid, &p.pgrp, &p.session, &p.tty_nr);
    fclose(fd);

    closedir(dp);
    
    if((tty == p.tty_nr)) { //id ppid =ppid or pid (invoking process)

        printf("\nPID %d\n",p.pid);
  
        newname2 =(char *)malloc(strlen(name)+ strlen("/fd")+1) ;
        strcpy(newname2,name);
        strcat(newname2,"/fd");
        //printf("tty  of this process is the same!Will search info in %s\n",newname2);
        //printf("newname2 %s\n",newname2);
        
        /********************************************************************************/
        /* Open proc/[pid]/fd directory */
        if (( dp2= opendir(newname2))== NULL ) {
            perror("opendir"); return ;
        }
        
        while (( dir2 = readdir(dp2)) != NULL ) {

            if (dir2->d_ino == 0 ) continue ;
            if ((strcmp(dir2->d_name,".") == 0) || ( strcmp(dir2->d_name,"..") == 0)) {
                //printf("Current or parent dir!\n");
                continue;
            }   
            
            newname3 =(char *)malloc(strlen(newname2)+ strlen(dir2->d_name)+2) ;

            strcpy(newname3,newname2);
            strcat(newname3,"/");
            strcat(newname3,dir2->d_name);

            //printf("Will printout3 file %s\n",newname3);
            printout3(newname3);

            free(newname3);

        }

        closedir(dp2);

        /********************************************************************************/
        free(newname2);
        newname2 = NULL;
        
    }//ppid same as invoking process

    free(newname); 
    newname = NULL;
}


int findNetInfo(char* name, int pid) {

    DIR *dp;
    struct dirent *dir, *dir2;
    char *newname, *newname2, *newname3;
    static char* originalDir;
    struct stat mybuf ;
    char linebuf[4096];
    FILE* fd, *fd3;
    ProcessInfo p;


    newname =(char *)malloc(strlen(name)+ strlen("/fd")+1) ;
    strcpy(newname,name);
    strcat(newname,"/fd");

    /* Open proc/[pid]/fd directory */
    if (( dp = opendir(newname))== NULL ) {
        return 1;
    }
        
    while (( dir = readdir(dp)) != NULL ) {

        if (dir->d_ino == 0 ) continue ;
        if ((strcmp(dir->d_name,".") == 0) || ( strcmp(dir->d_name,"..") == 0)) {
            //printf("Current or parent dir!\n");
            continue;
        }   
            
        newname2 =(char *)malloc(strlen(newname)+ strlen(dir->d_name)+2) ;

        strcpy(newname2,newname);
        strcat(newname2,"/");
        strcat(newname2,dir->d_name);

        //printf("Will printout4 file %s\n",newname2);
        if(pid != getpid()){ //TODO remove to see current process too
            printout4(newname2,pid);
        }
        
        free(newname2);

    }

    closedir(dp);
    free(newname);
    newname = NULL;     
}


int findIO(char* name, int pid) {

    DIR *dp;
    struct dirent *dir;
    char *newname, *newname2;
    struct stat mybuf ;

    char linebuf[4096];
    FILE* fd;
    ProcessInfo p;
    char fieldname[20];
    Record2 *rec;

 
    /* Open proc/[pid] directory */
    if(( dp= opendir(name))== NULL ){
        perror("opendir"); return ;
    }

    if((dir = readdir(dp)) == NULL ){
        perror("readdir"); return;
    }

    /* Open proc/[pid]/io file */
    newname =(char *)malloc(strlen(name)+ strlen("/io")) ;
    strcpy(newname,name);
    strcat(newname,"/io");
    fd = fopen(newname,"r");
    if( fd == NULL); return 1;

    //printf("Readind %s ,currren pid %d\n",newname,getpid());
    rec = malloc(sizeof(Record2));
    rec->PID = pid;
    
    /* Read IO info */
    fscanf(fd,"%s %lu",fieldname, &rec->RCHAR);
    fscanf(fd,"%s %lu",fieldname, &rec->WCHAR);
    fscanf(fd,"%s %lu",fieldname, &rec->SYSR);
    fscanf(fd,"%s %lu",fieldname, &rec->SYSW);
    fscanf(fd,"%s %lu",fieldname, &rec->RFS);
    fscanf(fd,"%s %lu",fieldname, &rec->WFS);

    //printRecord2(rec);    
    insertOrderedList(myList,rec,field) ;

    fclose(fd);
    free(newname);
    newname = NULL;
}

int listProc(char * name, int uid,int function ){
    
    DIR *dp;
    struct dirent *dir;
    char *newname;
    struct stat mybuf ;
    pid_t ppid;
    char pidStr[5];
    char* currName;
    FILE *fd;
    ProcessInfo p; //tty process info 

    CPUcount =0;
    VSZcount =0;
    pagesize = sysconf(_SC_PAGESIZE);  //or PAGE_SIZE
    clocksPerSec = sysconf(_SC_CLK_TCK);

    if (( dp= opendir(name))== NULL ) { //open proc
        return 1 ;
    }
    //printf("Listing directory %s\n",name);

    /* Get the tty of the invoking process (for PS, FT) */
    ppid = getppid();
    sprintf(pidStr,"%d",ppid);
    currName =(char *)malloc(strlen("/proc/")+ strlen("/stat")+strlen(pidStr)+1) ; //+5
    strcpy(currName,"/proc/");
    strcat(currName,pidStr);
    strcat(currName,"/stat");

    fd = fopen(currName,"r");

    fscanf(fd,"%d %s %c %d %d %d %d",&p.pid, p.comm, &p.state, &p.ppid, &p.pgrp, &p.session, &p.tty_nr);
    tty = p.tty_nr;

    //printf("TTY of invoking process %d\n",MINOR(tty)); //for FT
    fclose(fd);
    free(currName);

    /* Read /proc directory */
    while (( dir = readdir(dp)) != NULL ) {
        if (dir->d_ino == 0 ) continue ;
        if ((strcmp(dir->d_name,".") == 0) || ( strcmp(dir->d_name,"..") == 0)) {
            //printf("Current or parent dir!\n");
            continue;   
        }

        newname =(char *)malloc(strlen(name)+ strlen(dir->d_name)+2) ;
        strcpy(newname,name);
        strcat(newname,"/");
        strcat(newname,dir->d_name);


        if (lstat(newname, &mybuf ) < 0) {
            perror(" Failed to get directory status in printout\n");
            exit(2) ;
        }
        if(mybuf.st_uid == uid){
            if (function == PS) {
                findProcessInfo(newname);
            }
            else if( function == FT){
                //printf("Found dir %s\n",newname);
                findFileInfo(newname);
            }
            else if(function == IO){ 
                findIO(newname,atoi(dir->d_name));
            }
            else if(function == NET){
                //printf("PID %s\n",dir->d_name);
                findNetInfo(newname,atoi(dir->d_name));
            }
                
        }
        //////////////
        free (newname);
        //newname=NULL ;
    }
    if(function == PS){
        printf("\n-----------------------------\n");
        printf("Total memory usage Kb:\t %lld\n", VSZcount/1000);
        printf("Total cpu time sec’s:\t %.1f\n\n", CPUcount/((float)clocksPerSec));
    }

    closedir(dp);

}
void list(char * name, int function ){
    
    DIR *dp;
    struct dirent *dir;
    char *newname;
    static char* originalDir;

    if (( dp= opendir(name))== NULL ) {
        perror("opendir"); return ;
    }
    //printf("Listing directory %s\n",name);
    while (( dir = readdir(dp)) != NULL ) {
        if (dir->d_ino == 0 ) continue ;
        if ((strcmp(dir->d_name,".") == 0) || ( strcmp(dir->d_name,"..") == 0)) {
            //printf("Current or parent dir!\n");
            continue;
            
        }
        newname =(char *)malloc(strlen(name)+ strlen(dir->d_name)+2) ;
        strcpy(newname,name);
        strcat(newname,"/");
        strcat(newname,dir->d_name);
        //printf("Will printout newname: %s\n",newname);
        switch(function){
        
        case CNT:   printout(newname);
                    break;
        case TIME:  printout2(newname);
                    break;
        case PS:    printout(newname);
                    break;
        default:    printf("No valid function of program!\n");      
        }
        free (newname);
        newname=NULL ;
    }
    closedir(dp);
}

int checkDir(char* directory,struct stat* mybuf){
    
    if (directory != NULL){
            
        if (stat(directory, mybuf ) < 0) {
            perror("Failed to get directory status ");
            exit(EXIT_FAILURE);
        }
        if(( mybuf->st_mode & S_IFMT ) == S_IFDIR ){
            //printf("Directory encountered!\n");
            return 0;
        }       
        else {
            fprintf(stderr,"Argument must be a directory, not a file!\n");
            return 1;
        }
    }
    else {
        return 1;
    }
    return 0;
}

int checkRange(struct stat* mybuf,time_t start,time_t end, int mode){
    if(mode == LR){
        if(( mybuf->st_atime >= start) && ( mybuf->st_atime <= end ) ){
            //printf("Access time:%s\n",ctime(&mybuf->st_atime));
            return 0;
        }
    }
    else if(mode == LW){
        if(( mybuf->st_mtime >= start) && ( mybuf->st_mtime <= end ) ){
                //printf("Modification:%s\n",ctime(&mybuf->st_mtime));
                return 0;
        }
    }
    return 1;
}
