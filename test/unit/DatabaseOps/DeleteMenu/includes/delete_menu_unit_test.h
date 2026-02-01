#ifndef DELETE_MENU_TEST_H
#define DELETE_MENU_TEST_H

#include "testGlobals.h"
#include "testSetups.h"
#include "databaseOps.h"

#define JAZZY_ROOT_ID 2
#define KEEPS_ROOT_ID 3
#define ARTS_ROOT_ID 6
#define PERSONAL_ROOT_ID 7

void testDeleteEntryCatchesInvalidIndex(void** state);
void testDeleteEntryAudioConfig(void** state);
void testDeleteEntryVideoConfig(void** state);
void testDeleteEntryCoverConfig(void** state);
void testDeleteEntryBlackList(void** state);

#endif
