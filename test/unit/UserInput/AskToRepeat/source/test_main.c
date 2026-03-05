#include "../includes/repeat_unit_test.h"

int main(void){
    const struct CMUnitTest repeat_test[] = {
        cmocka_unit_test(testAsktoRepeatSingleValidLowerCase),
        cmocka_unit_test(testAsktoRepeatSingleValidUpperCase),

        cmocka_unit_test(testAsktoRepeatAllYesForms),
        cmocka_unit_test(testAsktoRepeatAllNoForms),

        cmocka_unit_test(testAsktoRepeatBeingsWithY),
        cmocka_unit_test(testAsktoRepeatBeingsWithN),

        cmocka_unit_test(testConfigToEditInvalid),
        cmocka_unit_test(testConfigToEditEmptyInput),
    };

    return cmocka_run_group_tests(repeat_test, NULL, NULL);
}
