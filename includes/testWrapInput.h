#ifndef TEST_WRAP_INPUT_H
#define TEST_WRAP_INPUT_H

#include <string.h>
#include "./testGlobals.h"

int __wrap_boundedInput(FILE* stream, char* dest, size_t dest_size);

#endif
