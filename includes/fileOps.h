#ifndef FILEOPS_H
#define FILEOPS_H

#include <stdlib.h>
#include <sys/stat.h>
#include <sys/dir.h>
#include <string.h>
#include <unistd.h>

#include "helpers.h"
#include "globals.h"


#define CONVERT_FAIL_MSG "\nERROR CNVT: Error in converting from .mp4 to .mp3"

//in dirent.h the max is 256 including nul terminator
#define MAX_FILE_NAME 255
#define MY_MAX_PATH_SIZE 4096
#define CONFIG_BUFFER 268 //255 + , + \n + 10 digits for depth + \0

int moveFile(const char* fileName, const char* destination);
void getFileNameByID(const char* id, const char* extension, char* outString, int length);
//void getSubdirectories(const char* basePath, Node_t** list);
void convertToMp3(const char* songName);

int checkIfExists(const char*);

int validateDirPath(const char*);

//wrapper function to check i file
FILE* openFile(const char*, const char* mode);


/*
 *  Check if a path given is a directory, and exists.
 *  Don't expect to be saved from race conditions since the database only holds paths.
 *  Updates that happen on the OS will not be reflected in the database.
 *
 *   return: VALID if meets conditions INVALID otherwise.
 */
enum INPUT checkDirPath(const char* dir_path);

#endif
