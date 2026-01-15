#include "../test_includes/take_index_input_unit_test.h"
#include "../test_includes/take_depth_input_unit_test.h"

int main(void){
    const struct CMUnitTest take_depth_input_group [] = {
        cmocka_unit_test(testTakeDepthInputEmptyInput),
        cmocka_unit_test(testTakeDepthInputJustNewline),
        cmocka_unit_test(testTakeDepthInputNegativeNumber),
        cmocka_unit_test(testTakeDepthInputZero),
        cmocka_unit_test(testTakeDepthInputInBoundNumber),
        cmocka_unit_test(testTakeDepthInputReallyLargeNumber),
        cmocka_unit_test(testTakeDepthInputReallySmallNumber),
        cmocka_unit_test(testTakeDepthInputInfInput),
        cmocka_unit_test(testTakeDepthInputRandomLetters),
        cmocka_unit_test(testTakeDepthInputNotAFullNumberLettersAfter),
        cmocka_unit_test(testTakeDepthInputNotAFullNumberLettersBefore),
        cmocka_unit_test(testTakeDepthInputWithCommas),
        cmocka_unit_test(testTakeDepthInputWithSpaces)
    };

    const struct CMUnitTest take_index_input_group [] = {
        cmocka_unit_test(testTakeIndexInputEmptyInput),
        cmocka_unit_test(testTakeIndexInputJustNewline),
        cmocka_unit_test(testTakeIndexInputNegativeNumber),
        cmocka_unit_test(testTakeIndexInputZero),
        cmocka_unit_test(testTakeIndexInputInBoundNumber),
        cmocka_unit_test(testTakeIndexInputEqualMax),
        cmocka_unit_test(testTakeIndexInputBeyondMax),
        cmocka_unit_test(testTakeIndexInputReallyLargeNumber),
        cmocka_unit_test(testTakeIndexInputReallySmallNumber),
        cmocka_unit_test(testTakeIndexInputRandomLetters),
        cmocka_unit_test(testTakeIndexInputNotAFullNumberLettersAfter),
        cmocka_unit_test(testTakeIndexInputNotAFullNumberLettersBefore),
        cmocka_unit_test(testTakeIndexInputWithCommas),
        cmocka_unit_test(testTakeIndexInputWithSpaces),
    };

    return
    (
    cmocka_run_group_tests(take_depth_input_group, NULL, NULL) &&
    cmocka_run_group_tests(take_index_input_group, NULL, NULL)
    );
}
