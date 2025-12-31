#include "../test_include/database_ops_unit_test.h"

int main(void){
    #ifndef TESTING
        fprintf(stderr, "Need to define the TESTING conditional compiler flag when compiling\n");
        exit(EXIT_FAILURE);
    #endif

    const struct CMUnitTest read_tests[] = {
        cmocka_unit_test_setup_teardown(testListAllRoots, createTestDB, closePerTestDB),
        cmocka_unit_test_setup_teardown(testListAllRootsWithPaths, createTestDB, closePerTestDB),
        cmocka_unit_test_setup_teardown(testListRootsForConfig, createTestDB, closePerTestDB),
        cmocka_unit_test_setup_teardown(testListRootAndPathsForConfig, createTestDB, closePerTestDB),
    };

    const struct CMUnitTest add_tests[] = {
        cmocka_unit_test_setup_teardown(testListAllRoots, createTestDB, closePerTestDB),
    };

    const struct CMUnitTest update_tests[] = {
        cmocka_unit_test_setup_teardown(testListAllRoots, createTestDB, closePerTestDB),
    };

    const struct CMUnitTest del_tests[] = {
        cmocka_unit_test_setup_teardown(testListAllRoots, createTestDB, closePerTestDB),
    };

    return
    (
        cmocka_run_group_tests(read_tests, NULL, NULL) &&
        cmocka_run_group_tests(add_tests, NULL, NULL) &&
        cmocka_run_group_tests(update_tests, NULL, NULL) &&
        cmocka_run_group_tests(del_tests, NULL, NULL)
    );
}
