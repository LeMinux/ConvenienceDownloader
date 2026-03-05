#ifndef INDEX_INPUT_TESTING_H
#define INDEX_INPUT_TESTING_H

#include "testWrapInput.h"
#include "userInput.h"

#define MAX_INDEX 2000

void testTakeIndexInputSendSkippingOnZeroLength(void** state);
void testTakeIndexInputJustNewline(void** state);
void testTakeIndexInputNegativeNumber(void** state);
void testTakeIndexInputZero(void** state);
void testTakeIndexInputInBoundNumber(void** state);
void testTakeIndexInputEqualMax(void** state);
void testTakeIndexInputBeyondMax(void** state);
void testTakeIndexInputReallyLargeNumber(void** state);
void testTakeIndexInputReallySmallNumber(void** state);
void testTakeIndexInputVeryLongInputJustNumbers(void** state);
void testTakeIndexInputNumberPaddedOutOfRange(void** state);
void testTakeIndexInputNumberPaddedOutOfRange(void** state);
void testTakeIndexInput10Spaces(void** state);
void testTakeIndexInputRandomLetters(void** state);
void testTakeIndexInputNotAFullNumberLettersAfter(void** state);
void testTakeIndexInputNotAFullNumberLettersBefore(void** state);

//tests more about formatting
void testTakeIndexInputWithCommas(void** state);
void testTakeIndexInputWithSpaces(void** state);

#endif
