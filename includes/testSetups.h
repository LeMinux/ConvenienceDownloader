#ifndef COMMON_SETUPS_H
#define COMMON_SETUPS_H

#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>

#include "globals.h"
#include "testGlobals.h"
#include "databaseOps.h"

//Really only 4 roots and one path each is necessary for testing functionality,
//but in order to test if updates or deletes are only acting on their rows
//having another group within their config_type helps test that.
//Essentially testing if the WHERE clause is strict enough to not modify other rows
#define AUDIO_ROOT_1 "/home/user/Music/Rock/"
#define AUDIO_ROOT_2 "/home/user/Music/Jazzy/"

#define VIDEO_ROOT_3 "/home/user/Videos/Keeps/"
#define VIDEO_ROOT_4 "/home/user/Videos/MVs/"

#define COVER_ROOT_5 "/home/user/Pictures/CVs/"
#define COVER_ROOT_6 "/home/user/Pictures/Arts/"

#define BLACK_ROOT_7 "/home/user/Pictures/Personal/"
#define BLACK_ROOT_8 "/home/user/Pictures/Sensitives/"

#define AUDIO_PATH_1 "Men At Work/"
#define AUDIO_PATH_2 "Rainbow/"
#define AUDIO_PATH_3 "Big Bad Voodoo Daddy/"
#define AUDIO_PATH_4 "Funky/"

#define VIDEO_PATH_1 "Youtube/"
#define VIDEO_PATH_2 "Documentary/"
#define VIDEO_PATH_3 "OtherVideos/"
#define VIDEO_PATH_4 "EvenMoreVideos/"

#define COVER_PATH_1 "CoverArts/"
#define COVER_PATH_2 "AlbumCovers/"
#define COVER_PATH_3 "SomeArts/"
#define COVER_PATH_4 "SurprisinglyMoreArts/"

#define AUDIO_DEPTH_1 5
#define AUDIO_DEPTH_2 5

#define TOTAL_ROWS 20

//locations of the test file will vary
//so they need to give a path to where they are.
//Of course where the makefile is ran determines relative positioning
extern const char* init_db_path;

int createTestDB(void** state);
int createTestDBWithRoots(void** state);
int closeDB(void** state);
int createTestDBWithRootsAndPaths(void** state);
void addExtraRootEntry(sqlite3* test_db, enum CONFIG config, const char* extra_name, int depth);

#endif
