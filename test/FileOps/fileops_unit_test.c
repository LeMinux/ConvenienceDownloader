#include "fileops_unit_test.h"

int __wrap_file_open(){

    return 0;
}

int main(void){
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(testOpenNormalFile),
        cmocka_unit_test(testOpenNonExistFile),
        cmocka_unit_test(testOpenFileButIsDirectory),
        cmocka_unit_test(testOpenLinkFileToValid),
        cmocka_unit_test(testOpenBrokenLink),
        cmocka_unit_test(testLinkToDevFile),
        cmocka_unit_test(testValidWriteToAudioConf),
        cmocka_unit_test(testValidWriteToVideoConf),
        cmocka_unit_test(testValidWriteToCoverConf),
        cmocka_unit_test(testValidWriteToCoverConf)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
