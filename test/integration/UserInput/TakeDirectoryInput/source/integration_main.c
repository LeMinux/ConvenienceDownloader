#include "../includes/take_directory_input_integration_test.h"

int main(void){
    const struct CMUnitTest dir_input_test[] = {
        cmocka_unit_test(testTakeDirectoryInputEmptyInputGivesEmptyResult),
        cmocka_unit_test(testTakeDirectoryInputAbsolutePathToDirWithTrailingSlash),
        cmocka_unit_test(testTakeDirectoryInputAbsolutePathToDirWithoutTrailingSlash),

        cmocka_unit_test(testTakeDirectoryInputRelativePathToDirWithTrailingSlash),
        cmocka_unit_test(testTakeDirectoryInputRelativePathToDirWithoutTrailingSlash),

        cmocka_unit_test(testTakeDirectoryInputAbsolutePathToNonDir),
        cmocka_unit_test(testTakeDirectoryInputRelativePathToNonDir),

        cmocka_unit_test(testTakeDirectoryInputTildePathToDir),
        cmocka_unit_test(testTakeDirectoryInputTildePathToFile),

        cmocka_unit_test(testTakeDirectoryInputPathToNonExist),

        cmocka_unit_test(testTakeDirectoryInputLinkToDir),
        cmocka_unit_test(testTakeDirectoryInputLinkToDirWithTrailingSlash),
        cmocka_unit_test(testTakeDirectoryInputLinkToFile),
        cmocka_unit_test(testTakeDirectoryInputPathToBrokenLink),

        cmocka_unit_test(testTakeDirectoryInputNoPermsRelativePath),
        cmocka_unit_test(testTakeDirectoryInputNoWritePerms),

        cmocka_unit_test(testTakeDirectoryInputPathHasSpaces),
    };

    return cmocka_run_group_tests(dir_input_test, NULL, NULL);
}
