#include "../includes/sanitize_meta_unit_test.h"

int main(void){
    const struct CMUnitTest sanitize_test[] = {
        cmocka_unit_test(testSanitizeMetaStringNoChange),
        cmocka_unit_test(testSanitizeMetaStringChangesOutputFormat),
        cmocka_unit_test(testSanitizeMetaStringChangesOutputFormatHasSpaces),
        cmocka_unit_test(testSanitizeMetaStringChangesOutputFormatHasSemicolon),
        cmocka_unit_test(testSanitizeMetaStringBrackets),
        cmocka_unit_test(testSanitizeMetaStringGiveZeroLength),
    };

    return cmocka_run_group_tests(sanitize_test, NULL, NULL);
}
