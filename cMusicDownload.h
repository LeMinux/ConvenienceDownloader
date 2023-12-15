#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "helpers.h"
#include "linkedList.h"

//error message defines
#define DOWNLOAD_FAIL_MSG "\nError DWNF: Failed to download with provided URL"
#define TEMP_FILE_FAIL_MSG "\nERROR READ: Error in reading temporary file"
#define FILE_FAIL_MSG  "\nERROR GVFL: Error in reading given file. File does not exist"
#define DIR_FAIL_MSG  "\nERROR GVDR: Error in reading given directory. Directory does not exist"
#define CONVERT_FAIL_MSG "\nERROR CNVT: Error in converting from .mp4 to .mp3"
#define MP4_FAIL_MSG "\nERROR MVP4: Error in moving video file to desired directory"
#define MP3_FAIL_MSG "\nERROR MVP3: Error in moving audio file to desired directory"
#define SKIP_VALID_MSG "\nERROR INVD: Error in user skipping selecting a directory"

//some constant defines
#define YT_URL_BUFFER 44 //this is from the beginning of the URL to the end of the ID parameter
#define YT_ID_SIZE 11

//asks user for a youtube URL
char* getURL(void);

//downloads a song given the URL for it
//0 is false for downloading thumbnails
//1 is true for downloading thumnbails
void downloadURL(const char*, int);

//helper method for getting the ID
char* getID(const char*);

//obtains the songName with grep and using a temp file
char* getSongName(const char*);

//converts and moves the song to the given directory
void convertMove(const char*, const char*);

//helper method for if the user wants to repeat or not
int repeat(void);

void getSubdirectories(const char*, Node_t**);

char* getUserChoiceForDir(const char*, const char*);

char* getUserChoiceForDirNoSkip(const char*, const char* prompt);

//converts the passes video file into a .mp3 with ffmpeg
//and return the newly created .mp3 file
char* convertToMp3(const char*);

//moves the specified file into the specified directory
void moveFile(const char*, const char*);
