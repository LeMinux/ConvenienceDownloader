#ifndef DELETE_MENU_TEST_H
#define DELETE_MENU_TEST_H

#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "testGlobals.h"
#include "databaseOps.h"
#include "testWrapInput.h"

int createTestDB(void** state);
int closeDB(void** state);

//check if root and its paths are gone
void testDeleteEntry(void** state);

//special cases
//I delete an entry removing that index then adding another entry
//
//probably will have some helper selector method to select the correct row
//because deleting can remove certain indexes making it not straight forward to just
//use that index


#endif
