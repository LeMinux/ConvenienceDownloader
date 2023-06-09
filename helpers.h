#ifndef HELPERS_H
#define HELPERS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//printing defines
#define PNT_GREEN "\x1b[32m"
#define PNT_RED "\x1b[31m"
#define PNT_RESET "\x1b[0m"

#define FAILED_MALLOC_MSG  PNT_RED"\nError MEM: Allocation of memory failed"PNT_RESET
#define FAILED_MALLOC_CODE 1

#define BUFFER_SIZE 101
#define LARGER_BUFFER_SIZE 201
#define YOUTUBE_URL "https://www.youtube.com/watch?v="

//printing an Error is more dynamic and more compact
//this also means there is no table of errors
void printError(int code, const char* message);

//helper method to get input and clears stdin
//also nul terminates the string
void exactUserInput(char* input, int buffer);

//method for obtaining input from file streams
//returns how many characters read for any error checking needed
//this excludes the count for the nul byte
int exactFileInput(FILE* stream, char* dest, int buffer);

//checks via ls if the passed in directory path or file exists
int checkIfExists(const char*);

//surrounds the passed in text with quotes
//so something like-> this is a test message
//turns into "this is a test message"
//this is useful for simplifying adding escape characters
char* surroundInQuotes(const char*);
#endif
