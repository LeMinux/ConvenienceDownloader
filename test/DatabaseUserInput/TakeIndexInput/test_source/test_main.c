#include "../test_includes/take_index_input_unit_test.h"

int main(void){
    const struct CMUnitTest take_index_input_group [] = {
        cmocka_unit_test(testTakeIndexInputSendInvalidOnZeroLength),
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

    return cmocka_run_group_tests(take_index_input_group, NULL, NULL);
}
