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

#define FAILED_MALLOC_MSG "\nError MEM: Allocation of memory failed"
//#define FAILED_GREP_MSG "\nERROR SGNF: Error in trying to grep passed text into a file"


#define CHUNK_READ 51
#define BUFFER_SIZE 101
#define LARGER_BUFFER_SIZE 201
#define YOUTUBE_URL "https://www.youtube.com/watch?v="

//prints an error and terminates execution returning the exit code provided
void printError(int code, const char* message);

//helper method to get input and clears stdin
//also nul terminates the string
//void exactUserInput(char* input, int buffer);

//surrounds the passed in text with quotes
//so something like-> this is a test message
//turns into "this is a test message"
//this is useful for simplifying adding escape characters
char* surroundInQuotes(const char*);

//helper method using grep to place matching text into a file
//this helps if further parsing is needed
//void grepIntoFile(const char*);
#endif
