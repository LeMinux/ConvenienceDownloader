#include "../test_include/bounded_input_unit_test.h"
#include "../test_include/take_depth_input_unit_test.h"

static int createMemFile(void** state){
    FILE* temp = tmpfile();

    if(temp == NULL){
        fprintf(stderr, "Could not create temporary file for testing\n");
        return 1;
    }

    *state = temp;
    return 0;
}

static int closeMemFile(void** state){
    fclose(*state);
    return 0;
}

int main(void){
    const struct CMUnitTest exact_input_group[] = {
        cmocka_unit_test_setup_teardown(testExactInputExactBoundWithNewline, createMemFile, closeMemFile),
        cmocka_unit_test_setup_teardown(testExactInputLessThanBoundWithNewline, createMemFile, closeMemFile),
        cmocka_unit_test_setup_teardown(testExactInputGreaterThanBoundWithNewline, createMemFile, closeMemFile),
        cmocka_unit_test_setup_teardown(testExactInputTakingOneCharWithNewline, createMemFile, closeMemFile),
        cmocka_unit_test_setup_teardown(testExactInputClearsLineToNewline, createMemFile, closeMemFile),

        cmocka_unit_test_setup_teardown(testExactInputExactBoundWithoutNewline, createMemFile, closeMemFile),
        cmocka_unit_test_setup_teardown(testExactInputLessThanBoundWithoutNewline, createMemFile, closeMemFile),
        cmocka_unit_test_setup_teardown(testExactInputGreaterThanBoundWithoutNewline, createMemFile, closeMemFile),
        cmocka_unit_test_setup_teardown(testExactInputTakingOneCharWithoutNewline, createMemFile, closeMemFile),
        cmocka_unit_test_setup_teardown(testExactInputClearsLineToEOF, createMemFile, closeMemFile),

        cmocka_unit_test_setup_teardown(testExactInputJustNewline, createMemFile, closeMemFile),
        cmocka_unit_test_setup_teardown(testExactInputNewlineAtExact, createMemFile, closeMemFile),
        cmocka_unit_test_setup_teardown(testExactInputEmptyInput, createMemFile, closeMemFile),
        cmocka_unit_test_setup_teardown(testExactInputEmptyInputResetsPrev, createMemFile, closeMemFile),
        cmocka_unit_test_setup_teardown(testExactInputMultipleLinesExactBound, createMemFile, closeMemFile),
        cmocka_unit_test_setup_teardown(testExactInputMultipleLinesBelowBound, createMemFile, closeMemFile),
        cmocka_unit_test_setup_teardown(testExactInputMultipleLinesAboveBound, createMemFile, closeMemFile),
    };

    const struct CMUnitTest take_depth_input_group[] = {
        cmocka_unit_test(testTakeDepthInputEmptyInput),
        /*
        cmocka_unit_test(testTakeDepthInputJustNewline),
        cmocka_unit_test(testTakeDepthInputNegativeNumber),
        cmocka_unit_test(testTakeDepthInputZero),
        cmocka_unit_test(testTakeDepthInputInBoundNumber),
        cmocka_unit_test(testTakeDepthInputReallyLargeNumber),
        cmocka_unit_test(testTakeDepthInputInfInput),
        cmocka_unit_test(testTakeDepthInputRandomLetters),
        cmocka_unit_test(testTakeDepthInputNotAFullNumberLettersAfter),
        cmocka_unit_test(testTakeDepthInputNotAFullNumberLettersBefore),
        cmocka_unit_test(testTakeDepthInputWithCommas)
        */
    };

    return (
        cmocka_run_group_tests(exact_input_group, NULL, NULL) &&
        cmocka_run_group_tests(take_depth_input_group, NULL, NULL)
    );
}
