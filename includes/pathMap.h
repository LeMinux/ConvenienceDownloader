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

#define NO_SLASH 0
#define ADD_SLASH 1

//technically this isn't a "map", but it will function like one
//the key is going to be the index and the value is the string
typedef struct Map{
	unsigned int length;
	char** map;
}Map_t;

//
Map_t* obtainPathMap(const char*);
void freePathMap(Map_t*);
void printPathMap(Map_t*);

#endif
