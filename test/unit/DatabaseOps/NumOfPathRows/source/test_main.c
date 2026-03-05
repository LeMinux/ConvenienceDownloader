#include "../includes/path_count_unit_test.h"

const char* init_db_path = "../../../../source/initDB.sql";

int main(void){
    #ifndef TESTING
        fprintf(stderr, "Need to define the TESTING conditional compiler flag when compiling\n");
        exit(EXIT_FAILURE);
    #endif

    const struct CMUnitTest count_tests [] = {
        cmocka_unit_test_setup_teardown(testGetNumOfPathRowsGivesZero, createTestDB, closeDB),
        cmocka_unit_test_setup_teardown(testGetNumOfPathRowsCorrectCount, createTestDBWithRootsAndPaths, closeDB),
    };
    return cmocka_run_group_tests(count_tests, NULL, NULL);
}
