#include "../includes/download_cover_test.h"

const char* init_db_path = "../../../../source/initDB.sql";

int main(void){
    #ifndef TESTING
        fprintf(stderr, "Need to define the TESTING conditional compiler flag when compiling\n");
        exit(EXIT_FAILURE);
    #endif

    const struct CMUnitTest download_cover_tests[] = {
        cmocka_unit_test_setup_teardown(testDownloadCoverDownloadsCoverArt, createTestDB, closeDB),
    };

    return cmocka_run_group_tests(download_cover_tests, NULL, NULL);
}
