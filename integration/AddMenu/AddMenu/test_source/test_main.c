#include "../test_include/add_menu_integration_test.h"

int main(void){
    #ifndef TESTING
        fprintf(stderr, "Need to define the TESTING conditional compiler flag when compiling\n");
        exit(EXIT_FAILURE);
    #endif

    const struct CMUnitTest add_tests[] = {
        cmocka_unit_test(testAddEntryCatchesInvalidPath),
        cmocka_unit_test(testAddEntryCatchesInvalidDepth),
        cmocka_unit_test(testAddEntryEnterInfInputOnRootDir),
        /*
        cmocka_unit_test(testAddEntryWithBlackList),
        cmocka_unit_test(testAddEntryToVideoConfig),
        cmocka_unit_test(testAddEntryToCoverConfig),
        cmocka_unit_test(testAddEntryDuplicateNameButDiffConfigType),
        cmocka_unit_test(testAddEntryCatchesDuplicateNameAndType),
        */
    };

    return cmocka_run_group_tests(add_tests, createTestDB, closeDB);
}
