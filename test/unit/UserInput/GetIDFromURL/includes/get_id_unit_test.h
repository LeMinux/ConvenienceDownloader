#ifndef BOUNDED_INPUT_TESTING_H
#define BOUNDED_INPUT_TESTING_H

#include "testGlobals.h"
#include "testWrapInput.h"
#include "userInput.h"

void testGetIDFromURLValidSimpleURL(void** state);
void testGetIDFromURLValidLongURLWithPlayList(void** state);
void testGetIDFromURLValidLongURLWithRadio(void** state);

void testGetIDFromURLValidURLWithDash(void** state);
void testGetIDFromURLValidURLWithUnderscore(void** state);

void testGetIDFromURLInvalidShortURL(void** state);
void testGetIDFromURLInvalidNotAYTURL(void** state);
void testGetIDFromURLInvalidIDPortion(void** state);

#endif
