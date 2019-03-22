/* functions.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h> 		/* To access inode information of files */
#include <sys/types.h>
#include <dirent.h>			/* To access directories */
#include <unistd.h> 		/* For readlink */
#include <linux/kdev_t.h> 	/* To show tty minor device number */
#include <pwd.h> 			/* For user  */
#include <grp.h> 			/* For  group */
#include <limits.h> 		/* PATH_MAX */
#include <arpa/inet.h>

#include "functions.h"

int fileCount=0, dirCount=0, slnCount=0, fifoCount=0, charDevCount=0, socketsCount=0, blockDevFilesCount=0, totalFiles=0; 


int filetype[7]; //for printout2
time_t start, end;
int timemode;
char* filetypes;
pid_t pid;

List *myList;
int field;
int tcp;
int udp;

/* 1 */
int cnt(char* directory, char* program) {
	
	struct stat mybuf;
	float percentage;

	if(checkDir(directory,&mybuf)){
		fprintf(stderr,"Usage : %s -cnt <directory>\n", program);
		return 1;
	}
	else {
		//printf("Valid!\n");
		list(directory,CNT);
	}

	percentage = (fileCount/ (float)(totalFiles))*100;
	printf("\nnumber of regular files:\t %d \t%2.0f%%\n",fileCount,percentage);
	
	percentage = (dirCount/ (float)(totalFiles))*100;
	printf("number of directories:\t\t %d \t%2.0f%%\n",dirCount,percentage);
	
	percentage = (slnCount/ (float)(totalFiles))*100;
	printf("number of symbolic links:\t %d \t%2.0f%%\n",slnCount,percentage);
	
	percentage = (fifoCount/ (float)(totalFiles))*100;
	printf("number of named pipes:\t\t %d \t%2.0f%%\n",fifoCount,percentage);
	
	percentage = (charDevCount/ (float)(totalFiles))*100;
	printf("number of char devices:\t\t %d \t%2.0f%%\n",charDevCount,percentage);
	
	percentage = (socketsCount/ (float)(totalFiles))*100;
	printf("number of sockets:\t\t %d \t%2.0f%%\n",socketsCount,percentage);
	
	percentage = (blockDevFilesCount/ (float)(totalFiles))*100;
	printf("number of block device files:\t %d \t%2.0f%%\n",blockDevFilesCount,percentage);
	
	printf("\nnumber of total files:\t %d\n\n",totalFiles);

	return 0;

}

/* 2 */
int inTimeframe(char* timeframe,char* directory,int mode,char* types, char* program){

	struct stat mybuf;
	char timeUnit;
	int timeValue, i;
	

	int foundType=0;
	timemode = mode;
	filetypes = types;


	/* Check directory */
	if(checkDir(directory,&mybuf)){
		fprintf(stderr,"Usage : %s -type [f|d|l|p|c|b|s] -[lr|lw] -<time>[d|h|m|s] <directory>\n",program);
		return 1;
	}

	//printf("Valid dir!\n");

	/* Find types of files */
	if(types == NULL){
		printf("Will show results for all types of files\n");
	}
	else{
		for(i=0; i < 7 ; i++){
			filetype[i] = 0;
		}
		for(i=0 ; i< strlen(types) ; i++){
			switch (types[i]){
				case 'f':	filetype[REGFILE] =1;
							break;
				case 'd':	filetype[DIRECTORY] =1;
							break;
				case 'l':	filetype[LINK] =1;
							break;
				case 'p':	filetype[PIPE] =1;
							break;
				case 'c':	filetype[CHAR] =1;
							break;																	
				case 'b':	filetype[BLOCK] =1;
							break;
				case 's':	filetype[SOCKET] =1;
							break;																								
				default:	break;
			}
		}
		printf("Types:");
		for(i=0; i < 7 ; i++){
			printf("%d",filetype[i]);
			if (filetype[i] == 1) {
				foundType =1;
			}
		}
		if(!foundType){

			fprintf(stderr,"Usage : %s -type [f|d|l|p|c|b|s] -[lr|lw] -<time>[d|h|m|s] <directory>\n",program);
			return 1;
		}
	}



	/* Find time unit  and value from  timeframe */
	//printf("strlen %d\n",strlen(timeframe));
	timeUnit =  timeframe[strlen(timeframe)-1];
	timeValue = atoi(timeframe+1);
	//printf("Time value %d\n",timeValue);
	//printf("Time unit %c\n",timeUnit);

	/* Find start and end time of range */
	end = time(0);	
	if (timeUnit == 'd'){		
		start = end - timeValue*24*60*60;	
	}
	else if( timeUnit == 'h'){
		start= end - timeValue*60*60;
	}
	else if( timeUnit == 'm'){
		start= end - timeValue*60;
	}
	else if( timeUnit == 's'){
		start= end - timeValue;
	}
	else {
		fprintf(stderr,"Time unit not valid!\n");
		return 1;
	}

	printf("Start %s\n",ctime(&start));
	printf("End %s\n",ctime(&end));

	checkRange(&mybuf,start,end,mode);
	list(directory,TIME);

	return  0;

}

/* 3 */
int ps() {

	struct stat mybuf;
	int uid;

	checkDir(".",&mybuf);
	uid = mybuf.st_uid;	

	printf("\n  PID  PPID\t  CPU\t  STATE\t START\t     VSZ\t  RSS    PRIORITY  CMDLINE\n\n");

	listProc("/proc",uid, PS);

	return 0;
}

/* 4 */
int ft() {

	struct stat mybuf;
	int uid;

	checkDir(".",&mybuf);
	uid = mybuf.st_uid;	

	createList(&myList);

	listProc("/proc",uid,FT);
	printf("\nTotal open files: %d\n\n",totalFiles);

	destroyList(myList);

	return 0;
}

int iostat(int records, char *fieldname) {

	struct stat mybuf;
	int uid;

	checkDir(".",&mybuf);
	uid = mybuf.st_uid;

	if ( strcmp(fieldname,"PID") == 0 ) {
		field = PID;
	}
	else if ( strcmp(fieldname,"RCHAR") == 0 ) {
		field = RCHAR;
	}
	else if ( strcmp(fieldname,"WCHAR") == 0 ) {
		field = WCHAR;
	}
	else if ( strcmp(fieldname,"SYSR") == 0 ) {
		field = SYSR;
	}
	else if ( strcmp(fieldname,"SYSW") == 0 ) {
		field = SYSW;
	}
	else if ( strcmp(fieldname,"RFS") == 0 ) {
		field = RFS;
	}
	else if ( strcmp(fieldname,"WFS") == 0 ) {
		field = WFS;
	}
	else {
		fprintf(stderr, "Please use a valid fieldname: [PID|RCHAR|WCHAR|SYSR|SYSW|RFS|WFS]\n" );
		return 1;
	}

	createList(&myList);

	printf("\n    PID      RCHAR      WCHAR\t  SYSR\t     SYSW\t RFS\t    WFS\n\n");

	listProc("/proc",uid, IO);
	printf("\n");
	printList(myList,records);

	destroyList(myList);

	return 0;
}

int netstat(int isTCP, int isUDP){

	
	struct stat mybuf;
	int uid;

	checkDir(".",&mybuf);
	uid = mybuf.st_uid;
	tcp = isTCP;
	udp = isUDP;
	
	printf("\n%8s %6s %15s %7s %15s %7s %8s\n\n","PROTOCOL","PID","L-ADDRESS","L-PORT","R-ADDRESS","R-PORT","INODE");
	listProc("/proc",uid,NET);


	return 0;
}



void printout(char *name){
	
	struct stat mybuf ;
	char type, perms[10];
	int i,j;
	
	//stat(name, &mybuf);
	if (lstat(name, &mybuf ) < 0) {
			perror(" Failed to get directory status in printout\n");
			exit(2) ;
	}

	totalFiles++;
	
	switch(mybuf.st_mode & S_IFMT){
		
		case S_IFREG: 	//printf("%s: regular file!\n",name);
						fileCount++; 
						break;
		case S_IFDIR:	//printf("%s: directory!\n",name);
						dirCount++;
						list(name,CNT);				
						break;
		case S_IFLNK:	//printf("%s: symbolic link!\n",name); 
						slnCount++;
						break;
		case S_IFIFO:	//printf ("%s named pipe!\n",name);
						fifoCount++;
						break;
		case S_IFSOCK:	//printf ("%s socket!\n",name);
						socketsCount++;
						break;
		case S_IFCHR:	//printf ("%s char device!\n",name);
						charDevCount++;
						//break;	
		case S_IFBLK:	printf ("%s block device file!\n",name);
						blockDevFilesCount++;
						break;																			
		default: 		type = '?';
						break;
	}
}


void printout2(char *name){
	
	struct stat mybuf ;

	if (lstat(name, &mybuf ) < 0) {
		perror(" Failed to get directory status in printout\n");
		exit(EXIT_FAILURE) ;
	}

	
	switch(mybuf.st_mode & S_IFMT){
		
		case S_IFREG: 	//printf("%s: Regular file!\n",name);
						if((filetypes ==NULL)  || filetype[REGFILE]){ 
							//printf("Acces Time: %sModification Time: %s\n",ctime(&mybuf.st_atime),ctime(&mybuf.st_mtime));
							if(checkRange(&mybuf,start,end,timemode) == 0){
								printf("%s \n",name);
							}
						}
						break;
		case S_IFDIR:	if((filetypes ==NULL)  || filetype[DIRECTORY]){ 
							//printf("Acces Time: %sModification Time: %s\n",ctime(&mybuf.st_atime),ctime(&mybuf.st_mtime));
							if(checkRange(&mybuf,start,end,timemode) == 0){
								printf("%s \n",name);
							}
						}
						list(name,TIME);				
						break;
		case S_IFLNK:	if((filetypes ==NULL)  || filetype[LINK]){ 
							//printf("Acces Time: %sModification Time: %s\n",ctime(&mybuf.st_atime),ctime(&mybuf.st_mtime));
							if(checkRange(&mybuf,start,end,timemode) == 0){
								printf("%s \n",name);
							}
						}
						break;
		case S_IFIFO:	if((filetypes ==NULL)  || filetype[PIPE]){ 
							//printf("Acces Time: %sModification Time: %s\n",ctime(&mybuf.st_atime),ctime(&mybuf.st_mtime));
							if(checkRange(&mybuf,start,end,timemode) == 0){
								printf("%s \n",name);
							}
						}
						break;
		case S_IFSOCK:	if((filetypes ==NULL)  || filetype[SOCKET]){ 
							//printf("Acces Time: %sModification Time: %s\n",ctime(&mybuf.st_atime),ctime(&mybuf.st_mtime));
							if(checkRange(&mybuf,start,end,timemode) == 0){
								printf("%s \n",name);
							}
						}
						break;
		case S_IFCHR:	if((filetypes ==NULL)  || filetype[CHAR]){ 
							//printf("Acces Time: %sModification Time: %s\n",ctime(&mybuf.st_atime),ctime(&mybuf.st_mtime));
							if(checkRange(&mybuf,start,end,timemode) == 0){
								printf("%s \n",name);
							}
						}
						break;	
		case S_IFBLK:	if((filetypes ==NULL)  || filetype[BLOCK]){
							//printf("Acces Time: %sModification Time: %s\n",ctime(&mybuf.st_atime),ctime(&mybuf.st_mtime));
							if(checkRange(&mybuf,start,end,timemode) == 0){
								printf("%s \n",name);
							}
						}
						break;																			
		default: 		break;
	}

}


void printout3(char *name){
	
	struct stat mybuf ;
	char type, perms[10];
	int i,j;
	struct passwd* pass;
	struct group* gr;
    ssize_t r;
    char actualpath[PATH_MAX + 1];
    Record * rec;
    int exists=0;


	if (lstat(name, &mybuf ) < 0) { 
			perror(" Failed to get directory status in printout\n");
			exit(2) ;
	}

	switch(mybuf.st_mode & S_IFMT){
		
		case S_IFREG: 	type = '-';
						//printf("%s: regular file!\n",name);
						break;
		case S_IFDIR:	type = 'd';
						//printf("%s: directory!\n",name);
						//list(name,CNT);				
						break;
		case S_IFLNK:	type = 'l';
						//printf("%s: symbolic link!\n",name); 
						break;
		case S_IFIFO:	type = 'p';
						//printf ("%s named pipe!\n",name);
						break;
		case S_IFSOCK:	type = 's';
						//printf ("%s socket!\n",name);
						break;
		case S_IFCHR:	type = 'c';
						//printf ("%s char device!\n",name);
						break;	
		case S_IFBLK:	type = 'b';
						//printf ("%s block device file!\n",name);
						break;																			
		default: 		type = '?';
						break;
	}
	
	pass = getpwuid(mybuf.st_uid);
	gr = getgrgid(mybuf.st_gid);

   realpath(name, actualpath); //idio me linkname vgainei!! dn xreiazetai readlink
   
   /* Create record */
   rec = malloc(sizeof(Record));
   rec->type = type;
   rec->mode = mybuf.st_mode;
   rec->nlinks = mybuf.st_nlink;
   strcpy(rec->user,pass->pw_name);
   strcpy(rec->group,gr->gr_name);
   rec->size = (long int)mybuf.st_size;
   strcpy(rec->path,actualpath);

   exists = insertList(myList,rec,actualpath); //TODO printlist
   if(!exists) {
   		printRecord(rec);
   		totalFiles++;

	}
	else {
		free(rec);
	}
   
}

int printout4(char *name, int pid){
	
	struct stat mybuf ;
	struct stat mybuf2 ;
	char type, perms[10];
	int i,j;

	struct passwd* pass;
	struct group* gr;

    ssize_t r;
    char actualpath[PATH_MAX + 1];

    Record * rec;
    int exists=0;

   char *token, *token2;
   long int inode;
   FILE *fp;
   char line[512];
   // char *sl, *local, *rem, *st, *queue, *when, *ret, *uid, *timeout, *inode;
   char sl[5],local[15],rem[15],st[3],queue[18],when[12],ret[9],uid[6],timeout[5],inodeStr[7];
   char *l_address, *l_port, *r_address, *r_port;


	if (stat(name, &mybuf ) < 0) { 
			perror(" Failed to get status\n");
			exit(2) ;
	}
	
	if( (mybuf.st_mode & S_IFMT) == S_IFSOCK ){

		realpath(name, actualpath); //idio me linkname vgainei!! dn xreiazetai readlink
		//printf("%s ---> %s\n", name, actualpath);

		/* get the first token */
   		token = strtok(actualpath,"[");
   
   		/* walk through other tokens */
   		if( token != NULL ) {
			token2 = strtok(NULL,"]");
   		}
   		if( token2 != NULL){
   			inode = atol(token2);		
   			//printf("%ld inode\n",inode);
   		}

	}
	int l_ad, r_ad, l_p, r_p;
	struct in_addr addrL , addrR;

	////////////////////////
	if (tcp) {
		fp = fopen("/proc/net/tcp","r");
		if( fp == NULL){
			perror("fopen tcp"); return 1;
		}
		//printf("Will read tcp\n");
		fgets(line,512, fp); //ignore first
		while (1) {
        	if (fgets(line,512, fp) == NULL) break;
        		i++;
        		//printf("%3d: %s", i, line);
        		sscanf(line,"%s %s %s %s %s %s %s %s %s %s",sl,local,rem,st,queue,when,ret,uid,timeout,inodeStr);
        		//printf("%s %s %s %s %s %s %s %s %s %s\n",sl,local,rem,st,queue,when,ret,uid,timeout,inodeStr);
        		if(atol(inodeStr) == inode) {
         			//printf("PID %d has inode %ld in TCP!!!!\n",pid,inode);
               		l_address = strtok(local,":");
         			l_port = strtok(NULL," ");
         			sscanf(l_address,"%x",&l_ad);
         			//printf("@ %s is %0x in hex\n",l_address,l_ad);
         			sscanf(l_port,"%x",&l_p);
         			//printf("port %s is %x in hex and %d in dec\n",l_port,l_p,l_p);
					addrL.s_addr = htonl(l_ad); // s_addr must be in network byte order 
					char *local_address = inet_ntoa(addrL); // --> "10.1.2.3"
    			
					r_address = strtok(rem,":");	
         			r_port = strtok(NULL," ");
         			sscanf(r_address,"%x",&r_ad);
         			//printf("@ %s is %0x in hex\n",r_address,r_ad);
         			sscanf(r_port,"%x",&r_p);
         			//printf("port %s is %x in hex and %d in dec \n",r_port,r_p, r_p);
					addrR.s_addr = htonl(r_ad); // s_addr must be in network byte order 
					char *remote_address = inet_ntoa(addrR); // --> "10.1.2.3"


					printf("TCP\t %6d %15s %7d %15s %7d %8ld\n",pid,local_address,l_p,remote_address, r_p,inode);
         			break;
        		}
		}
		fclose(fp);
	}
	if (udp) {
		fp = fopen("/proc/net/udp","r");
		if( fp == NULL){
			perror("fopen udp"); return 1;
		}
		//printf("Will read udp\n");

		fgets(line,512, fp); //ignore first
		while (1) {
        	if (fgets(line,512, fp) == NULL) break;
        		i++;
        		//printf("%3d: %s", i, line);
        		sscanf(line,"%s %s %s %s %s %s %s %s %s %s",sl,local,rem,st,queue,when,ret,uid,timeout,inodeStr);
        		//printf("%s %s %s %s %s %s %s %s %s %s\n",sl,local,rem,st,queue,when,ret,uid,timeout,inodeStr);
        		if(atol(inodeStr) == inode){
        			//printf("PID %d has inode %ld in UDP!!!!\n",pid,inode);
         			l_address = strtok(local,":");
         			l_port = strtok(NULL," ");
         			sscanf(l_address,"%x",&l_ad);
         			//printf("@ %s is %0x in hex\n",l_address,l_ad);
         			sscanf(l_port,"%x",&l_p);
         			//printf("port %s is %x in hex and %d in dec\n",l_port,l_p,l_p);
					addrL.s_addr = htonl(l_ad); // s_addr must be in network byte order 
					char *local_address = inet_ntoa(addrL); // --> "10.1.2.3"
    			
					r_address = strtok(rem,":");	
         			r_port = strtok(NULL," ");
         			sscanf(r_address,"%x",&r_ad);
         			//printf("@ %s is %0x in hex\n",r_address,r_ad);
         			sscanf(r_port,"%x",&r_p);
         			//printf("port %s is %x in hex and %d in dec \n",r_port,r_p, r_p);
					addrR.s_addr = htonl(r_ad); // s_addr must be in network byte order 
					char *remote_address = inet_ntoa(addrR); // --> "10.1.2.3"


         			printf("UDP\t %6d %15s %7d %15s %7d %8ld\n",pid,local_address,l_p,remote_address, r_p,inode);
 
        		}
		}
		//
		fclose(fp);

	}


   // sl  local_address  rem_address   st tx_queue rx_queue tr tm->when retrnsmt   uid  timeout inode                                                     
   // 0:   00000000:1389 00000000:0000 0A 00000000:00000000 00:00000000 00000000  1000        0 37371 

}




	









