#ifndef COVERART_H
#define COVERART_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "helpers.h"

#define ART_ERROR_MSG "Error in adding cover art to .mp3 file"
#define MV_TEMP_MSG "Error in overwriting original file to place cover art"
#define MP3_LIST_MSG "Error in opening MP3 list given. File does not exist"
#define COVER_ART_MSG "Error in finding cover art. Cover art file does not exist."

const static char FFMPEG [] = "ffmpeg";
const static char DASH_I [] = " -i ";
const static char OPTIONS [] = " -map 0:0 -map 1:0 -id3v2_version 3 -codec copy -metadata:s:v comment=\"Cover (front)\" ";
const static char MV [] = "mv ";


//writes the specified image to the list of songs as mp3's
//The full path should be specified per entry in the song file
void writeCovers(FILE*, const char*);

//writes to one song the specified cover art
//this needs the songName as an mp3
void writeCover(const char*, const char*);

#endif
