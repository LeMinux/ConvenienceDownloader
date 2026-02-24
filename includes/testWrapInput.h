#ifndef TEST_WRAP_INPUT_H
#define TEST_WRAP_INPUT_H

#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "testGlobals.h"
#include "userInput.h"

size_t __wrap_boundedInput(FILE* stream, char* dest, size_t size, enum FOUND_END* end);
int __wrap_takeIndexInput(int max);
int __wrap_takeDepthInput(void);
char* __wrap_takeDirectoryInput(void);

#endif
