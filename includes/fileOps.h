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
#define NOT_A_DIR 0
#define CHECK_FOR_LINK 1
#define NO_CARE_LINK 0

#define AUDIO_CONFIG 1
#define VIDEO_CONFIG 2
#define COVER_CONFIG 3

#define AUDIO_CONFIG_PATH "~/.con-downloader/audioDests.txt"
#define VIDEO_CONFIG_PATH "~/.con-downloader/videoDests.txt"
#define COVER_CONFIG_PATH "~/.con-downloader/coverDests.txt"

typedef struct DirInfo{
    char* root_name;
    DIR* open_dir;
    int name_length;
    int depth;
} DirInfo;

typedef struct DirInfoArray{
    DirInfo* dir_entries;
    int length;
} DirInfoArray;


int moveFile(const char* fileName, const char* destination);
void getFileNameByID(const char* id, const char* extension, char* outString, int length);
//void getSubdirectories(const char* basePath, Node_t** list);
void convertToMp3(const char* songName);

int checkIfExists(const char*);

int validateDirPath(const char*);

//wrapper function to check mode is file
FILE* openFile(const char*, const char* mode);

int setConfigDest(int config, const DirInfoArray* dir_infos);

int readConfig(int config, DirInfoArray* dir_array);

void openDir(const char* path, DIR** stream_result);

#endif
