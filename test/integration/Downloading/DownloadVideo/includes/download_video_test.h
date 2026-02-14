#ifndef DOWNLOAD_VIDEO_TEST_H
#define DOWNLOAD_VIDEO_TEST_H

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>

#include "testGlobals.h"
#include "testSetups.h"
#include "downloading.h"

#define VIDEO_ROOT "./dir_env"

#define ROOT_ITSELF "/"
#define PATH_1 "/1/"
#define PATH_2 "/2/"
#define PATH_3 "/3/"
#define PATH_4 "/4/"
#define PATH_5 "/5/"

#define PATH_1_ID 2
#define PATH_2_ID 3
#define PATH_3_ID 4
#define PATH_4_ID 5
#define PATH_5_ID 6

#define GREP_FOUND 0
#define GREP_NO_FOUND 1

void testDownloadVideoNoMetaData(void** state);
void testDownloadVideoAllMetaData(void** state);
void testDownloadVideoWeirdMetaData(void** state);

#endif
