#ifndef ADD_MENU_TEST_H
#define ADD_MENU_TEST_H

#include "testSetups.h"

typedef struct PathCheck{
    char* exp_path_name;
    int exp_path_len;
}PathCheck_t;

#define NOT_FOUND -1

#define ROOT1 "../../dir_env/root1/"
#define ROOT2 "../../dir_env/root2/"

#define LEFT_DIR "left_dir"
#define LEFT_DIR_LEFT "left_dir/inner_left"
#define LEFT_DIR_RIGHT "left_dir/inner_right"
#define LEFT_DIR_MOST_INNER "left_dir/inner_right/inner_inner_dir"


#define RIGHT_DIR "right_dir"
#define RIGHT_DIR_LEFT "right_dir/inner_left"
#define RIGHT_DIR_RIGHT "right_dir/inner_right"
#define RIGHT_DIR_MOST_INNER "right_dir/inner_right/inner_inner_dir"

void testAddEntryCatchesInvalidPath(void** state);
void testAddEntryCatchesInvalidDepth(void** state);
void testAddEntryCatchesPathInBlackList(void** state);

void testAddEntryEnterInfInputOnRootDir(void** state);
void testAddEntryWithBlackList(void** state);
void testAddEntryToVideoConfig(void** state);
void testAddEntryToCoverConfig(void** state);

void testAddEntryDuplicateNameButDiffConfigType(void** state);
void testAddEntryCatchesDuplicateNameAndType(void** state);

#endif
