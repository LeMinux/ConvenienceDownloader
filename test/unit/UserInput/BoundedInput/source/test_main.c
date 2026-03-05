#include "../includes/bounded_input_unit_test.h"

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

        cmocka_unit_test_setup_teardown(testExactInputSetItCouldNotFindNewLineIfInputIsTooLarge, createMemFile, closeMemFile),
        cmocka_unit_test_setup_teardown(testExactInputSetItCouldNotFindEOFIfInputIsTooLarge, createMemFile, closeMemFile),
    };

    return cmocka_run_group_tests(exact_input_group, NULL, NULL);
}
