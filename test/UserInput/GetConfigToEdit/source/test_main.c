#include "../includes/config_input_unit_test.h"

int main(void){
    const struct CMUnitTest config_test[] = {
        cmocka_unit_test(testConfigToEditShortHandOptionsLowerCase),
        cmocka_unit_test(testConfigToEditShortHandOptionsUpperCase),
        cmocka_unit_test(testConfigToEditInvalidShortHandOptions),

        cmocka_unit_test(testConfigToEditLongHandOptionsLowerCase),
        cmocka_unit_test(testConfigToEditLongHandOptionsUpperCase),
        cmocka_unit_test(testConfigToEditLongHandOptionsMixedCase),
        cmocka_unit_test(testConfigToEditInvalidLongHandOptions),
        cmocka_unit_test(testConfigToEditEmptyInput),
    };

    return cmocka_run_group_tests(config_test, NULL, NULL);
}



