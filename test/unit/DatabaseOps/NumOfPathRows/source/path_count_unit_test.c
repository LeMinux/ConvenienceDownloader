#include "../includes/path_count_unit_test.h"

void testGetNumOfPathRowsGivesZero(void** state){
    (void)state;
    enum CONFIG type = AUDIO_CONFIG;
    int exp_count = 0;

    int act_count = getNumOfPathRowsForConfig(type);

    assert_int_equal(act_count, exp_count);
}

void testGetNumOfPathRowsCorrectCount(void** state){
    (void)state;
    enum CONFIG type = AUDIO_CONFIG;
    int exp_count = PATHS_PER_CONFIG;

    int act_count = getNumOfPathRowsForConfig(type);

    assert_int_equal(act_count, exp_count);
}
