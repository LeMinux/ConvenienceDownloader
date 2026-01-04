#ifndef BOUNDED_INPUT_TESTING_H
#define BOUNDED_INPUT_TESTING_H

#include "../../testGlobals.h"
#include "../../../includes/userInput.h"

#define TEST_BUFFER_SIZE 21

//Testing the differences in streams like stdin vs a file isn't necessary
//They are both FILE* streams, and testing OS nuance isn't needed.
//There are of course differences like how you can't ftell or fseek if it's a buffered stream

//simulates reading from a user
void testExactInputExactBoundWithNewline(void** state);
void testExactInputLessThanBoundWithNewline(void** state);
void testExactInputGreaterThanBoundWithNewline(void** state);
void testExactInputTakingOneCharWithNewline(void** state);

//simulates reading a file where it reaches EOF
void testExactInputExactBoundWithoutNewline(void** state);
void testExactInputLessThanBoundWithoutNewline(void** state);
void testExactInputGreaterThanBoundWithoutNewline(void** state);
void testExactInputTakingOneCharWithoutNewline(void** state);

//testing if clearing to end of line works
void testExactInputClearsLineToNewline(void** state);
void testExactInputClearsLineToEOF(void** state);

//testing potential edge cases
void testExactInputJustNewline(void** state);
void testExactInputEmptyInput(void** state);
void testExactInputNewlineAtExact(void** state);
void testExactInputEmptyInputResetsPrev(void** state);

//test file pointer positions
void testExactInputMultipleLinesExactBound(void** state);
void testExactInputMultipleLinesBelowBound(void** state);
void testExactInputMultipleLinesAboveBound(void** state);
void testExactInputMultipleLinesMixedBound(void** state);

#endif
