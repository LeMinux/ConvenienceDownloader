#ifndef DELETE_MENU_TEST_H
#define DELETE_MENU_TEST_H

#include "testGlobals.h"
#include "testSetups.h"
#include "databaseOps.h"

void testDeleteEntryCatchesInvalidIndex(void** state);
void testDeleteMenuCatchesNoRows(void** state);
void testDeleteMenuCatchesSkipping(void** state);

void testDeleteEntryAudioConfig(void** state);
void testDeleteEntryVideoConfig(void** state);
void testDeleteEntryCoverConfig(void** state);
void testDeleteEntryBlackList(void** state);

#endif
