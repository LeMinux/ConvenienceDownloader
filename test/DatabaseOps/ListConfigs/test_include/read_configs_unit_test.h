#ifndef LIST_CONFIG_TEST_H
#define LIST_CONFIG_TEST_H

#include "testGlobals.h"
#include "testSetups.h"
#include "databaseOps.h"

//Tests if the printing functions don't return error
//as stdout would need to be redirected which can affect other tests breaking isolation
void testListAllRoots(void** state);
void testListAllRootsWithPaths(void** state);
void testListRootsForConfig(void** state);
void testListRootAndPathsForConfig(void** state);

#endif
