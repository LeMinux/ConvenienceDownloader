#include "../includes/download_audio_test.h"

const char* init_db_path = "../../../../source/initDB.sql";

int main(void){
    #ifndef USE_TESTING_DB
        fprintf(stderr, "Need to define the USE_TESTING_DB conditional compiler flag when compiling\n");
        exit(EXIT_FAILURE);
    #endif

    const struct CMUnitTest download_audio_tests[] = {
        cmocka_unit_test(testDownloadAudioNoMetaDataUsesOnPageStats),
        cmocka_unit_test(testDownloadAudioAllMetaData),
        cmocka_unit_test(testDownloadAudioEmbedsCoverArt),
        cmocka_unit_test(testDownloadAudioEmbedsNoCoverArt),
        cmocka_unit_test(testDownloadAudioWeirdMetaData),
        cmocka_unit_test(testDownloadAudioEmbedsGivenCoverArt),
        cmocka_unit_test(testDownloadAudioEmbedsGivenCoverArtAndAllMetaData),
        cmocka_unit_test(testDownloadAudioPartialMetaDataFillsRestFromPage),
    };

    return cmocka_run_group_tests(download_audio_tests, createTestDB, closeTestDB);
}
