#ifndef DATABASE_OPS_TEST_H
#define DATABASE_OPS_TEST_H

#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../../testGlobals.h"
#include "../../../includes/databaseOps.h"

#define WRITE_END 1
#define READ_END 0

int createTestDB(void** state);
int closePerTestDB(void** state);

//Tests if the printing functions don't return error
//Testing of the contents will be for integration testing
//as stdout would need to be redirected which can affect other tests breaking isolation
void testListAllRoots(void** state);
void testListAllRootsWithPaths(void** state);
void testListRootsForConfig(void** state);
void testListRootAndPathsForConfig(void** state);

void testAddEntryAbsolutePath(void** state);
void testAddEntryEmtpy(void** state);
void testAddEntryVeryLong(void** state);
void testAddEntryRelativePath(void** state);
void testAddEntryLinkToFile(void** state);
void testAddEntryLinkToDir(void** state);
void testAddEntryFileInvalidPermissions(void** state);
void testAddEntryUsingTildeShortcut(void** state);

//probably want just 2 entries to make sure that updating isn't 
//affecting other rows
void testUpdateEntryDiffPathAndDiffDepth(void** state);
void testUpdateEntryDiffPathAndSameDepth(void** state);
void testUpdateEntrySamePathAndDiffDepth(void** state);
void testUpdateEntrySamePathAndSameDepth(void** state);
void testUpdateEntryRelativePath(void** state);
void testUpdateEntryNegativeDepth(void** state);
void testUpdateEntryReallyLargeDepth(void** state);
void testUpdateEntryInfiniteDepth(void** state);

//check if root and its paths are gone
void testDeleteEntry(void** state);

//special cases
//I delete an entry removing that index then adding another entry
//
//probably will have some helper selector method to select the correct row
//because deleting can remove certain indexes making it not straight forward to just
//use that index


#endif
