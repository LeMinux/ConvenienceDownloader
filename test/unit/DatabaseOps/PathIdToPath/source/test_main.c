#include "../includes/path_id_to_path_unit_test.h"

const char* init_db_path = "../../../../source/initDB.sql";

int main(void){
    #ifndef USE_TESTING_DB
        fprintf(stderr, "Need to define the USE_TESTING_DB conditional compiler flag when compiling\n");
        exit(EXIT_FAILURE);
    #endif

    const struct CMUnitTest path_id_tests[] = {
        cmocka_unit_test_setup_teardown(testPathIdToPathRootItself, createTestDBWithRootsAndPaths, closeTestDB),
        cmocka_unit_test_setup_teardown(testPathIdToPathAudioPath, createTestDBWithRootsAndPaths, closeTestDB),
        cmocka_unit_test_setup_teardown(testPathIdToPathVideoPath, createTestDBWithRootsAndPaths, closeTestDB),
        cmocka_unit_test_setup_teardown(testPathIdToPathCoverPath, createTestDBWithRootsAndPaths, closeTestDB),
    };
    return cmocka_run_group_tests(path_id_tests, NULL, NULL);
}
