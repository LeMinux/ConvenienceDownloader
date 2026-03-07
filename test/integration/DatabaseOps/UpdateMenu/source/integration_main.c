#include "../includes/update_menu_integration_test.h"

const char* init_db_path = "../../../../source/initDB.sql";

int main(void){
    #ifndef USE_TESTING_DB
        fprintf(stderr, "Need to define the USE_TESTING_DB conditional compiler flag when compiling\n");
        exit(EXIT_FAILURE);
    #endif

    const struct CMUnitTest update_tests[] = {
        cmocka_unit_test_setup_teardown(testUpdateMenuCatchesNoRows, createTestDB, closeTestDB),
        cmocka_unit_test_setup_teardown(testUpdateMenuCatchesInvalidIndex, createTestDBWithRootsAndPaths, closeTestDB),
        cmocka_unit_test_setup_teardown(testUpdateMenuCatchesInvalidDepth, createTestDBWithRootsAndPaths, closeTestDB),
        cmocka_unit_test_setup_teardown(testUpdateMenuCatchesSkippingIndex, createTestDBWithRootsAndPaths, closeTestDB),
        cmocka_unit_test_setup_teardown(testUpdateMenuCatchesRootIsDeleted, createTestDB, closeTestDB),

        cmocka_unit_test_setup_teardown(testUpdateMenuSkippingDepth, createTestDB, closeTestDB),
        cmocka_unit_test_setup_teardown(testUpdateMenuEnterInfInputOnRootDir, createTestDB, closeTestDB),
        cmocka_unit_test_setup_teardown(testUpdateMenuLargerDepthThanWhatRootHas, createTestDB, closeTestDB),
        cmocka_unit_test_setup_teardown(testUpdateMenuSmallerDepthThanWhatRootHas, createTestDB, closeTestDB),
        cmocka_unit_test_setup_teardown(testUpdateMenuZeroDepthOnRoot, createTestDB, closeTestDB),
        cmocka_unit_test_setup_teardown(testUpdateMenuOneDepthOnRoot, createTestDB, closeTestDB),
        cmocka_unit_test_setup_teardown(testUpdateMenuMaxDepth, createTestDB, closeTestDB),

        cmocka_unit_test_setup_teardown(testUpdateMenuToVideoConfig, createTestDB, closeTestDB),
        cmocka_unit_test_setup_teardown(testUpdateMenuToCoverConfig, createTestDB, closeTestDB),
        cmocka_unit_test_setup_teardown(testUpdateMenuPathIsInBlackList, createTestDB, closeTestDB),
    };

    return cmocka_run_group_tests(update_tests, NULL, NULL);
}
