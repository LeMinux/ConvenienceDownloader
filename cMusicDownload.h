#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "linkedList.h"

//error message defines
#define DOWNLOAD_FAIL_MSG  PNT_RED"\nError DWNF: Failed to download with provided URL"PNT_RESET
#define TEMP_FILE_FAIL_MSG  PNT_RED"\nERROR READ: Error in reading temporary file"PNT_RESET
#define FILE_FAIL_MSG  PNT_RED"\nERROR GVFL: Error in reading given file. File does not exist"PNT_RESET
#define DIR_FAIL_MSG  PNT_RED"\nERROR GVDR: Error in reading given directory. Directory does not exist"PNT_RESET
#define DOWNLOAD_READ_MSG PNT_RED"\nERROR DWTO: Error in reading DownloadTo.txt file"PNT_RESET
#define DOWNLOAD_CREATE_MSG PNT_RED"\nERROR DWTC: Error in creating DownloadTo.txt file"PNT_RESET
#define CONVERT_FAIL_MSG  PNT_RED"\nERROR CNVT: Error in converting from .mp4 to .mp3"PNT_RESET
#define MP4_FAIL_MSG  PNT_RED"\nERROR MVP4: Error in moving video file to desired directory"PNT_RESET
#define MP3_FAIL_MSG  PNT_RED"\nERROR MVP3: Error in moving audio file to desired directory"PNT_RESET
#define SKIP_VALID_MSG  PNT_RED"\nERROR INVD: Error in user skipping selecting a music directory"PNT_RESET

//error code defines
#define DOWNLOAD_FAIL_CODE 2
#define TEMP_FILE_FAIL_CODE 3
#define FILE_FAIL_CODE 4
#define DIR_FAIL_CODE 5
#define DOWNLOAD_READ_CODE 6
#define DOWNLOAD_CREATE_CODE 7
#define CONVERT_FAIL_CODE 8
#define MP4_FAIL_CODE 9
#define MP3_FAIL_CODE 10
#define SKIP_VALID_CODE 11

//some constant defines
#define YT_URL_BUFFER 44 //this is from the beginning of the URL to the end of the ID parameter
#define YT_ID_SIZE 11
#define WHERE_SEND_FILES "DownloadTo.txt"

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

//Obtains a list of directories given what
//DownloadTo.txt has provided. This will result in a case sensitive list
//this also keeps <directory>/ to prevent further reading of the DownloadTo.txt
//the parameter is simply for which mode to read I.E. read MP4 dest or MP3 dest
Node_t* getDirectories(int);

//gets from the user what directory they want to download into
//with the help of getDirectories
//char* getMP4Dest(void);

//gets where the user specified to download mp3 files
//char* getMP3Dest(void);

//depending on the mode it will ask the user from a certain directory
//4 is for looking at the mp4 directory
//3 is for looking at the mp3 directory
//2 is for looking at a CoverArt directory
char* getDests(int, const char*);

//converts the passes video file into a .mp3 with ffmpeg
//and return the newly created .mp3 file
char* convertToMp3(const char*);

//moves the specified file into the specified directory
void moveFile(const char*, const char*);
