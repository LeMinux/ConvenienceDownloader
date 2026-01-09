#ifndef DIR_INPUT_TESTING_H
#define DIR_INPUT_TESTING_H

#include <unistd.h>
#include <limits.h>

#include "testWrapInput.h"
#include "testGlobals.h"
#include "databaseUserInput.h"


void testTakeDirectoryInputAbsolutePathToDir(void** state);
void testTakeDirectoryInputAbsolutePathToFile(void** state);

void testTakeDirectoryInputRelativePathToDir(void** state);
void testTakeDirectoryInputRelativePathToFile(void** state);

void testTakeDirectoryInputTildePathToDir(void** state);
void testTakeDirectoryInputTildePathToFile(void** state);

void testTakeDirectoryInputPathToNonExist(void** state);

void testTakeDirectoryInputLinkToDir(void** state);
void testTakeDirectoryInputLinkToDirWithTrailingSlash(void** state);
void testTakeDirectoryInputLinkToFile(void** state);
void testTakeDirectoryInputPathToBrokenLink(void** state);
void testTakeDirectoryInputLinkToNoPerms(void** state);

//permissions
void testTakeDirectoryInputNoPermsAbsolutePath(void** state);
void testTakeDirectoryInputNoPermsRelativePath(void** state);
void testTakeDirectoryInputLinkToNoPerms(void** state);

void testTakeDirectoryInputNoWritePerms(void **state);

//formatting

void testTakeDirectoryInputNoSlashAtEndAboslute(void** state);
void testTakeDirectoryInputNoSlashAtEndRelative(void** state);

#endif
