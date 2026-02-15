#ifndef DOWNLOAD_AUDIO_TEST_H
#define DOWNLOAD_AUDIO_TEST_H

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>

#include "testGlobals.h"
#include "testSetups.h"
#include "downloading.h"

#define AUDIO_ROOT "./dir_env"

#define ROOT_ITSELF "/"
#define PATH_1 "/1/"
#define PATH_2 "/2/"
#define PATH_3 "/3/"
#define PATH_4 "/4/"
#define PATH_5 "/5/"
#define PATH_6 "/6/"
#define PATH_7 "/7/"

#define PATH_1_ID 2
#define PATH_2_ID 3
#define PATH_3_ID 4
#define PATH_4_ID 5
#define PATH_5_ID 6
#define PATH_6_ID 7
#define PATH_7_ID 8

#define GREP_FOUND 0
#define GREP_NO_FOUND 1

void testDownloadAudioNoMetaData(void** state);
void testDownloadAudioAllMetaData(void** state);
void testDownloadAudioEmbedsCoverArt(void** state);
void testDownloadAudioEmbedsGivenCoverArt(void** state);
void testDownloadAudioEmbedsGivenCoverArtAndAllMetaData(void** state);
void testDownloadAudioEmbedsNoCoverArt(void** state);
void testDownloadAudioWeirdMetaData(void** state);

#endif
