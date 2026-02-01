#ifndef BOUNDED_INPUT_TESTING_H
#define BOUNDED_INPUT_TESTING_H

#include <stdio.h>

#include "testGlobals.h"
#include "globals.h"
#include "userInput.h"

typedef struct InputWithExp{
    const char* input;
    enum CONFIG exp_result;
}InputWithExp_t;

void testConfigToEditShortHandOptionsLowerCase(void** state);
void testConfigToEditShortHandOptionsUpperCase(void** state);
void testConfigToEditInvalidShortHandOptions(void** state);

void testConfigToEditLongHandOptionsLowerCase(void** state);
void testConfigToEditLongHandOptionsUpperCase(void** state);
void testConfigToEditLongHandOptionsMixedCase(void** state);
void testConfigToEditInvalidLongHandOptions(void** state);

void testConfigToEditEmptyInput(void** state);


#endif
