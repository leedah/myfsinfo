/* list.c */

#include <stdio.h>
#include <stdlib.h>
#include "list.h"

int createList(List** list) {

    *list = NULL;
    *list = malloc(sizeof(List));
    if(*list == NULL) {
        printf("createList: Could not allocate memory for list");
        // return 1;
        exit(EXIT_FAILURE);
    }

    (*list)->head = NULL;
    (*list)->last = NULL;
    (*list)->size = 0;

    return 0;

}


int insertList(List* list,Record* record,char *path) {
    
    Node* current;
    Node* node;
    int j=1;
    int found = 0;

    if( list == NULL){
        printf("insertList: List is not created.Could not insert!\n");
        exit(EXIT_FAILURE);
    }
    /* Create  a new node */
    node = malloc(sizeof(Node));
    if(node == NULL) {
        perror("insertList: Could not allocate memory for new Node");
        exit(EXIT_FAILURE);
    }
    node->record = record;


    if (list->size == 0) { //list is empty
        //printf("List empty!Inserting first..\n");
        node->next = NULL;
        node->previous = NULL;
        list->head = node;
        list->last = node;
    }
    else{
        current = list->head; //
        while (current != NULL) {
            if(strcmp(((Record* )(current->record))->path, path) == 0) {        
                //printf("%s exists!\n",path);
                free(node);
                return 1;
            }
            else {
                current = current->next;
                j++;
            }
        }

        node->previous = list->last; //not found, insert last
        node->next = NULL;
        list->last->next= node;
        list->last = node;

    }
    
    list->size++; 
    
    return 0;
}
/* Returns 1 if record's field is greater than current record */
int compareRecs(Record2* rec, Record2* curr, int field){


    switch(field) {

        case PID:   if(rec->PID >= curr->PID) return 1;
                    else return 0;
        case RCHAR: if(rec->RCHAR >= curr->RCHAR) return 1;
                    else return 0;
        case WCHAR: if(rec->WCHAR >= curr->WCHAR) return 1;
                    else return 0;                  
        case SYSR:  if(rec->SYSR >= curr->SYSR) return 1;
                    else return 0;  
        case SYSW:  if(rec->SYSW >= curr->SYSW) return 1;
                    else return 0;
        case RFS:   if(rec->RFS >= curr->RFS) return 1;
                    else return 0;
        case WFS:   if(rec->WFS >= curr->WFS) return 1;
                    else return 0;
        default:    return 0;
    }

}
/* Records in list are sorted by field in descending order */
void insertOrderedList(List* list, Record2* record, int field) { //TODO size of list up to records
    
    Node* current;
    Node* node;
    int j=1;
    int found = 0;

    if( list == NULL){
        printf("insertList: List is not created.Could not insert!\n");
        exit(EXIT_FAILURE);
    }
    /* Create  a new node */

    node = malloc(sizeof(Node)); //TODO free
    if(node == NULL) {
        perror("insertList: Could not allocate memory for new Node");
        exit(EXIT_FAILURE);
    }
    node->record = record;

    if (list->size == 0) { //list is empty
        //printf("List empty!Inserting %d first..\n",((Record2 *)(node->record))->PID);
        node->next = NULL;
        node->previous = NULL;
        list->head = node;
        list->last = node;
    }
    else if (compareRecs(node->record,list->head->record,field)== 1) { /* New record has the greatest value in field */
        //printf("Record is the new head!Inserting..\n");
        node->next = list->head;
        node->previous = NULL;
        list->head->previous = node;
        list->head = node;
    }
    else {
        current = list->head->next;
        while (current != NULL) {
            if( compareRecs(node->record, current->record, field )) {
                found = 1;
                break; //current = current->next
            }
            else {
                // smaller
                current = current->next;
                j++;
            }
        }
        if(found) {
            /* Field of new record greater or equal than current node's record.Insert before it */
            //printf("Inserting in position %d..\n",j);
            node->previous = current->previous;
            node->next = current;
            current->previous->next = node;
            current->previous = node;
        }
        else {
            //printf("Inserting last at position %d..\n",j);
            node->previous = list->last;
            node->next = NULL;
            list->last->next= node;
            list->last = node;
        }
    }
    
    list->size++;
}

        
int destroyList(List* list) {
    
    Node* current = list->head;
    //printf("List size %d\n",list->size);
    
    while (current != NULL) { //list->size >0
        free(current->record);
        if(current->next == NULL){
            //last node
            current->next = NULL;
            current->previous = NULL;
            free(current);
            current = NULL;
            break;
        }
        else{
            current = current->next;
            current->previous->previous = NULL;
            current->previous->next = NULL;
            free(current->previous);
            // current->previous = NULL;
        }

        //printf("Deleting..size is now %d\n",--list->size);
    }
    list->head = NULL;
    list->last = NULL;
    //printf("List deleted!size %d\n",--list->size);
    free(list);

}
int printList(List* list,int records) {

    int count=0;

    if(list == NULL){
        printf("printList: List is not created.Could not print!\n");
        return 1;
    }
    Node* current = list->head;
    while(current != NULL) {
        if( count < records) {
            printRecord2(current->record);
            current = current->next;
            count++;
        }
        else break;
    }
    printf("\n");
    return 0;
}
void printRecord(Record* rec){
    
    printf ("%c",rec->type);
    printf ((rec->mode & S_IRUSR) ? "r" : "-");
    printf ((rec->mode & S_IWUSR) ? "w" : "-");
    printf ((rec->mode & S_IXUSR) ? "x" : "-");
    printf ((rec->mode & S_IRGRP) ? "r" : "-");
    printf ((rec->mode & S_IWGRP) ? "w" : "-");
    printf ((rec->mode & S_IXGRP) ? "x" : "-");
    printf ((rec->mode & S_IROTH) ? "r" : "-");
    printf ((rec->mode & S_IWOTH) ? "w" : "-");
    printf ((rec->mode & S_IXOTH) ? "x" : "-");
    printf("%3d %s %s %7ld %s\n", rec->nlinks, rec->user, rec->group, rec->size, rec->path);
}

void printRecord2(Record2* rec){

    printf("%5d %10lu %10lu %10lu %10lu %10lu %10lu \n",
    rec->PID, rec->RCHAR, rec->WCHAR, rec->SYSR, rec->SYSW, rec->RFS, rec->WFS);
}