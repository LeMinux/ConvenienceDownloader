#ifndef PATHS_MAP
#define PATHS_MAP

#include "globals.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/dir.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#include "./userInput.h"

#define NO_SLASH 0
#define ADD_SLASH 1

//technically this isn't a "map", but it will function like one
//the key is going to be the index and the value is the string
//This holds the subdirectories of a given base directory path
typedef struct Map{
	unsigned int length;
	char** map;
}Map_t;

//a struct that holds an array of Map_t
//the length specifies how many maps are in the array
//This is used for when a user specifies multiple directories so each index
//is a map of what subdirectories there are.
//You may consider this struct unecessary as the Map_t map could just have
//every path in a gigantic array in the heap, but splitting the maps helps
//to find available space on the heap instead of reallocing a 200 element string array
typedef struct MapArray{
	unsigned int length;
	Map_t* mapArray;
}MapArray_t;

Map_t* obtainPathMap(const char*);
char* getSelection(MapArray_t*, const char*, int);
char* findPath(MapArray_t*, const char*);
void freePathMap(Map_t*);
void printPathMap(Map_t*);
void printMapArray(MapArray_t*);

#endif
