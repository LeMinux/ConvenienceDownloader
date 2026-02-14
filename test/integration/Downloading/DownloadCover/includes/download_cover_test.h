#ifndef DOWNLOAD_COVER_TEST_H
#define DOWNLOAD_COVER_TEST_H

#include <dirent.h>
#include <stdio.h>

#include "testGlobals.h"
#include "testSetups.h"
#include "downloading.h"

#define COVER_ROOT "./dir_env"

#define ROOT_ITSELF "/"
#define PATH_1 "/1/"

#define PATH_1_ID 2

void testDownloadCoverDownloadsCoverArt(void** state);

#endif
