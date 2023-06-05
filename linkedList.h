#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "helpers.h"

#define FAILED_LMALLOC_MSG  PNT_RED"\nError MEM: Allocation of memory failed for linked list"PNT_RESET
#define FAILED_GET_MSG PNT_RED"\nUse of getElement in linked list went out of bounds"PNT_RESET

//begins with 76 since 'L' = 76 in ASCII
#define FAILED_LMALLOC_CODE 761
#define FAILED_GET_CODE 762

//this file is to simplify greatly the return of file names
typedef struct Node{
	struct Node* next;
	char* string;
}Node_t;

//adds a string to the list
void addToList(Node_t**, const char*);

//deletes all nodes from the list
void deleteList(Node_t**);

//prints all the nodes in the list with a "-> " preceeding it
void printList(Node_t*);

//tries to find the specified string passed
//if the element is found it return the index it was found at
//return value starts at 0
//if the element can not be found it returns -1
int containsElement(Node_t*, const char*);

//retuns a string if possible at the specified index
//the int parameter should start at 0. 0 being the first element
//This will return a newly malloced string to prevent aliasing
char* getElement(Node_t*, int);

#endif