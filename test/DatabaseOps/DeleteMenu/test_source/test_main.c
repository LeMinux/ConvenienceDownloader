#include "../test_include/database_ops_unit_test.h"

int main(void){
    #ifndef TESTING
        fprintf(stderr, "Need to define the TESTING conditional compiler flag when compiling\n");
        exit(EXIT_FAILURE);
    #endif

    const struct CMUnitTest databas_ops_tests[] = {
        /* read tests */
        cmocka_unit_test(testListAllRoots),
        cmocka_unit_test(testListAllRootsWithPaths),
        cmocka_unit_test(testListRootsForConfig),
        cmocka_unit_test(testListRootAndPathsForConfig),

        /* add tests */
        cmocka_unit_test(testAddEntryCatchesInvalidPath),
        cmocka_unit_test(testAddEntryCatchesInvalidDepth),
        cmocka_unit_test(testAddEntryEnterBothValidInput),
        cmocka_unit_test(testAddEntryWithBlackList),
        cmocka_unit_test(testAddEntryToVideoConfig),
        cmocka_unit_test(testAddEntryToCoverConfig),

        /* update tests */
        cmocka_unit_test_setup_teardown(testUpdateEntryCatchesInvalidIndex, createTestDB, closeDB),
        cmocka_unit_test_setup_teardown(testUpdateEntryCatchesInvalidDepth, createTestDB, closeDB),
        cmocka_unit_test_setup_teardown(testUpdateEntryCatchesInvalidPath, createTestDB, closeDB),
        //cmocka_unit_test(testUpdateEntryDiffPathAndDiffDepth),
        //cmocka_unit_test(testUpdateEntryDiffPathAndSameDepth),
        //cmocka_unit_test(testUpdateEntrySamePathAndDiffDepth),
        //cmocka_unit_test(testUpdateEntrySamePathAndSameDepth),

        /* delete tests */

        /* edit menu tests? */
    };
    return
        cmocka_run_group_tests(read_tests, createTestDB, closeDB) &&
        cmocka_run_group_tests(add_tests, createTestDB, closeDB) &&
        cmocka_run_group_tests(update_tests, NULL, NULL)// &&
}
