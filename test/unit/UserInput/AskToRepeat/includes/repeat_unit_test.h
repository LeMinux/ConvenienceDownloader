#ifndef BOUNDED_INPUT_TESTING_H
#define BOUNDED_INPUT_TESTING_H

#include <stdio.h>

#include "testGlobals.h"
#include "testWrapInput.h"
#include "userInput.h"

typedef struct InputWithExp{
    const char* input;
    enum REPEAT exp_result;
}InputWithExp_t;

void testAsktoRepeatSingleValidLowerCase(void** state);
void testAsktoRepeatSingleValidUpperCase(void** state);

void testAsktoRepeatAllYesForms(void** state);
void testAsktoRepeatAllNoForms(void** state);

void testAsktoRepeatBeingsWithY(void** state);
void testAsktoRepeatBeingsWithN(void** state);

void testConfigToEditInvalid(void** state);
void testConfigToEditEmptyInput(void** state);

#endif
