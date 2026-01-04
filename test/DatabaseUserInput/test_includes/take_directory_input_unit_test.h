#ifndef DIR_INPUT_TESTING_H
#define DIR_INPUT_TESTING_H

#include "../../testGlobals.h"
#include "../../../includes/databaseUserInput.h"


void testTakeDirectoryInputPathToDir(void** state);
void testTakeDirectoryInputPathToFile(void** state);
void testTakeDirectoryInputPathToNonExist(void** state);

void testTakeDirectoryInputPathToLink(void** state);

void testTakeDirectoryInputPathToBrokenLink(void** state);

void testTakeDirectoryInputLinkToDevFile(void** state);

#endif
