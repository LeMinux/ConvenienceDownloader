#ifndef DIR_INPUT_TESTING_H
#define DIR_INPUT_TESTING_H

#include "testGlobals.h"
#include "fileOps.h"

void testOpenFileOpenRegFile(void** state);
void testOpenFileOpenDir(void** state);
void testOpenFileNonExist(void** state);
void testOpenFileDevFile(void** state);

//Really these tests below are regression tests because the OS handles it with the open() syscall.
//However if implementation changes and for some reason I can't open a symlink to a file then it should be known.
void testOpenFileLinkToFile(void** state);
void testOpenFileLinkToDir(void** state);
void testOpenFileBrokenLink(void** state);

void testOpenFileAbsolutePathToFile(void** state);

void testOpenFileNoPerms(void** state);

#endif
