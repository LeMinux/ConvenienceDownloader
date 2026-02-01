#ifndef USERINPUT_H
#define USERINPUT_H

#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <errno.h>

#include "globals.h"


//Another NASA sin :(
#ifdef WRAPPED_INPUT
    #include "testWrapInput.h"
#endif

#define AUDIO_STRING "audio"
#define VIDEO_STRING "video"
#define COVER_STRING "cover"
#define BLACK_STRING "black"

#define YOUTUBE_URL "https://www.youtube.com/watch?v="

#define LEN_BEFORE_ID 32
#define YT_ID_LEN 11

#define YT_URL_INPUT_SIZE (LEN_BEFORE_ID + YT_ID_LEN + 1)
#define YT_ID_SIZE 12

#define OPTION_LEN 5

enum REPEAT {ASK_AGAIN = -1, NO_REPEAT, REPEAT};

/*
*	gets the URL from the user.
*	the parameter is a passed by reference string to place the ID into
*	It's not necessary to carry around the entire string as sometimes there can
*	be really long youtube URLs, and the only thing that's really needed to get
*	to a youtube video is the beginning youtube.com protion and the video ID parameter
*
*	return:
*    VALID for a valid URL INVALID to prompt input again.
*    If input is VALID the video ID is placed into the parameter.
*    Don't try to use the ID if INVALID is passed as this method won't overwrite
*    previous contents so you will use what ever was in ret_id before calling.
*/
enum INPUT getIDFromURL(char ret_id [YT_ID_SIZE]);

/*
*	downloads a song given the URL for it
*	different modes are specified depending on what is desired
*	preferably this should only be called once per URL to minimize network traffic.
*
*	youtubeURL: the youtubeURL given
*	mode: 3 to specify to download audio 4 to specify to download video
*	downloadCoverArt: 0 to for not downloading thumbnails and 1 to download thumbnails
*
*	return: success or failure
*/
int downloadFromURL(const char* youtubeURL, int mode, int downloadCoverArt);

/*
*	asks the user if they would like to continue downloading
*
*	return: REPEAT for yes NO_REPEAT for no ASK_AGAIN if input is invalid
*/
enum REPEAT askToRepeat(void);


char* getUserChoiceForDir(const char* baseDir, const char* prompt);

char* getUserChoiceForDirNoSkip(const char* baseDir, const char* prompt);

/*
*	method for obtaining user input where size is bounded
*   returns how many characters were read excluding newlines and the nul byte.
*   The amount returned is able to be used to determine the position of the nul byte.
*   The bound is meant for c_strings, so the bound must be at least 1 so a nul byte can be inserted at the end, and at least 2 for actual input.
*
*	stream: file stream which can include stdin*
*	dest: destination string and should not be NULL
*	buffer: size of input expected. This should account for the nul byte like you would for fgets.
*
*	return: returns how much was read
*/
int boundedInput(FILE* stream, char* dest, size_t size);

/*
*	helper method to read a line in a file of an unknown length
*	returns the length of the amount read
*
*	stream: File stream to take input from
*	dest: address to string which can be NULL
*
*	return: returns how much was read excluding nul byte
*/
//int unknownInput(FILE* stream, char** dest);

//int appendRootEntry(RootInfoArray* entry_array, const char* new_entry);
//

enum CONFIG getConfigToEdit(const char* input);

#endif
