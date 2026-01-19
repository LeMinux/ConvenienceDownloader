#ifndef ADD_MENU_TEST_H
#define ADD_MENU_TEST_H

#include "../../menu_includes/common_setups.h"

#define DUPLICATE_ENTRY "Duplicate/"

void testAddEntryCatchesInvalidPath(void** state);
void testAddEntryCatchesInvalidDepth(void** state);
void testAddEntryEnterBothValidInput(void** state);
void testAddEntryWithBlackList(void** state);
void testAddEntryToVideoConfig(void** state);
void testAddEntryToCoverConfig(void** state);
void testAddEntryDuplicateNameButDiffConfigType(void** state);
void testAddEntryCatchesDuplicateNameAndType(void** state);

#endif
