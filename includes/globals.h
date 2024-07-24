#ifndef __GLOBALS_H
#define __GLOBALS_H


#include <stdio.h>
#include <stdlib.h>

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define CYAN    "\033[36m"
#define NO_ERROR 0
#define HAD_ERROR -1

#define PRINT_ERROR(message_) ((void)fputs(RED message_ RESET"\n", stderr))

//general failed malloc message
#define FAILED_MALLOC_MSG "\nError MEM: Allocation of memory failed"

#define YT_URL_INPUT_SIZE 44 //this is from the beginning of the URL to the end of the ID (32 + 11 + 1)
#define YT_ID_SIZE 11

#endif
