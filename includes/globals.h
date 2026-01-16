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

#define INF_DEPTH -5

enum ERROR {HAD_ERROR = -1, NO_ERROR = 0};
enum CONFIG {AUDIO_CONFIG = 1, VIDEO_CONFIG, COVER_CONFIG, BLACK_CONFIG};
enum INPUT {INVALID = -1, VALID};

#define PRINT_ERROR(message_) ((void)fputs(RED message_ RESET"\n", stderr))

//If you're a NASA programmer seeing this sorry for the variadic macro.
//Although you probably don't like the usage of dynamic memory after initalization from the printf family anyway lol
#define PRINT_FORMAT_ERROR(format_, ...) ((void)fprintf(stderr, RED format_ RESET"\n", __VA_ARGS__))

#define ADVISE_USER(message_) ((void)puts(CYAN message_ RESET))

#define ADVISE_USER_FORMAT(format_, ...) ((void)fprintf(stderr, CYAN format_ RESET"\n", __VA_ARGS__))

//general failed malloc message
#define FAILED_MALLOC_MSG "\nError MEM: Allocation of memory failed"

#define YT_URL_INPUT_SIZE 44 //this is from the beginning of the URL to the end of the ID (32 + 11 + 1)
#define YT_ID_SIZE 12

#endif
