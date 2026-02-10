#include "../includes/path_id_to_path_unit_test.h"

const char* init_db_path = "../../../../source/initDB.sql";

int main(void){
    #ifndef TESTING
        fprintf(stderr, "Need to define the TESTING conditional compiler flag when compiling\n");
        exit(EXIT_FAILURE);
    #endif

    const struct CMUnitTest path_id_tests[] = {
        cmocka_unit_test_setup_teardown(testPathIdToPathRootItself, createTestDBWithRootsAndPaths, closeDB),
        cmocka_unit_test_setup_teardown(testPathIdToPathAudioPath, createTestDBWithRootsAndPaths, closeDB),
        cmocka_unit_test_setup_teardown(testPathIdToPathVideoPath, createTestDBWithRootsAndPaths, closeDB),
        cmocka_unit_test_setup_teardown(testPathIdToPathCoverPath, createTestDBWithRootsAndPaths, closeDB),
    };
    return cmocka_run_group_tests(path_id_tests, NULL, NULL);
}
