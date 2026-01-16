#ifndef UPDATE_MENU_TEST_H
#define UPDATE_MENU_TEST_H

#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "testGlobals.h"
#include "databaseOps.h"
#include "testWrapInput.h"

int createTestDB(void** state);
int closeDB(void** state);

//probably want just 2 entries to make sure that updating isn't
//affecting other rows
void testUpdateEntryCatchesInvalidDepth(void** state);
void testUpdateEntryCatchesInvalidPath(void** state);
void testUpdateEntryCatchesInvalidIndex(void** state);

void testUpdateEntryDiffPathAndDiffDepth(void** state);
void testUpdateEntryDiffPathAndSameDepth(void** state);
void testUpdateEntrySamePathAndDiffDepth(void** state);
void testUpdateEntrySamePathAndSameDepth(void** state);
void testUpdateBlackList(void** state);
void testUpdateVideoConfig(void** state);
void testUpdateCoverConfig(void** state);

#endif
