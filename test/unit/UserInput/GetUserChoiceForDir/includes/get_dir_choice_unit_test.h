#ifndef DIR_CHOICE_INPUT_TESTING_H
#define DIR_CHOICE_INPUT_TESTING_H

#include "testSetups.h"
#include "testWrapInput.h"
#include "userInput.h"

void getUserDirChoiceNoRowsIsSkip(void** state);
void getUserDirChoiceSkippingIndexIsSkipping(void** state);

//Simulates when the roots have been added at different times like
//root 'D' is added before root 'A'
void getUserDirChoiceRootItselfUsingAlphabeticalSorting(void** state);

//root 'A' added before root 'D'
void getUserDirChoiceRootItselfAlreadyAlphabetical(void** state);

//path 'D' added before dir 'A'
void getUserDirChoicePathUsingAlphabeticalSorting(void** state);

//path 'A' added before dir 'D'
void getUserDirChoicePathAlreadyAlphabetical(void** state);

#endif
