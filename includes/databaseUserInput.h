#ifndef DATABASE_USERINPUT_H
#define DATABASE_USERINPUT_H

//this file mostly exists to separate out the functions strictly for user input from user input for the database
//Since cmocka uses --wrap for stubbing/mocking if the dependency is in the same file as the unit being tested
//the linker makes it impossible for --wrap to insert the stubbed/mocked function since it links within that file and has no context of the wrapper
//By separating out these functions from a dependency like boundedInput() it then prevents internal linking.

#include <stdio.h>
#include <ctype.h>

#include "globals.h"
#include "userInput.h"
#include "fileOps.h"

#define INF_STRING "INF"
#define MAX_DEPTH 2048

/*
*   Takes input from the user to accept a directory.
*
*   return: NULL on error or a malloced absolute path
*/
char* takeDirectoryInput(void);

/*
*   Function for taking in input for what depth a root path should have.
*   There is no index 0, so index 1 is the first index.
*
*   return: returns a value < INT_MAX or the value of INF_DEPTH or INVALID
*/
int takeDepthInput(void);

/*
*   Function for taking in input for what index a root path is.
*
*   param: max_index specifies what is the maximum index allowed
*
*   return: returns a value < max_index or INVALID
*/
int takeIndexInput(int max_index);

#endif

