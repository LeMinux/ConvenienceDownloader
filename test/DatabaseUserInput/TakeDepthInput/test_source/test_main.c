#include "../test_includes/take_depth_input_unit_test.h"

int main(void){
    const struct CMUnitTest take_depth_input_group [] = {
        cmocka_unit_test(testTakeDepthInputSendInvalidOnZeroLength),
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

    return cmocka_run_group_tests(take_depth_input_group, NULL, NULL);
}
