#ifndef USERINPUT_H
#define USERINPUT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "helpers.h"
#include "linkedList.h"
#include "fileOps.h"
#include <stdio.h>
#include "./includes/globals.h"

//error message defines
#define DOWNLOAD_FAIL_MSG "\nError DWNF: Failed to download with provided URL"
#define TEMP_FILE_FAIL_MSG "\nERROR READ: Error in reading temporary file"
#define FILE_FAIL_MSG  "\nERROR GVFL: Error in reading given file. File does not exist"
#define DIR_FAIL_MSG  "\nERROR GVDR: Error in reading given directory. Directory does not exist"
#define MP4_FAIL_MSG "\nERROR MVP4: Error in moving video file to desired directory"
#define MP3_FAIL_MSG "\nERROR MVP3: Error in moving audio file to desired directory"
#define SKIP_VALID_MSG "\nERROR INVD: Error in user skipping selecting a directory"

#define CHUNK_READ 51

//gets the URL from the user
//the parameter is an array of YT_URL_BUFFER size since
//the only required part needed is the base URL and the video ID parameter
void getURL(char [YT_URL_INPUT_SIZE]);

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
*	return: 0 for yes 1 for no
*/
int askToRepeat(void);


char* getUserChoiceForDir(const char* baseDir, const char* prompt);

char* getUserChoiceForDirNoSkip(const char* baseDir, const char* prompt);

/*
*	method for obtaining input from file streams where input size is known
*	returns how many characters read for any error checking needed
*	this excludes the count for the nul byte
*	Uses fgets so it is gaurenteed to be nul terminating
*
*	stream: file stream which can include stdin*
*	dest: destination string and should not be NULL
*		NULL is not checked since if you are calling this you already can check yourself
*	buffer: size of input expected. This should account for the nul byte
*
*	return: success or failure on success returns how much was read
*/
int exactInput(FILE* stream, char* dest, int buffer);

/*
*	helper method to read a line in a file of an unknown length
*	returns the length of the amount read
*
*	stream: File stream to take input from
*	dest: address to string which can be NULL
*
*	return: success or failure on success returns how much was read
*/
int unknownInput(FILE* stream, char** dest);

/*
*	 helper method to flush the stream to the next line.
*	 This does not use fflush because it is meant for output streams not input,
*	 this should be used after user input is taken
*	 To avoid having to press enter twice consider where the new line character
*	 is when parsing the input
*
*	stream: file stream to take input from
*
*	return: success or failure
*/
static void clearLine(FILE* stream);

#endif
