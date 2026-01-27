#ifndef FILEOPS_H
#define FILEOPS_H

#include <stdlib.h>
#include <sys/stat.h>
#include <sys/dir.h>
#include <string.h>
#include <unistd.h>

#include "globals.h"


#define CONVERT_FAIL_MSG "\nERROR CNVT: Error in converting from .mp4 to .mp3"

/*
int moveFile(const char* fileName, const char* destination);
void getFileNameByID(const char* id, const char* extension, char* outString, int length);
void convertToMp3(const char* songName);

int checkIfExists(const char*);

*/
//wrapper function to check file
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
