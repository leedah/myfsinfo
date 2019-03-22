/* list.h */

#ifndef LIST_H_
#define LIST_H_

#define MAX_USR_NAME 31
#define MAX_GR_NAME 31

#include "utils.h"

enum fieldtype { PID, RCHAR, WCHAR, SYSR, SYSW, RFS, WFS} ;

typedef struct Record{
	
	char 		type;
	mode_t 		mode;
	int 		nlinks;
	char		user[MAX_USR_NAME];
	char		group[MAX_GR_NAME];
	long int	size;	
	char  		path[PATH_MAX + 1];

} Record;


typedef struct Record2{
	
	pid_t				PID;
	unsigned long int 	RCHAR;
	unsigned long int 	WCHAR;
	unsigned long int 	SYSR;
	unsigned long int 	SYSW;
	unsigned long int 	RFS;
	unsigned long int 	WFS;

} Record2;

typedef struct Node{
	void* record;
	struct Node* next;
	struct Node* previous;
} Node;

typedef struct List{
	Node* head;
	Node* last;
	int size;
} List;

int createList(List** list);
int destroyList(List* list);

int insertList(List* list, Record* record, char *path); /* If path already exists in list, returns 1 */
void insertOrderedList(List* list,Record2* record, int field) ;
int compareRecs(Record2* rec, Record2* curr, int field);

int printList(List* list, int records);
void printRecord(Record* record);
void printRecord2(Record2* record);

#endif