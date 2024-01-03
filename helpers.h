#ifndef HELPERS_H
#define HELPERS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//printing defines
#define PNT_GREEN "\x1b[32m"
#define PNT_RED "\x1b[31m"
#define PNT_RESET "\x1b[0m"

//general failed malloc message
#define FAILED_MALLOC_MSG "\nError MEM: Allocation of memory failed"


#define BUFFER_SIZE 101
#define LARGER_BUFFER_SIZE 201
#define YOUTUBE_URL "https://www.youtube.com/watch?v="

//prints an error and terminates execution returning the exit code provided
void printError(int code, const char* message);

//surrounds the passed in text with quotes
//so something like-> this is a test message
//turns into "this is a test message"
//this is useful for simplifying adding escape characters
char* surroundInQuotes(const char* string);
#endif
