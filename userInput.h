#ifndef USERINPUT_H
#define USERINPUT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>


//error message defines
#define DOWNLOAD_FAIL_MSG "\nError DWNF: Failed to download with provided URL"
#define TEMP_FILE_FAIL_MSG "\nERROR READ: Error in reading temporary file"
#define FILE_FAIL_MSG  "\nERROR GVFL: Error in reading given file. File does not exist"
#define DIR_FAIL_MSG  "\nERROR GVDR: Error in reading given directory. Directory does not exist"
#define MP4_FAIL_MSG "\nERROR MVP4: Error in moving video file to desired directory"
#define MP3_FAIL_MSG "\nERROR MVP3: Error in moving audio file to desired directory"
#define SKIP_VALID_MSG "\nERROR INVD: Error in user skipping selecting a directory"

//some constant defines
#define YT_URL_BUFFER 44 //this is from the beginning of the URL to the end of the ID (33 + 11)
#define YT_ID_SIZE 11

#define CHUNK_READ 51

char* getURL(void);

//downloads a song given the URL for it
//0 is false for downloading thumbnails
//1 is true for downloading thumnbails
void downloadFromURL(const char* youtubeURL, int mode, int downloadCoverArt);

int askToRepeat(void);

char* getUserChoiceForDir(const char* baseDir, const char* prompt);

char* getUserChoiceForDirNoSkip(const char* baseDir, const char* prompt);

//method for obtaining input from file streams
//returns how many characters read for any error checking needed
//this excludes the count for the nul byte
int exactInput(FILE* stream, char* dest, int buffer);

//helper method to read a line in a file of an unknown length
//returns the length of the amount read
int unknownInput(FILE* stream, char** dest);

#endif
