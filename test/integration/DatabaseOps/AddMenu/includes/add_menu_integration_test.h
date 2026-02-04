#ifndef ADD_MENU_TEST_H
#define ADD_MENU_TEST_H

#include "testGlobals.h"
#include "testSetups.h"
#include "testWrapInput.h"

typedef struct PathCheck{
    const char* exp_path_name;
    const int exp_path_len;
}PathCheck_t;

#define NOT_FOUND -1

#define ROOT1 "./dir_env/root1/"
#define ROOT2 "./dir_env/root2/"

#define ROOT_ITSELF "/"
#define LEFT_DIR "/left_dir/"
#define LEFT_DIR_LEFT "/left_dir/inner_left/"
#define LEFT_DIR_RIGHT "/left_dir/inner_right/"
#define LEFT_DIR_MOST_INNER "/left_dir/inner_right/inner_inner_dir/"


#define RIGHT_DIR "/right_dir/"
#define RIGHT_DIR_LEFT "/right_dir/inner_left/"
#define RIGHT_DIR_RIGHT "/right_dir/inner_right/"
#define RIGHT_DIR_MOST_INNER "/right_dir/inner_left/inner_inner_dir/"

#define DUPLICATE_ENTRY RIGHT_DIR
#define IN_BLACK_LIST RIGHT_DIR

void testAddMenuCatchesInvalidPath(void** state);
void testAddMenuCatchesInvalidDepth(void** state);
void testAddMenuCatchesDuplicateNameAndType(void** state);
void testAddMenuCatchesPathInBlackList(void** state);
void testAddMenuCatchesSkippingPath(void** state);
void testAddMenuCatchesSkippingDepth(void** state);

void testAddMenuEnterInfInputOnRootDir(void** state);
void testAddMenuLargerDepthThanWhatRootHas(void** state);
void testAddMenuSmallerDepthThanWhatRootHas(void** state);
void testAddMenuZeroDepthOnRoot(void** state);
void testAddMenuOneDepthOnRoot(void** state);
void testAddMenuMaxDepth(void** state);

void testAddMenuToBlackList(void** state);
void testAddMenuToVideoConfig(void** state);
void testAddMenuToCoverConfig(void** state);

void testAddMenuDuplicateNameButDiffConfigType(void** state);


void testAddMenuPathIsInBlackList(void** state);

#endif
