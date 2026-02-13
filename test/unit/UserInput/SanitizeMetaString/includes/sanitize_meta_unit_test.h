#ifndef SANITIZE_META_TESTING_H
#define SANITIZE_META_TESTING_H

#include <string.h>

#include "testGlobals.h"
#include "userInput.h"

void testSanitizeMetaStringNoChange(void** state);
void testSanitizeMetaStringChangesOutputFormat(void** state);
void testSanitizeMetaStringChangesOutputFormatHasSpaces(void** state);
void testSanitizeMetaStringChangesOutputFormatHasSemicolon(void** state);
void testSanitizeMetaStringBrackets(void** state);
void testSanitizeMetaStringGiveZeroLength(void** state);

#endif
