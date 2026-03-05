#ifndef PATH_TO_PATH_TEST_H
#define PATH_TO_PATH_TEST_H

#include "testGlobals.h"
#include "testSetups.h"
#include "databaseOps.h"

void testPathIdToPathRootItself(void** state);
void testPathIdToPathAudioPath(void** state);
void testPathIdToPathVideoPath(void** state);
void testPathIdToPathCoverPath(void** state);

#endif
