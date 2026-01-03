#ifndef DEPTH_INPUT_TESTING_H
#define DEPTH_INPUT_TESTING_H

#include "../../testGlobals.h"
#include "../../../includes/userInput.h"

int __wrap_boundedInput(FILE* stream, char* dest, size_t dest_size);

void testTakeDepthInputEmptyInput(void** state);
void testTakeDepthInputJustNewline(void** state);
void testTakeDepthInputNegativeNumber(void** state);
void testTakeDepthInputZero(void** state);
void testTakeDepthInputInBoundNumber(void** state);
void testTakeDepthInputReallyLargeNumber(void** state);
void testTakeDepthInputInfInput(void** state);
void testTakeDepthInputRandomLetters(void** state);
void testTakeDepthInputNotAFullNumberLettersAfter(void** state);
void testTakeDepthInputNotAFullNumberLettersBefore(void** state);

//tests more about formatting
void testTakeDepthInputWithCommas(void** state);

#endif
