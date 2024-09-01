#ifndef HELPERS_H
#define HELPERS_H

#include <stdlib.h>
#include <string.h>

#include "globals.h"

//printing defines
#define PNT_GREEN "\x1b[32m"
#define PNT_RED "\x1b[31m"
#define PNT_RESET "\x1b[0m"

#define BUFFER_SIZE 101
#define LARGER_BUFFER_SIZE 201
#define YOUTUBE_URL "https://www.youtube.com/watch?v="

//prints a message and terminates the program
void printAndExit(int, const char*);

//surrounds the passed in text with quotes
//so something like-> this is a test message
//turns into "this is a test message"
//this is useful for simplifying adding escape characters
char* surroundInQuotes(const char* string);

#endif
