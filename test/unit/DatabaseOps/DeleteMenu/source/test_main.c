#include "../includes/delete_menu_unit_test.h"

const char* init_db_path = "../../../../source/initDB.sql";

int main(void){
    #ifndef USE_TESTING_DB
        fprintf(stderr, "Need to define the USE_TESTING_DB conditional compiler flag when compiling\n");
        exit(EXIT_FAILURE);
    #endif

    const struct CMUnitTest delete_tests[] = {
        cmocka_unit_test_setup_teardown(testDeleteEntryCatchesInvalidIndex, createTestDBWithRootsAndPaths, closeTestDB),
        cmocka_unit_test_setup_teardown(testDeleteMenuCatchesNoRows, createTestDB, closeTestDB),
        cmocka_unit_test_setup_teardown(testDeleteMenuCatchesSkipping, createTestDBWithRootsAndPaths, closeTestDB),
        cmocka_unit_test_setup_teardown(testDeleteEntryAudioConfig, createTestDBWithRootsAndPaths, closeTestDB),
        cmocka_unit_test_setup_teardown(testDeleteEntryVideoConfig, createTestDBWithRootsAndPaths, closeTestDB),
        cmocka_unit_test_setup_teardown(testDeleteEntryCoverConfig, createTestDBWithRootsAndPaths, closeTestDB),
        cmocka_unit_test_setup_teardown(testDeleteEntryBlackList, createTestDBWithRootsAndPaths, closeTestDB),
    };
    return cmocka_run_group_tests(delete_tests, NULL, NULL);
}
