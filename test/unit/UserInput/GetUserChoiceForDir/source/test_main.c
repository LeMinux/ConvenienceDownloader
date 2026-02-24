#include "../includes/get_dir_choice_unit_test.h"

const char* init_db_path = "../../../../source/initDB.sql";

int main(void){
    const struct CMUnitTest dir_sel_group [] = {
        cmocka_unit_test_setup_teardown(getUserDirChoiceSkippingIndexIsSkipping, createTestDBWithRootsAndPaths, closeDB),
        cmocka_unit_test_setup_teardown(getUserDirChoiceRootItselfUsingAlphabeticalSorting, createTestDBWithRootsAndPaths, closeDB),
        cmocka_unit_test_setup_teardown(getUserDirChoiceRootItselfAlreadyAlphabetical, createTestDBWithRootsAndPaths, closeDB),
        cmocka_unit_test_setup_teardown(getUserDirChoicePathUsingAlphabeticalSorting, createTestDBWithRootsAndPaths, closeDB),
        cmocka_unit_test_setup_teardown(getUserDirChoicePathAlreadyAlphabetical, createTestDBWithRootsAndPaths, closeDB),
        cmocka_unit_test_setup_teardown(getUserDirChoiceSkipsAudioIfEmpty, createTestDB, closeDB),
        cmocka_unit_test_setup_teardown(getUserDirChoiceSkipsVideoIfEmpty, createTestDB, closeDB),
        cmocka_unit_test_setup_teardown(getUserDirChoiceSkipsCoverIfEmpty, createTestDB, closeDB),
    };

    return cmocka_run_group_tests(dir_sel_group, NULL, NULL);
}
