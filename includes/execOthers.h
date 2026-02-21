#ifndef EXEC_OTHERS_H
#define EXEC_OTHERS_H

#include <unistd.h>
#include <wait.h>

#include "globals.h"

/*
*   Helper function for executing another binary
*   the bin_path should avoid using PATH, and should give an absolute path
*   to the binary.
*
*   bin_path: path to the binary
*   command_args: crafted command arguments
*
*   return:
*       HAD_ERROR if fork fails or if the program returns an error
*       NO_ERROR if everything is fine
*/
enum ERROR execProgram(const char* bin_path, char* const command_args []);

#endif
