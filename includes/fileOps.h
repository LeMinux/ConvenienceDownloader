#ifndef FILEOPS_H
#define FILEOPS_H

#include <stdlib.h>
#include <sys/stat.h>
#include <sys/dir.h>
#include <string.h>

#include "linkedList.h"
#include "helpers.h"
#include "globals.h"

#define CONVERT_FAIL_MSG "\nERROR CNVT: Error in converting from .mp4 to .mp3"

//in dirent.h the max is 256 including nul terminator
#define MAX_FILE_NAME 255
#define NOT_A_DIR 0

void moveFile(const char* fileName, const char* destination);
void getFileNameByID(const char* id, const char* extension, char* outString, int length);
//void getSubdirectories(const char* basePath, Node_t** list);
void convertToMp3(const char* songName);

int checkIfExists(const char*);

int validateDirPath(const char*);

#endif
