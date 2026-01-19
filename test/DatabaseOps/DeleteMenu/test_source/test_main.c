#include "../test_include/delete_menu_unit_test.h"

int main(void){
    #ifndef TESTING
        fprintf(stderr, "Need to define the TESTING conditional compiler flag when compiling\n");
        exit(EXIT_FAILURE);
    #endif

    const struct CMUnitTest delete_tests[] = {
        cmocka_unit_test_setup_teardown(testDeleteEntryCatchesInvalidIndex, createTestDBWithRootsAndPaths, closeDB),
        cmocka_unit_test_setup_teardown(testDeleteEntryAudioConfig, createTestDBWithRootsAndPaths, closeDB),
        cmocka_unit_test_setup_teardown(testDeleteEntryVideoConfig, createTestDBWithRootsAndPaths, closeDB),
        cmocka_unit_test_setup_teardown(testDeleteEntryCoverConfig, createTestDBWithRootsAndPaths, closeDB),
        cmocka_unit_test_setup_teardown(testDeleteEntryBlackList, createTestDBWithRootsAndPaths, closeDB),
    };
    return cmocka_run_group_tests(delete_tests, NULL, NULL);
}
