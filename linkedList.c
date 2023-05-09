#include "linkedList.h"

void addToList(Node_t** head, const char* string){
	Node_t* newNode = malloc(sizeof(Node_t));
	if(newNode == NULL)
		printError(FAILED_LMALLOC_CODE, FAILED_LMALLOC_MSG);

	newNode->next = *head;
	newNode->string = malloc(strlen(string) + 1);
	if(newNode->string == NULL)
		printError(FAILED_LMALLOC_CODE, FAILED_LMALLOC_MSG);

	snprintf(newNode->string, strlen(string) + 1,"%s",string);
	*head = newNode;
}

void deleteList(Node_t** head){
	Node_t* temp = *head;
	while(*head != NULL){
		*head = (*head)->next;

		free(temp->string);
		temp->string = NULL;
		free(temp);
		temp = NULL;

		temp = *head;
	}
}

void printList(Node_t* head){
	while(head != NULL){
		printf("-> %s\n", head->string);
		head = head->next;
	}
}

int containsElement(Node_t* head, const char* compareTo){
	int index = 0;
	while(head != NULL){
		if(strcmp(head->string, compareTo) == 0)
			return index;

		++index;
		head = head->next;
	}

	return -1;
}

char* getElement(Node_t* head, int get){
	if(get < 0)
		printError(FAILED_GET_CODE, FAILED_GET_MSG);

	int index = 0;
	char* returnString = NULL;
	while(head != NULL){
		if(index++ == get){
			returnString = malloc(strlen(head->string) + 1);
			snprintf(returnString, strlen(head->string) + 1, "%s", head->string);
			return returnString;
		}
		head = head->next;
	}

	//if it could not find at an index it was out of bound of the length
	printError(FAILED_GET_CODE, FAILED_GET_MSG);	
	//just to keep compiler happy
	return NULL;
}
