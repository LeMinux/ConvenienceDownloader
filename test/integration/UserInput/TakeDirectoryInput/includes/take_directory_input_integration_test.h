#ifndef DIR_INPUT_TESTING_H
#define DIR_INPUT_TESTING_H

#include <unistd.h>
#include <limits.h>

#include "testWrapInput.h"
#include "testGlobals.h"
#include "databaseUserInput.h"


//trailing slashes
void testTakeDirectoryInputAbsolutePathToDirWithTrailingSlash(void** state);
void testTakeDirectoryInputAbsolutePathToDirWithoutTrailingSlash(void** state);

void testTakeDirectoryInputRelativePathToDirWithTrailingSlash(void** state);
void testTakeDirectoryInputRelativePathToDirWithoutTrailingSlash(void** state);

//tilde
void testTakeDirectoryInputTildePathToDir(void** state);
void testTakeDirectoryInputTildePathToFile(void** state);

//special cases
void testTakeDirectoryInputAbsolutePathToNonDir(void** state);
void testTakeDirectoryInputRelativePathToNonDir(void** state);
void testTakeDirectoryInputPathToNonExist(void** state);

//links
void testTakeDirectoryInputLinkToDir(void** state);
void testTakeDirectoryInputLinkToDirWithTrailingSlash(void** state);
void testTakeDirectoryInputLinkToFile(void** state);
void testTakeDirectoryInputPathToBrokenLink(void** state);
void testTakeDirectoryInputLinkToNoPerms(void** state);

//permissions
void testTakeDirectoryInputNoPermsAbsolutePath(void** state);
void testTakeDirectoryInputNoPermsRelativePath(void** state);

void testTakeDirectoryInputNoWritePerms(void **state);

//formatting
void testTakeDirectoryInputPathHasSpaces(void** state);

#endif
