#include "../includes/refresh_db_integration_test.h"

const char* init_db_path = "../../../../source/initDB.sql";

int main(void){
    #ifndef USE_TESTING_DB
        fprintf(stderr, "Need to define the USE_TESTING_DB conditional compiler flag when compiling\n");
        exit(EXIT_FAILURE);
    #endif

    const struct CMUnitTest refresh_tests[] = {
        cmocka_unit_test_setup_teardown(testRefreshDatabase, createTestDB, closeTestDB),
    };

    return cmocka_run_group_tests(refresh_tests, NULL, NULL);
}
