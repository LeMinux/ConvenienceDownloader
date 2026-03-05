#include "../includes/get_dir_choice_unit_test.h"

void getUserDirChoiceSkippingIndexIsSkipping(void** state){
    (void)state;
    int selection = SKIPPING;
    int expect_ret = SKIPPING;
    enum CONFIG type = AUDIO_CONFIG;

    expect_function_calls(__wrap_takeIndexInput, 1);
    will_return(__wrap_takeIndexInput, selection);

    int actual_ret = getUserChoiceForDir(type);
    assert_int_equal(actual_ret, expect_ret);
}


void getUserDirChoiceRootItselfUsingAlphabeticalSorting(void** state){
    (void)state;
    int selection = JAZZ_PATH_ITSELF_SEL;
    int expect_ret = JAZZ_PATH_ITSELF_ID;
    enum CONFIG type = AUDIO_CONFIG;

    expect_function_calls(__wrap_takeIndexInput, 1);
    will_return(__wrap_takeIndexInput, selection);

    int actual_ret = getUserChoiceForDir(type);

    assert_int_equal(actual_ret, expect_ret);
}

void getUserDirChoiceRootItselfAlreadyAlphabetical(void** state){
    (void)state;
    int selection = KEEPS_PATH_ITSELF_SEL;
    int expect_ret = KEEPS_PATH_ITSELF_ID;
    enum CONFIG type = VIDEO_CONFIG;

    expect_function_calls(__wrap_takeIndexInput, 1);
    will_return(__wrap_takeIndexInput, selection);

    int actual_ret = getUserChoiceForDir(type);

    assert_int_equal(actual_ret, expect_ret);
}

void getUserDirChoicePathUsingAlphabeticalSorting(void** state){
    (void)state;
    int selection = KEEPS_PATH_YOUTUBE_SEL;
    int expect_ret = KEEPS_PATH_YOUTUBE_ID;
    enum CONFIG type = VIDEO_CONFIG;


    expect_function_calls(__wrap_takeIndexInput, 1);
    will_return(__wrap_takeIndexInput, selection);

    int actual_ret = getUserChoiceForDir(type);

    assert_int_equal(actual_ret, expect_ret);
}

void getUserDirChoicePathAlreadyAlphabetical(void** state){
    (void)state;
    int selection = ARTS_PATH_SOME_SEL;
    int expect_ret = ARTS_PATH_SOME_ID;
    enum CONFIG type = COVER_CONFIG;

    expect_function_calls(__wrap_takeIndexInput, 1);
    will_return(__wrap_takeIndexInput, selection);

    int actual_ret = getUserChoiceForDir(type);

    assert_int_equal(actual_ret, expect_ret);
}

void getUserDirChoiceSkipsAudioIfEmpty(void** state){
    (void)state;
    int expect_ret = SKIPPING;
    enum CONFIG type = AUDIO_CONFIG;

    expect_function_calls(__wrap_takeIndexInput, 0);
    int actual_ret = getUserChoiceForDir(type);

    assert_int_equal(actual_ret, expect_ret);
}

void getUserDirChoiceSkipsVideoIfEmpty(void** state){
    (void)state;
    int expect_ret = SKIPPING;
    enum CONFIG type = VIDEO_CONFIG;

    expect_function_calls(__wrap_takeIndexInput, 0);
    int actual_ret = getUserChoiceForDir(type);

    assert_int_equal(actual_ret, expect_ret);
}

void getUserDirChoiceSkipsCoverIfEmpty(void** state){
    (void)state;
    int expect_ret = SKIPPING;
    enum CONFIG type = COVER_CONFIG;

    expect_function_calls(__wrap_takeIndexInput, 0);
    int actual_ret = getUserChoiceForDir(type);

    assert_int_equal(actual_ret, expect_ret);
}
