#ifndef WRITE_ART_H
#define WRITE_ART_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "execOthers.h"
#include "fileOps.h"

#define TEMP_FILE "/tmp/newly_added_art.mp3"

/*
*   Reads the file that yt-dlp makes that contains the file name and adds
*   a given cover art to the file.
*
*   file_with_path: file path to the file yt-dlp created with the --print-to-file option
*   cover_path: file path to the desired cover. Since FFMPEG doesn't allow in place
*   editing the cover art is added to a temporary file in /tmp and then moved into
*   place.
*
*   return:
*       NO_ERROR if was able to write the cover art without an error
*       HAD_ERROR if an error prevented writing the cover art
*/
enum ERROR writeCover(const char* file_with_path, const char* cover_path);

#endif
