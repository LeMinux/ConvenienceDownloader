#include "../includes/add_menu_integration_test.h"

const char* init_db_path = "../../../../source/initDB.sql";

int main(void){
    #ifndef USE_TESTING_DB
        fprintf(stderr, "Need to define the USE_TESTING_DB conditional compiler flag when compiling\n");
        exit(EXIT_FAILURE);
    #endif

    const struct CMUnitTest add_tests[] = {
        cmocka_unit_test_setup_teardown(testAddMenuCatchesInvalidPath, createTestDB, closeTestDB),
        cmocka_unit_test_setup_teardown(testAddMenuCatchesInvalidDepth, createTestDB, closeTestDB),
        cmocka_unit_test_setup_teardown(testAddMenuCatchesPathInBlackList, createTestDB, closeTestDB),
        cmocka_unit_test_setup_teardown(testAddMenuCatchesDuplicateNameAndType, createTestDB, closeTestDB),
        cmocka_unit_test_setup_teardown(testAddMenuCatchesSkippingPath, createTestDB, closeTestDB),
        cmocka_unit_test_setup_teardown(testAddMenuCatchesSkippingDepth, createTestDB, closeTestDB),

        cmocka_unit_test_setup_teardown(testAddMenuEnterInfInputOnRootDir, createTestDB, closeTestDB),
        cmocka_unit_test_setup_teardown(testAddMenuMaxDepth, createTestDB, closeTestDB),
        cmocka_unit_test_setup_teardown(testAddMenuLargerDepthThanWhatRootHas, createTestDB, closeTestDB),
        cmocka_unit_test_setup_teardown(testAddMenuSmallerDepthThanWhatRootHas, createTestDB, closeTestDB),
        cmocka_unit_test_setup_teardown(testAddMenuZeroDepthOnRoot, createTestDB, closeTestDB),
        cmocka_unit_test_setup_teardown(testAddMenuOneDepthOnRoot, createTestDB, closeTestDB),

        cmocka_unit_test_setup_teardown(testAddMenuToBlackList, createTestDB, closeTestDB),
        cmocka_unit_test_setup_teardown(testAddMenuToVideoConfig, createTestDB, closeTestDB),
        cmocka_unit_test_setup_teardown(testAddMenuToCoverConfig, createTestDB, closeTestDB),
        cmocka_unit_test_setup_teardown(testAddMenuDuplicateNameButDiffConfigType, createTestDB, closeTestDB),
        cmocka_unit_test_setup_teardown(testAddMenuPathIsInBlackList, createTestDB, closeTestDB),
    };

    return cmocka_run_group_tests(add_tests, NULL, NULL);
}
