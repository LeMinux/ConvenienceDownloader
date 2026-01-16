#ifndef ADD_MENU_TEST_H
#define ADD_MENU_TEST_H

#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "testGlobals.h"
#include "databaseOps.h"
#include "testWrapInput.h"

#define DUPLICATE_ENTRY "Duplicate/"

int createTestDB(void** state);
int closeDB(void** state);

void testAddEntryCatchesInvalidPath(void** state);
void testAddEntryCatchesInvalidDepth(void** state);
void testAddEntryEnterBothValidInput(void** state);
void testAddEntryWithBlackList(void** state);
void testAddEntryToVideoConfig(void** state);
void testAddEntryToCoverConfig(void** state);
void testAddEntryDuplicateNameButDiffConfigType(void** state);
void testAddEntryCatchesDuplicateNameAndType(void** state);

#endif
