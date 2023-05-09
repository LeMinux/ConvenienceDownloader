#ifndef HELPERS_H
#define HELPERS_H

#include <stdio.h>
#include <stdlib.h>

//printing defines
#define PNT_GREEN "\x1b[32m"
#define PNT_RED "\x1b[31m"
#define PNT_RESET "\x1b[0m"

//printing an Error is more dynamic and more compact
//this also means there is no table of errors
void printError(int code, const char* message);

//helper method to get input and clears stdin
//also nul terminates the string
void exactUserInput(char* input, int buffer);

//method for obtaining input from file streams
//returns how many characters read for any error checking needed
int exactFileInput(FILE* stream, char* dest, int buffer);

#endif
