#include "../includes/get_dir_choice_unit_test.h"

const char* init_db_path = "../../../../source/initDB.sql";

int main(void){
    const struct CMUnitTest dir_sel_group [] = {
        cmocka_unit_test_setup_teardown(getUserDirChoiceSkippingIndexIsSkipping, createTestDBWithRootsAndPaths, closeTestDB),
        cmocka_unit_test_setup_teardown(getUserDirChoiceRootItselfUsingAlphabeticalSorting, createTestDBWithRootsAndPaths, closeTestDB),
        cmocka_unit_test_setup_teardown(getUserDirChoiceRootItselfAlreadyAlphabetical, createTestDBWithRootsAndPaths, closeTestDB),
        cmocka_unit_test_setup_teardown(getUserDirChoicePathUsingAlphabeticalSorting, createTestDBWithRootsAndPaths, closeTestDB),
        cmocka_unit_test_setup_teardown(getUserDirChoicePathAlreadyAlphabetical, createTestDBWithRootsAndPaths, closeTestDB),
        cmocka_unit_test_setup_teardown(getUserDirChoiceSkipsAudioIfEmpty, createTestDB, closeTestDB),
        cmocka_unit_test_setup_teardown(getUserDirChoiceSkipsVideoIfEmpty, createTestDB, closeTestDB),
        cmocka_unit_test_setup_teardown(getUserDirChoiceSkipsCoverIfEmpty, createTestDB, closeTestDB),
    };

    return cmocka_run_group_tests(dir_sel_group, NULL, NULL);
}
