#ifndef HELPERS_H
#define HELPERS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//printing defines
#define PNT_GREEN "\x1b[32m"
#define PNT_RED "\x1b[31m"
#define PNT_RESET "\x1b[0m"

#define FAILED_MALLOC_MSG "\nError MEM: Allocation of memory failed"
#define FAILED_GREP_MSG "\nERROR SGNF: Error in trying to grep passed text into a file"


#define BUFFER_SIZE 101
#define LARGER_BUFFER_SIZE 201
#define YOUTUBE_URL "https://www.youtube.com/watch?v="

#define DES "Destinations/"
#define DES_MP3 "Destinations/audioDest.txt"
#define DES_MP4 "Destinations/videoDest.txt"

//prints an error and terminates execution returning the exit code provided
void printError(int code, const char* message);

//helper method to get input and clears stdin
//also nul terminates the string
void exactUserInput(char* input, int buffer);

//method for obtaining input from file streams
//returns how many characters read for any error checking needed
//this excludes the count for the nul byte
int exactFileInput(FILE* stream, char* dest, int buffer);

//helper method to read a line in a file of an unkown length
//returns the length of the amount read
int unknownInput(FILE* stream, char** dest);

//checks if the passed in directory or file exists
int checkIfExists(const char*);

//surrounds the passed in text with quotes
//so something like-> this is a test message
//turns into "this is a test message"
//this is useful for simplifying adding escape characters
char* surroundInQuotes(const char*);

//helper method using grep to place matching text into a file
//this helps if further parsing is needed
void grepIntoFile(const char*);
#endif
