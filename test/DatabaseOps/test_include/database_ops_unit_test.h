#ifndef DATABASE_OPS_TEST_H
#define DATABASE_OPS_TEST_H

#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "testGlobals.h"
#include "databaseOps.h"
#include "testWrapInput.h"

#define WRITE_END 1
#define READ_END 0

char* __wrap_takeDirectoryInput(void);
int __wrap_takeDepthInput(void);


int createTestDB(void** state);
int closePerGroupDB(void** state);

//Tests if the printing functions don't return error
//Testing of the contents will be for integration testing
//as stdout would need to be redirected which can affect other tests breaking isolation
void testListAllRoots(void** state);
void testListAllRootsWithPaths(void** state);
void testListRootsForConfig(void** state);
void testListRootAndPathsForConfig(void** state);

void testAddEntryCatchesInvalidPath(void** state);
void testAddEntryCatchesInvalidDepth(void** state);
void testAddEntryEnterBothValidInput(void** state);
void testAddEntryWithBlackList(void** state);
void testAddEntryToVideoConfig(void** state);
void testAddEntryToCoverConfig(void** state);

//probably want just 2 entries to make sure that updating isn't 
//affecting other rows
void testUpdateEntryCatchesInvalidDepth(void** state);
void testUpdateEntryCatchesInvalidPath(void** state);
void testUpdateEntryDiffPathAndDiffDepth(void** state);
void testUpdateEntryDiffPathAndSameDepth(void** state);
void testUpdateEntrySamePathAndDiffDepth(void** state);
void testUpdateEntrySamePathAndSameDepth(void** state);
void testUpdateBlackList(void** state);
void testUpdateVideoConfig(void** state);
void testUpdateCoverConfig(void** state);

//check if root and its paths are gone
void testDeleteEntry(void** state);

//might have to add this somewhere else as mocking the tested functions isn't
//a great idea
void testEditMenuCallsAddMenu(void** state);
void testEditMenuCallsUpdateMenu(void** state);
void testEditMenuCallsDelMenu(void** state);
void testEditMenuExits(void** state);
void testEditMenuCatchesInvalidOption(void** state);

//special cases
//I delete an entry removing that index then adding another entry
//
//probably will have some helper selector method to select the correct row
//because deleting can remove certain indexes making it not straight forward to just
//use that index


#endif
