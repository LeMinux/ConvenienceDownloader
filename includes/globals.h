#ifndef __GLOBALS_H
#define __GLOBALS_H

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define CYAN    "\033[36m"
#define NO_ERROR 0
#define HAD_ERROR -1

#define PRINT_ERROR(message_) ((void)fputs(RED message_ RESET, stderr))

#endif
