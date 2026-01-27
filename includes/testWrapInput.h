#ifndef TEST_WRAP_INPUT_H
#define TEST_WRAP_INPUT_H

#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "./testGlobals.h"

enum CALL{NO_CALL = 0, CALL};

extern enum CALL call_real_function;

int __wrap_boundedInput(FILE* stream, char* dest, size_t dest_size);
int __wrap_takeIndexInput(int max);
int __wrap_takeDepthInput(void);
char* __wrap_takeDirectoryInput(void);

#endif
