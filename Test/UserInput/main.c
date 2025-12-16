#include "./user_input_unit_test.h"

int main(void){
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(testExactInputExactBoundWithNewline),
        cmocka_unit_test(testExactInputLessThanBoundWithNewline),
        cmocka_unit_test(testExactInputGreaterThanBoundWithNewline),
        cmocka_unit_test(testExactInputClearsLineWithNewline),

        cmocka_unit_test(testExactInputExactBoundWithoutNewline),
        cmocka_unit_test(testExactInputLessThanBoundWithoutNewline),
        cmocka_unit_test(testExactInputGreaterThanBoundWithoutNewline),
        cmocka_unit_test(testExactInputClearsLineWithoutNewline),

        cmocka_unit_test(testExactInputJustNewline),
        cmocka_unit_test(testExactInputNewlineAtExact),
        cmocka_unit_test(testExactInputEmptyInput),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
