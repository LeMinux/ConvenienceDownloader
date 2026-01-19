#ifndef DELETE_MENU_TEST_H
#define DELETE_MENU_TEST_H

#include "../../menu_includes/common_setups.h"

#define JAZZY_ROOT_ID 2
#define KEEPS_ROOT_ID 3
#define ARTS_ROOT_ID 6
#define PERSONAL_ROOT_ID 7

void testDeleteEntryCatchesInvalidIndex(void** state);
void testDeleteEntryAudioConfig(void** state);
void testDeleteEntryVideoConfig(void** state);
void testDeleteEntryCoverConfig(void** state);
void testDeleteEntryBlackList(void** state);

//special cases
//I delete an entry removing that index then adding another entry
//
//probably will have some helper selector method to select the correct row
//because deleting can remove certain indexes making it not straight forward to just
//use that index


#endif
