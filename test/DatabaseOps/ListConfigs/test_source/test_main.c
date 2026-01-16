#include "../test_include/read_configs_unit_test.h"

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

    return cmocka_run_group_tests(read_tests, createTestDB, closeDB);
}
