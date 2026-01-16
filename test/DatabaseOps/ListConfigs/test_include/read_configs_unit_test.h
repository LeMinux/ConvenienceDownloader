#ifndef LIST_CONFIG_TEST_H
#define LIST_CONFIG_TEST_H

#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "testGlobals.h"
#include "databaseOps.h"
#include "testWrapInput.h"

int createTestDB(void** state);
int closeDB(void** state);

//Tests if the printing functions don't return error
//Testing of the contents will be for integration testing
//as stdout would need to be redirected which can affect other tests breaking isolation
void testListAllRoots(void** state);
void testListAllRootsWithPaths(void** state);
void testListRootsForConfig(void** state);
void testListRootAndPathsForConfig(void** state);

#endif
