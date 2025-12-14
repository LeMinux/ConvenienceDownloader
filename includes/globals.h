#ifndef __GLOBALS_H
#define __GLOBALS_H


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


#define VERSION "2025 9.7"

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define CYAN    "\033[36m"
#define YOUTUBE_URL "https://www.youtube.com/watch?v="

#define BUFFER_SIZE 101
#define LARGER_BUFFER_SIZE 201

enum ERROR {NO_ERROR, HAD_ERROR};
enum CONFIG {AUDIO_CONFIG = 1, VIDEO_CONFIG, COVER_CONFIG, BLACK_CONFIG};

#define PRINT_ERROR(message_) ((void)fputs(RED message_ RESET"\n", stderr))
//If you're a NASA programmer seeing this sorry for the variadic macro.
//Although you probably don't like the usage of dynamic memory after initalization anyway lol
#define PRINT_FORMAT_ERROR(format_, ...) fprintf(stderr, RED format_ RESET, __VA_ARGS__)

//general failed malloc message
#define FAILED_MALLOC_MSG "\nError MEM: Allocation of memory failed"

#define YT_URL_INPUT_SIZE 44 //this is from the beginning of the URL to the end of the ID (32 + 11 + 1)
#define YT_ID_SIZE 12

#endif
