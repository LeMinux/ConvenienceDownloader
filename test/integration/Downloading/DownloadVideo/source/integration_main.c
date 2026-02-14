#include "../includes/download_video_test.h"

const char* init_db_path = "../../../../source/initDB.sql";

int main(void){
    #ifndef TESTING
        fprintf(stderr, "Need to define the TESTING conditional compiler flag when compiling\n");
        exit(EXIT_FAILURE);
    #endif

    const struct CMUnitTest download_audio_tests[] = {
        cmocka_unit_test_setup_teardown(testDownloadVideoNoMetaData, createTestDB, closeDB),
        cmocka_unit_test_setup_teardown(testDownloadVideoAllMetaData, createTestDB, closeDB),
        cmocka_unit_test_setup_teardown(testDownloadVideoWeirdMetaData, createTestDB, closeDB),
    };

    return cmocka_run_group_tests(download_audio_tests, NULL, NULL);
}
