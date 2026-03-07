#include "../includes/download_video_test.h"

const char* init_db_path = "../../../../source/initDB.sql";

int main(void){
    #ifndef USE_TESTING_DB
        fprintf(stderr, "Need to define the USE_TESTING_DB conditional compiler flag when compiling\n");
        exit(EXIT_FAILURE);
    #endif

    const struct CMUnitTest download_video_tests[] = {
        cmocka_unit_test(testDownloadVideoNoMetaDataUsesOnPageStats),
        cmocka_unit_test(testDownloadVideoAllMetaData),
        cmocka_unit_test(testDownloadVideoWeirdMetaData),
        cmocka_unit_test(testDownloadVideoPartialMetaDataFillsRestWithPageContent),
    };

    return cmocka_run_group_tests(download_video_tests, createTestDB, closeTestDB);
}
