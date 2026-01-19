#include "../test_include/update_menu_unit_test.h"

//Note that this testing only really tests if does update the roots
//However, the paths associated with the root are still there
//Implementation for this hasn't been done yet
int main(void){
    #ifndef TESTING
        fprintf(stderr, "Need to define the TESTING conditional compiler flag when compiling\n");
        exit(EXIT_FAILURE);
    #endif

    const struct CMUnitTest update_tests[] = {
        /* update tests */
        cmocka_unit_test_setup_teardown(testUpdateEntryCatchesInvalidIndex, createTestDBWithRoots, closeDB),
        cmocka_unit_test_setup_teardown(testUpdateEntryCatchesInvalidDepth, createTestDBWithRoots, closeDB),
        cmocka_unit_test_setup_teardown(testUpdateEntryCatchesInvalidPath, createTestDBWithRoots, closeDB),

        cmocka_unit_test_setup_teardown(testUpdateEntryBlackList, createTestDBWithRoots, closeDB),
        cmocka_unit_test_setup_teardown(testUpdateEntryCoverConfig, createTestDBWithRoots, closeDB),
        cmocka_unit_test_setup_teardown(testUpdateEntryVideoConfig, createTestDBWithRoots, closeDB),

        cmocka_unit_test_setup_teardown(testUpdateEntryDiffPathAndDiffDepth, createTestDBWithRoots, closeDB),
        cmocka_unit_test_setup_teardown(testUpdateEntryDiffPathAndSameDepth, createTestDBWithRoots, closeDB),
        cmocka_unit_test_setup_teardown(testUpdateEntrySamePathAndDiffDepth, createTestDBWithRoots, closeDB),
        cmocka_unit_test_setup_teardown(testUpdateEntrySamePathAndSameDepth, createTestDBWithRoots, closeDB),

        cmocka_unit_test_setup_teardown(testUpdateEntryAddingExistingToBlackList, createTestDBWithRoots, closeDB),
    };

    return cmocka_run_group_tests(update_tests, NULL, NULL);
}
