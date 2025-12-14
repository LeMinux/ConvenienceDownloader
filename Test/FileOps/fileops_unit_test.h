#ifndef UNIT_TEST_FILEOPS
#define UNIT_TEST_FILEOPS

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "../..//includes/fileOps.h"

//NASA wouldn't like it, but I'm not really sure how else to do it

#undef AUDIO_CONFIG_PATH
#undef VIDEO_CONFIG_PATH
#undef COVER_CONFIG_PATH

//redefine config paths for testing purposes

#define AUDIO_CONFIG_PATH "./ConfigFiles/testAudioDests.txt"
#define VIDEO_CONFIG_PATH "./ConfigFiles/testVideoDests.txt"
#define COVER_CONFIG_PATH "./ConfigFiles/testCoverDests.txt"

#endif
