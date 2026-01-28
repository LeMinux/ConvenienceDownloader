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

#define LEFT_DIR "left_dir"
#define LEFT_DIR_LEFT "left_dir/inner_left"
#define LEFT_DIR_RIGHT "left_dir/inner_right"
#define LEFT_DIR_MOST_INNER "left_dir/inner_right/inner_inner_dir"


#define RIGHT_DIR "right_dir"
#define RIGHT_DIR_LEFT "right_dir/inner_left"
#define RIGHT_DIR_RIGHT "right_dir/inner_right"
#define RIGHT_DIR_MOST_INNER "right_dir/inner_left/inner_inner_dir"

#define DUPLICATE_ENTRY RIGHT_DIR
#define IN_BLACK_LIST RIGHT_DIR

void testUpdateMenuCatchesInvalidPath(void** state);
void testUpdateMenuCatchesInvalidDepth(void** state);
void testUpdateMenuCatchesInvalidIndex(void** state);
void testUpdateMenuCatchesPathInBlackList(void** state);
void testUpdateMenuCatchesDuplicateNameAndType(void** state);

void testUpdateMenuEnterInfInputOnRootDir(void** state);
void testUpdateMenuLargerDepthThanWhatRootHas(void** state);
void testUpdateMenuSmallerDepthThanWhatRootHas(void** state);
void testUpdateMenuZeroDepthOnRoot(void** state);
void testUpdateMenuOneDepthOnRoot(void** state);
void testUpdateMenuMaxDepth(void** state);

void testUpdateMenuToBlackList(void** state);
void testUpdateMenuToVideoConfig(void** state);
void testUpdateMenuToCoverConfig(void** state);

void testUpdateMenuDuplicateNameButDiffConfigType(void** state);
void testUpdateMenuPathIsInBlackList(void** state);

#endif
