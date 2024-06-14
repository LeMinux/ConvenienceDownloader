#ifndef FILEOPS_H
#define FILEOPS_H

#include <stdlib.h>
#include <sys/stat.h>
#include <sys/dir.h>
#include <string.h>

#include "linkedList.h"
#include "helpers.h"

#define CONVERT_FAIL_MSG "\nERROR CNVT: Error in converting from .mp4 to .mp3"

#define DES "Destinations/"
#define DES_MP3 "Destinations/audioDest.txt"
#define DES_MP4 "Destinations/videoDest.txt"
#define DES_COVER "Destinations/coverDest.txt"

//in dirent.h the max is 256 including nul terminator
#define MAX_FILE_NAME 255

void writeDest(char* string, int mode);
void moveFile(const char* fileName, const char* destination);
void getFileNameByID(const char* id, const char* extension, char* outString, int length);
//void getSubdirectories(const char* basePath, Node_t** list);
void convertToMp3(const char* songName);

int checkIfExists(const char*);

#endif
