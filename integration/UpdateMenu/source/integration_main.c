#include "../includes/update_menu_integration_test.h"

const char* init_db_path = "../../source/initDB.sql";

int main(void){
    #ifndef TESTING
        fprintf(stderr, "Need to define the TESTING conditional compiler flag when compiling\n");
        exit(EXIT_FAILURE);
    #endif

    const struct CMUnitTest update_tests[] = {
        cmocka_unit_test_setup_teardown(testUpdateMenuCatchesInvalidPath, createTestDBWithRootsAndPaths, closeDB),
        cmocka_unit_test_setup_teardown(testUpdateMenuCatchesInvalidDepth, createTestDBWithRootsAndPaths, closeDB),
        cmocka_unit_test_setup_teardown(testUpdateMenuCatchesPathInBlackList, createTestDBWithRootsAndPaths, closeDB),
        cmocka_unit_test_setup_teardown(testUpdateMenuCatchesDuplicateNameAndType, createTestDBWithRootsAndPaths, closeDB),
        cmocka_unit_test_setup_teardown(testUpdateMenuEnterInfInputOnRootDir, createTestDBWithRootsAndPaths, closeDB),

        cmocka_unit_test_setup_teardown(testUpdateMenuMaxDepth, createTestDBWithRootsAndPaths, closeDB),
        cmocka_unit_test_setup_teardown(testUpdateMenuLargerDepthThanWhatRootHas, createTestDBWithRootsAndPaths, closeDB),
        cmocka_unit_test_setup_teardown(testUpdateMenuSmallerDepthThanWhatRootHas, createTestDBWithRootsAndPaths, closeDB),
        cmocka_unit_test_setup_teardown(testUpdateMenuZeroDepthOnRoot, createTestDBWithRootsAndPaths, closeDB),
        cmocka_unit_test_setup_teardown(testUpdateMenuOneDepthOnRoot, createTestDBWithRootsAndPaths, closeDB),

        cmocka_unit_test_setup_teardown(testUpdateMenuToBlackList, createTestDBWithRootsAndPaths, closeDB),
        cmocka_unit_test_setup_teardown(testUpdateMenuToVideoConfig, createTestDBWithRootsAndPaths, closeDB),
        cmocka_unit_test_setup_teardown(testUpdateMenuToCoverConfig, createTestDBWithRootsAndPaths, closeDB),
        cmocka_unit_test_setup_teardown(testUpdateMenuDuplicateNameButDiffConfigType, createTestDBWithRootsAndPaths, closeDB),
        cmocka_unit_test_setup_teardown(testUpdateMenuPathIsInBlackList, createTestDBWithRootsAndPaths, closeDB),
    };

    return cmocka_run_group_tests(update_tests, NULL, NULL);
}
