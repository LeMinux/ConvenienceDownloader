#ifndef DOWNLOADING_H
#define DOWNLOADING_H

#include <string.h>
#include <unistd.h>
#include <wait.h>

#include "globals.h"
#include "databaseOps.h"

typedef struct MetaData{
    const char* genre;
    const char* artist;
    const char* album;
}MetaData_t;

enum ERROR downloadVideo(const char* yt_url, int v_id, const MetaData_t* meta);
enum ERROR downloadAudio(const char* yt_url, int a_id, const MetaData_t* meta, enum COVERS wants_cover);
enum ERROR downloadCover(const char* yt_url, int c_id);

#endif
