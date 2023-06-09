#ifndef COVERART_H
#define COVERART_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "helpers.h"

#define ART_ERROR_MSG PNT_RED"Error in adding cover art to .mp3 file"PNT_RESET
#define MV_TEMP_MSG PNT_RED"Error in overwriting original file to place cover art"PNT_RESET
#define MP3_LIST_MSG PNT_RED"Error in opening MP3 list given. File does not exist"PNT_RESET
#define COVER_ART_MSG PNT_RED"Error in finding cover art. Cover art file does not exist."PNT_RESET

//ASCII letter 'C' == 43
#define ART_ERROR_CODE 431
#define MV_TEMP_CODE 432
#define MP3_LIST_CODE 433
#define COVER_ART_CODE 434


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
