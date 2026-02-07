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
#define ROCK_ROOT "/home/user/Music/Rock"
#define JAZZ_ROOT "/home/user/Music/Jazzy"

#define KEEPS_ROOT "/home/user/Videos/Keeps"
#define MVS_ROOT "/home/user/Videos/MVs"

#define CVS_ROOT "/home/user/Pictures/CVs"
#define ARTS_ROOT "/home/user/Pictures/Arts"

#define BLACK_ROOT_1 "/home/user/Pictures/Personal"
#define BLACK_ROOT_2 "/home/user/Pictures/Sensitives"

#define ROCK_PATH_1 "/Men At Work/"
#define ROCK_PATH_2 "/Rainbow/"
#define JAZZ_PATH_1 "/Big Bad Voodoo Daddy/"
#define JAZZ_PATH_2 "/Funky/"

#define KEEPS_PATH_1 "/Youtube/"
#define KEEPS_PATH_2 "/Documentary/"
#define MVS_PATH_1 "/OtherVideos/"
#define MVS_PATH_2 "/EvenMoreVideos/"

#define CVS_PATH_1 "/CoverArts/"
#define CVS_PATH_2 "/AlbumCovers/"
#define ARTS_PATH_1 "/SomeArts/"
#define ARTS_PATH_2 "/SurprisinglyMoreArts/"

#define TOTAL_ROWS 26

//root IDs
#define ROCK_ROOT_ID 1
#define JAZZ_ROOT_ID 2
#define KEEPS_ROOT_ID 3
#define MVS_ROOT_ID 4
#define CVS_ROOT_ID 5
#define ARTS_ROOT_ID 6
#define PERSONAL_ROOT_ID 7

//path IDs
#define ROCK_PATH_ITSELF_ID 1
#define ROCK_PATH_MAW_ID 2
#define ROCK_PATH_RAINBOW_ID 3

#define JAZZ_PATH_ITSELF_ID 4
#define JAZZ_PATH_BBVD_ID 5
#define JAZZ_PATH_FUNKY_ID 6

#define KEEPS_PATH_ITSELF_ID 7
#define KEEPS_PATH_DOCUMENTARY_ID 8
#define KEEPS_PATH_YOUTUBE_ID 9

#define MVS_PATH_ITSELF_ID 10
#define MVS_PATH_OTHERS_ID 11
#define MVS_PATH_EVEN_MORE_ID 12

#define CVS_PATH_ITSELF_ID 13
#define CVS_PATH_COVER_ID 14
#define CVS_PATH_ALBUMN_ID 15

#define ARTS_PATH_ITSELF_ID 16
#define ARTS_PATH_SOME_ID 17
#define ARTS_PATH_MORE_ID 18


//The selections below account for the expect alphabetic ordering

//Root index selection
#define JAZZ_ROOT_INDEX 1
#define ROCK_ROOT_INDEX 2
#define KEEPS_ROOT_INDEX 1
#define MVS_ROOT_INDEX 2
#define ARTS_ROOT_INDEX 1
#define CVS_ROOT_INDEX 2
#define PERSONAL_ROOT_INDEX 1
#define SENSITIVES_ROOT_INDEX 2

//Path index selection
#define JAZZ_PATH_ITSELF_SEL 1
#define JAZZ_PATH_BBVD_SEL 2
#define JAZZ_PATH_FUNKY_SEL 3

#define ROCK_PATH_ITSELF_SEL 4
#define ROCK_PATH_MAW_SEL 5
#define ROCK_PATH_RAINBOW_SEL 6

#define KEEPS_PATH_ITSELF_SEL 1
#define KEEPS_PATH_YOUTUBE_SEL 2
#define KEEPS_PATH_DOCUMENTARY_SEL 3

#define MVS_PATH_ITSELF_SEL 4
#define MVS_PATH_OTHERS_SEL 5
#define MVS_PATH_EVEN_MORE_SEL 6

#define ARTS_PATH_ITSELF_SEL 1
#define ARTS_PATH_SOME_SEL 2
#define ARTS_PATH_MORE_SEL 3

#define CVS_PATH_ITSELF_SEL 4
#define CVS_PATH_COVER_SEL 5
#define CVS_PATH_ALBUMN_SEL 6

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
