#ifndef DEPTH_INPUT_TESTING_H
#define DEPTH_INPUT_TESTING_H

#include "testWrapInput.h"
#include "databaseUserInput.h"


void testTakeDepthInputSendInvalidOnZeroLength(void** state);
void testTakeDepthInputNegativeNumber(void** state);
void testTakeDepthInputZero(void** state);
void testTakeDepthInputInBoundNumber(void** state);
void testTakeDepthInputReallyLargeNumber(void** state);
void testTakeDepthInputReallySmallNumber(void** state);
void testTakeDepthInputInfInput(void** state);
void testTakeDepthInputRandomLetters(void** state);
void testTakeDepthInputNotAFullNumberLettersAfter(void** state);
void testTakeDepthInputNotAFullNumberLettersBefore(void** state);

//tests more about formatting
void testTakeDepthInputWithCommas(void** state);
void testTakeDepthInputWithSpaces(void** state);

#endif
