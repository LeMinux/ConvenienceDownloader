#include "../includes/open_file_integration.h"

int main(void){
    const struct CMUnitTest dir_input_test[] = {
        cmocka_unit_test(testOpenFileOpenRegFile),
        cmocka_unit_test(testOpenFileOpenDir),
        cmocka_unit_test(testOpenFileDevFile),
        cmocka_unit_test(testOpenFileNonExist),

        cmocka_unit_test(testOpenFileLinkToFile),
        cmocka_unit_test(testOpenFileLinkToDir),
        cmocka_unit_test(testOpenFileBrokenLink),

        cmocka_unit_test(testOpenFileAbsolutePathToFile),
        cmocka_unit_test(testOpenFileNoPerms),
    };

    return cmocka_run_group_tests(dir_input_test, NULL, NULL);
}
