#include "../integration_includes/take_directory_input_integration_test.h"

int main(void){
    const struct CMUnitTest dir_input_test[] = {
        cmocka_unit_test(testTakeDirectoryInputAbsolutePathToDir),
        cmocka_unit_test(testTakeDirectoryInputAbsolutePathToFile),

        cmocka_unit_test(testTakeDirectoryInputRelativePathToDir),
        cmocka_unit_test(testTakeDirectoryInputRelativePathToFile),

        cmocka_unit_test(testTakeDirectoryInputTildePathToDir),
        cmocka_unit_test(testTakeDirectoryInputTildePathToFile),

        cmocka_unit_test(testTakeDirectoryInputPathToNonExist),

        cmocka_unit_test(testTakeDirectoryInputLinkToDir),
        cmocka_unit_test(testTakeDirectoryInputLinkToDirWithTrailingSlash),
        cmocka_unit_test(testTakeDirectoryInputLinkToFile),
        cmocka_unit_test(testTakeDirectoryInputPathToBrokenLink),

        cmocka_unit_test(testTakeDirectoryInputNoWritePerms),
        cmocka_unit_test(testTakeDirectoryInputNoSlashAtEndAboslute),
        cmocka_unit_test(testTakeDirectoryInputNoSlashAtEndRelative)
    };

    return cmocka_run_group_tests(dir_input_test, NULL, NULL);
}
