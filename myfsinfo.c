/* myfsinfo.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>   /* To access inode information of files */
#include <sys/types.h>
#include <dirent.h>     /* To access directories */

#include "functions.h"


int main (int argc ,char ** argv ){

    char* directory=NULL;
    int lwFlag=0, lrFlag = 0;
    char *timeframe=NULL;       /* Period of time */
    char* types=NULL;
    int i,j, mode;
    int err=-1;
    int records =0;
    char* fieldname = NULL;
    
    /* Manage use cases */
    for (i = 0; i < argc; i++) {
        
        /* Case 1 */
        if(strcmp(argv[i],"-cnt") == 0 ) {  
            directory = argv[i+1];
            err = cnt(directory,argv[0]);
        }

        /* Case 2 */
        else if( (strcmp (argv[i],"-lr") ==0) || (strcmp (argv[i],"-lw") ==0)) {

            timeframe = argv[i+1];
            //printf("%s \n",timeframe);
            if(strcmp (argv[i],"-lr") ==0) //read
                mode = LR;
            else
                mode = LW;
            directory = argv[i+2];
            if(argc > 4) {
                if(strcmp (argv[1],"-type") ==0){
                    if(strcmp (argv[i],argv[2]) ==0){
                        fprintf(stderr, "You have to specify the type of file after the -type argument or omit it!\n");
                        err =1;                 
                        fprintf(stderr,"Usage : %s -type [f|d|l|p|c|b|s] -[lr|lw] -<time>[d|h|m|s] <directory>\n",argv[0]);
                        break;
                    }
                    types= argv[2];

                }
                else {
                    err =1;
                    fprintf(stderr,"Usage : %s -type [f|d|l|p|c|b|s] -[lr|lw] -<time>[d|h|m|s] <directory>\n",argv[0]);
                    break;
                }
            }
            err = inTimeframe(timeframe,directory,mode,types,argv[0]);
        
        }

        /* Case 3 */
        else if( strcmp (argv[i],"-ps") ==0) {
            err=ps();

        }
        /* Case 4 */
        else if( strcmp (argv[i],"-ft") ==0) {
            err=ft();
        }
        /* Case 5*/
        else if( strcmp (argv[i],"-iostat") ==0) {
            if (argc == 6){
                for(j = i+1 ; j< argc ; j++){           
                    if(strcmp(argv[j],"-k") == 0){
                        records = atoi(argv[j+1]);
                    }
                    else if(strcmp(argv[j],"-f") == 0){
                        fieldname = argv[j+1];
                    }
                }
            }
            else{
                fprintf(stderr,"Usage : %s -iostat -k <#records> -f <fieldname>\n",argv[0]);
                err =1; break;
            }
            //printf("Records %d fieldname %s\n",records,fieldname);
            err=iostat(records,fieldname);
        }
        /* Case 6 */
        else if( strcmp (argv[i],"-netstat") ==0) {     
            if(argc ==2) {
                err = netstat(1,1);
            }
            else  {
                if( strcmp (argv[i+1],"tcp") ==0) {
                    err = netstat(1,0);
                }
                else if( strcmp (argv[i+1],"udp") ==0) {
                    err = netstat(0,1);
                }
                else {
                    fprintf(stderr,"Usage : %s -netstat [tcp|udp]\n",argv[0]);
                    err =1; break;
                }
            }
        }

    }

    /* One does not simply exit */
    if(err == 1){
        exit(EXIT_FAILURE);
    }
    else if(err == 0){
        exit(EXIT_SUCCESS);
    }

    fprintf(stderr,"\nPlease run the program with one of the following ways:\n\n");
    fprintf(stderr,"Usage : %s -cnt <directory>\n",argv[0]);
    fprintf(stderr,"Usage : %s -type [f|d|l|p|c|b|s] -[lr|lw] -<time>[d|h|m|s] <directory>\n",argv[0]);
    fprintf(stderr,"Usage : %s -ps\n",argv[0]);
    fprintf(stderr,"Usage : %s -ft\n",argv[0]);
    fprintf(stderr,"Usage : %s -iostat -k <#records> -f <fieldname>\n",argv[0]);
    fprintf(stderr,"Usage : %s -netstat [tcp|udp]\n",argv[0]);
    
    exit(EXIT_FAILURE);
}


