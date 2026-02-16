#ifndef FILEOPS_H
#define FILEOPS_H

#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/dir.h>
#include <string.h>
#include <unistd.h>

#include "globals.h"


#define CONVERT_FAIL_MSG "\nERROR CNVT: Error in converting from .mp4 to .mp3"

/*
 *  Wrapper of fopen to check additonal properties.
 *  These extra checks are for if the file is an actual file and not a directory as fopen won't NULL on directories.
 *  Mostly this is for paths given from a user.
 *
 *  param:
 *      file_path: path to the file
 *      mode: same mode you would give to fopen()
 *
 *  return:
 *      NULL if couldn't open file or file is not a file
 *      a FILE* if it could
 */
FILE* openFile(const char* file_path, const char* mode);

/*
 *  Check if a path given is a directory, and exists.
 *  Don't expect to be saved from race conditions since the database only holds paths.
 *  Updates that happen on the OS will not be reflected in the database.
 *
 *   return: VALID if meets conditions INVALID otherwise.
 */
enum INPUT checkDirPath(const char* dir_path);

#endif
