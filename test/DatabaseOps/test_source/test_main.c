#include "../test_include/database_ops_unit_test.h"

int main(void){
    #ifndef TESTING
        fprintf(stderr, "Need to define the TESTING conditional compiler flag when compiling\n");
        exit(EXIT_FAILURE);
    #endif

    const struct CMUnitTest read_tests[] = {
        cmocka_unit_test(testListAllRoots),
        cmocka_unit_test(testListAllRootsWithPaths),
        cmocka_unit_test(testListRootsForConfig),
        cmocka_unit_test(testListRootAndPathsForConfig),
    };

    const struct CMUnitTest add_tests[] = {
        cmocka_unit_test(testAddEntryCatchesInvalidPath),
        cmocka_unit_test(testAddEntryCatchesInvalidDepth),
        cmocka_unit_test(testAddEntryEnterBothValidInput),
        cmocka_unit_test(testAddEntryWithBlackList),
        cmocka_unit_test(testAddEntryToVideoConfig),
        cmocka_unit_test(testAddEntryToCoverConfig),
    };

    const struct CMUnitTest update_tests[] = {
        cmocka_unit_test(testUpdateEntryCatchesInvalidDepth),
        cmocka_unit_test(testUpdateEntryCatchesInvalidPath),
        cmocka_unit_test(testUpdateEntryDiffPathAndDiffDepth),
        cmocka_unit_test(testUpdateEntryDiffPathAndSameDepth),
        cmocka_unit_test(testUpdateEntrySamePathAndDiffDepth),
        cmocka_unit_test(testUpdateEntrySamePathAndSameDepth),
    };

    const struct CMUnitTest del_tests[] = {
        cmocka_unit_test(testListAllRoots),
    };

    const struct CMUnitTest edit_menu_tests[] = {
        cmocka_unit_test(testListAllRoots),
    };

    return
    (
        //cmocka_run_group_tests(read_tests, createTestDB, closePerGroupDB) &&
        //cmocka_run_group_tests(add_tests, createTestDB, closePerGroupDB) &&
        cmocka_run_group_tests(update_tests, createTestDB, closePerGroupDB)// &&
        //cmocka_run_group_tests(del_tests, NULL, NULL)
    );
}
