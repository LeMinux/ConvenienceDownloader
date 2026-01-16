#include "../test_include/update_menu_unit_test.h"

int main(void){
    #ifndef TESTING
        fprintf(stderr, "Need to define the TESTING conditional compiler flag when compiling\n");
        exit(EXIT_FAILURE);
    #endif

    const struct CMUnitTest update_tests[] = {
        /* update tests */
        cmocka_unit_test_setup_teardown(testUpdateEntryCatchesInvalidIndex, createTestDB, closeDB),
        cmocka_unit_test_setup_teardown(testUpdateEntryCatchesInvalidDepth, createTestDB, closeDB),
        cmocka_unit_test_setup_teardown(testUpdateEntryCatchesInvalidPath, createTestDB, closeDB),
        //cmocka_unit_test(testUpdateEntryDiffPathAndDiffDepth, createTestDB, closeDB),
        //cmocka_unit_test(testUpdateEntryDiffPathAndSameDepth, createTestDB, closeDB),
        //cmocka_unit_test(testUpdateEntrySamePathAndDiffDepth, createTestDB, closeDB),
        //cmocka_unit_test(testUpdateEntrySamePathAndSameDepth, createTestDB, closeDB),

    };
    return cmocka_run_group_tests(update_tests, NULL, NULL);
}
