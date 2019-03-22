/* functions.h */

#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

#include "utils.h"
#include "list.h"

/* Main functionalities of the program */
int cnt(char* directory, char* name);
int inTimeframe(char* timeframe,char* directory,int mode,char* types, char* program);
int ps();
int ft();
int iostat(int records, char *fieldname);


void printout(char *name);
void printout2(char *name);
void printout3(char *name);
int printout4(char *name,int pid);

void do_ls(char dirname []);


#endif