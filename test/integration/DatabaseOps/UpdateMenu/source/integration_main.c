#include "../includes/update_menu_integration_test.h"

const char* init_db_path = "../../../../source/initDB.sql";

int main(void){
    #ifndef TESTING
        fprintf(stderr, "Need to define the TESTING conditional compiler flag when compiling\n");
        exit(EXIT_FAILURE);
    #endif

    const struct CMUnitTest update_tests[] = {
        cmocka_unit_test_setup_teardown(testUpdateMenuCatchesNoRows, createTestDB, closeDB),
        cmocka_unit_test_setup_teardown(testUpdateMenuCatchesInvalidIndex, createTestDBWithRootsAndPaths, closeDB),
        cmocka_unit_test_setup_teardown(testUpdateMenuCatchesInvalidDepth, createTestDBWithRootsAndPaths, closeDB),
        cmocka_unit_test_setup_teardown(testUpdateMenuCatchesSkippingIndex, createTestDBWithRootsAndPaths, closeDB),
        cmocka_unit_test_setup_teardown(testUpdateMenuCatchesRootIsDeleted, createTestDB, closeDB),

        cmocka_unit_test_setup_teardown(testUpdateMenuSkippingDepth, createTestDB, closeDB),
        cmocka_unit_test_setup_teardown(testUpdateMenuEnterInfInputOnRootDir, createTestDB, closeDB),
        cmocka_unit_test_setup_teardown(testUpdateMenuLargerDepthThanWhatRootHas, createTestDB, closeDB),
        cmocka_unit_test_setup_teardown(testUpdateMenuSmallerDepthThanWhatRootHas, createTestDB, closeDB),
        cmocka_unit_test_setup_teardown(testUpdateMenuZeroDepthOnRoot, createTestDB, closeDB),
        cmocka_unit_test_setup_teardown(testUpdateMenuOneDepthOnRoot, createTestDB, closeDB),
        cmocka_unit_test_setup_teardown(testUpdateMenuMaxDepth, createTestDB, closeDB),

        cmocka_unit_test_setup_teardown(testUpdateMenuToVideoConfig, createTestDB, closeDB),
        cmocka_unit_test_setup_teardown(testUpdateMenuToCoverConfig, createTestDB, closeDB),
        cmocka_unit_test_setup_teardown(testUpdateMenuPathIsInBlackList, createTestDB, closeDB),
    };

    return cmocka_run_group_tests(update_tests, NULL, NULL);
}
