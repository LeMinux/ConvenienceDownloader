#include "../includes/add_menu_integration_test.h"

const char* init_db_path = "../../source/initDB.sql";

int main(void){
    #ifndef TESTING
        fprintf(stderr, "Need to define the TESTING conditional compiler flag when compiling\n");
        exit(EXIT_FAILURE);
    #endif

    const struct CMUnitTest add_tests[] = {
        cmocka_unit_test_setup_teardown(testAddMenuCatchesInvalidPath, createTestDB, closeDB),
        cmocka_unit_test_setup_teardown(testAddMenuCatchesInvalidDepth, createTestDB, closeDB),
        cmocka_unit_test_setup_teardown(testAddMenuCatchesPathInBlackList, createTestDB, closeDB),
        cmocka_unit_test_setup_teardown(testAddMenuCatchesDuplicateNameAndType, createTestDB, closeDB),
        cmocka_unit_test_setup_teardown(testAddMenuEnterInfInputOnRootDir, createTestDB, closeDB),

        cmocka_unit_test_setup_teardown(testAddMenuMaxDepth, createTestDB, closeDB),
        cmocka_unit_test_setup_teardown(testAddMenuLargerDepthThanWhatRootHas, createTestDB, closeDB),
        cmocka_unit_test_setup_teardown(testAddMenuSmallerDepthThanWhatRootHas, createTestDB, closeDB),
        cmocka_unit_test_setup_teardown(testAddMenuZeroDepthOnRoot, createTestDB, closeDB),
        cmocka_unit_test_setup_teardown(testAddMenuOneDepthOnRoot, createTestDB, closeDB),

        cmocka_unit_test_setup_teardown(testAddMenuToBlackList, createTestDB, closeDB),
        cmocka_unit_test_setup_teardown(testAddMenuToVideoConfig, createTestDB, closeDB),
        cmocka_unit_test_setup_teardown(testAddMenuToCoverConfig, createTestDB, closeDB),
        cmocka_unit_test_setup_teardown(testAddMenuDuplicateNameButDiffConfigType, createTestDB, closeDB),
    };

    return cmocka_run_group_tests(add_tests, NULL, NULL);
}
