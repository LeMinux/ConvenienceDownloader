#ifndef UPDATE_MENU_TEST_H
#define UPDATE_MENU_TEST_H

#include "../../menu_includes/common_setups.h"

void updateSetUp(sqlite3* database);

void testUpdateEntryCatchesInvalidDepth(void** state);
void testUpdateEntryCatchesInvalidPath(void** state);
void testUpdateEntryCatchesInvalidIndex(void** state);

void testUpdateEntryDiffPathAndDiffDepth(void** state);
void testUpdateEntryDiffPathAndSameDepth(void** state);
void testUpdateEntrySamePathAndDiffDepth(void** state);
void testUpdateEntrySamePathAndSameDepth(void** state);
void testUpdateEntryBlackList(void** state);
void testUpdateEntryVideoConfig(void** state);
void testUpdateEntryCoverConfig(void** state);

void testUpdateEntryDuplicateNameInSameConfig(void** state);
void testUpdateEntryDuplicateNameInDiffConfig(void** state);

void testUpdateEntryAddingExistingToBlackList(void** state);
#endif
