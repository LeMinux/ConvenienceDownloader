#include "../includes/path_id_to_path_unit_test.h"

void testPathIdToPathRootItself(void** state){
    (void) state;
    char act_result [PATH_MAX] = "";
    const char exp_result [] = ROCK_ROOT"/";
    int path_id = ROCK_PATH_ITSELF_ID;

    int act_ret = pathIDToPath(path_id, act_result);

    assert_true(act_ret == NO_ERROR);
    assert_string_equal(act_result, exp_result);
}

void testPathIdToPathAudioPath(void** state){
    (void) state;
    char act_result [PATH_MAX] = "";
    const char exp_result [] = JAZZ_ROOT JAZZ_PATH_1;
    int path_id = JAZZ_PATH_BBVD_ID;

    int act_ret = pathIDToPath(path_id, act_result);

    assert_true(act_ret == NO_ERROR);
    assert_string_equal(act_result, exp_result);
}

void testPathIdToPathVideoPath(void** state){
    (void) state;
    char act_result [PATH_MAX] = "";
    const char exp_result [] = KEEPS_ROOT KEEPS_PATH_1;
    int path_id = KEEPS_PATH_YOUTUBE_ID;

    int act_ret = pathIDToPath(path_id, act_result);

    assert_true(act_ret == NO_ERROR);
    assert_string_equal(act_result, exp_result);
}

void testPathIdToPathCoverPath(void** state){
    (void) state;
    char act_result [PATH_MAX] = "";
    const char exp_result [] = CVS_ROOT CVS_PATH_2;
    int path_id = CVS_PATH_ALBUMN_ID;

    int act_ret = pathIDToPath(path_id, act_result);

    assert_true(act_ret == NO_ERROR);
    assert_string_equal(act_result, exp_result);
}
