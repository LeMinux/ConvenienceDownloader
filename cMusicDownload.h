#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "linkedList.h"

//error message defines
#define FAILED_MALLOC_MSG  PNT_RED"\nError MEM: Allocation of memory failed"PNT_RESET
#define DOWNLOAD_FAIL_MSG  PNT_RED"\nError DWNF: Failed to download with provided URL"PNT_RESET
#define TEMP_FILE_FAIL_MSG  PNT_RED"\nERROR READ: Error in reading temporary file"PNT_RESET
#define FILE_FAIL_MSG  PNT_RED"\nERROR GVFL: Error in reading given file. File does not exist"PNT_RESET
#define DIR_FAIL_MSG  PNT_RED"\nERROR GVDR: Error in reading given directory. Directory does not exist"PNT_RESET
#define DOWNLOAD_TO_FAIL PNT_RED"\nERROR DWTO: Error in reading DownloadTo.txt file"PNT_RESET
#define CONVERT_FAIL_MSG  PNT_RED"\nERROR CNVT: Error in converting from .mp4 to .mp3"PNT_RESET
#define MP4_FAIL_MSG  PNT_RED"\nERROR MVP4: Error in moving video file to desired directory"PNT_RESET
#define MP3_FAIL_MSG  PNT_RED"\nERROR MVP3: Error in moving .mp3 file to /Unsynced directory"PNT_RESET
#define SKIP_VALID_MSG  PNT_RED"\nERROR INVD: Error in user skipping validation of music directories"PNT_RESET
#define SONG_NOT_MSG PNT_RED"\nERROR SGNF: Error in trying to convert song as song was not found"PNT_RESET


//error code defines
#define FAILED_MALLOC_CODE 1
#define DOWNLOAD_FAIL_CODE 2
#define TEMP_FILE_FAIL_CODE 3
#define FILE_FAIL_CODE 4
#define DIR_FAIL_CODE 5
#define DOWNLOAD_TO_CODE 6
#define CONVERT_FAIL_CODE 7
#define MP4_FAIL_CODE 8
#define MP3_FAIL_CODE 9
#define SKIP_VALID_CODE 10
#define SONG_NOT_CODE 11

//some constant defines
#define YT_URL_BUFFER 44 //this is from the beginning of the URL to the end of the ID parameter
#define YT_ID_SIZE 11
#define WHERE_SEND_FILES "DownloadTo.txt"

//prints an error message
//returns the exit code provided and prints out the message
void printError(int, const char*);

//helper method to get input and clears stdin
//also nul terminates the string
void exactUserInput(char*, int);

//method for obtaining input from file streams
//returns how many characters read for any error checking needed
int exactFileInput(FILE* stream, char* dest, int buffer);


//asks user for a youtube URL
char* getURL(void);

//downloads a song given the URL for it
void downloadURL(char*);

//helper method for getting the ID
char* getID(char*);

//obtains the songName with grep and using a temp file
char* getSongName(char*);

//converts and moves the song to the given directory
void convertMove(char*, const char*);

//helper method for if the user wants to repeat or not
int repeat(void);

//Obtains a list of directories given what root directory
//DownloadTo.txt has provided. This will result in a case sensitive list
//this also keeps <directory>/ to prevent further reading of the DownloadTo.txt
Node_t* getDirectories(void);

//gets from the user what directory they want to download into
//with the help of getDirectories
char* userDirectory(void);

//converts the passes video file into a .mp3 with ffmpeg
//and return the newly created .mp3 file
char* convertToMp3(char* songName);

//moves the specified file into the specified directory
void moveFile(char* file, const char* destination);

