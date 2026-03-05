#include "../includes/get_url_unit_test.h"

int main(void){
    const struct CMUnitTest id_test[] = {
        cmocka_unit_test(testGetIDFromURLValidSimpleURL),
        cmocka_unit_test(testGetIDFromURLValidLongURLWithPlayList),
        cmocka_unit_test(testGetIDFromURLValidLongURLWithRadio),
        cmocka_unit_test(testGetIDFromURLValidURLWithDash),
        cmocka_unit_test(testGetIDFromURLValidURLWithUnderscore),

        cmocka_unit_test(testGetIDFromURLInvalidShortURL),
        cmocka_unit_test(testGetIDFromURLInvalidNotAYTURL),
        cmocka_unit_test(testGetIDFromURLInvalidIDPortion),
    };

    return cmocka_run_group_tests(id_test, NULL, NULL);
}
