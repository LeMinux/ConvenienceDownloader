#ifndef DOWNLOADING_H
#define DOWNLOADING_H

#include <string.h>

#include "globals.h"

typedef struct MetaData{
    const char* genre;
    const char* artist;
    const char* album;
}MetaData_t;

/*
*   Downloads videos using yt-dlp's -t mp4 option.
*   Metadata is added using the default options, or from the meta struct given.
*
*   yt_url: Youtube URL
*   v_id: Root ID representing a video config entry
*   meta: metadata information to add if necessary
*
*   return:
*       NO_ERROR if able to download
*       HAD_ERROR if couldn't
*/
enum ERROR downloadVideo(const char* yt_url, int v_id, const MetaData_t* meta);

/*
*   Downloads audio using yt-dlp's --extract-audio option
*   Metadata is added using the default options, or from the meta struct given.
*
*   yt_url: Youtube URL
*   a_id: Root ID representing an audio config entry
*   meta: metadata information to add if necessary
*   wants_cover: determine if needs to add the thumbnail
*   cover_path: Path to a given cover that wants to be added
*
*   return:
*       NO_ERROR if could do all operations
*       HAD_ERROR if couldn't
*/
enum ERROR downloadAudio(const char* yt_url, int a_id, const MetaData_t* meta, enum COVERS wants_cover, const char* cover_path);

/*
*   Downloads Youtube thumbnail.
*
*   yt_url: Youtube URL
*   c_id: Root ID representing a cover config entry
*
*   return:
*       NO_ERROR if able to download
*       HAD_ERROR if couldn't
*/
enum ERROR downloadCover(const char* yt_url, int c_id);

#endif
