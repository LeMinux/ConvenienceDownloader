#include "../includes/config_input_unit_test.h"

void testConfigToEditShortHandOptionsLowerCase(void** state){
    (void) state;
    const InputWithExp_t input_exp [] = {
        {"a", AUDIO_CONFIG},
        {"v", VIDEO_CONFIG},
        {"c", COVER_CONFIG},
        {"b", BLACK_CONFIG},
    };
    int test_count = sizeof(input_exp)/sizeof(input_exp[0]);

    for(int i = 0; i < test_count; ++i){
        printf("Testing: %s\n", input_exp[i].input);
        int act_result = getConfigToEdit(input_exp[i].input);
        assert_int_equal(act_result, input_exp[i].exp_result);
    }
}

void testConfigToEditShortHandOptionsUpperCase(void** state){
    (void) state;
    const InputWithExp_t input_exp [] = {
        {"A", AUDIO_CONFIG},
        {"V", VIDEO_CONFIG},
        {"C", COVER_CONFIG},
        {"B", BLACK_CONFIG},
    };
    int test_count = sizeof(input_exp)/sizeof(input_exp[0]);

    for(int i = 0; i < test_count; ++i){
        printf("Testing: %s\n", input_exp[i].input);
        int act_result = getConfigToEdit(input_exp[i].input);
        assert_int_equal(act_result, input_exp[i].exp_result);
    }
}

void testConfigToEditInvalidShortHandOptions(void** state){
    (void) state;
    const char* invalid_short_hand = "defghijklmnopqrstuwxyz";

    for(int i = 0; i < (int)sizeof(invalid_short_hand) - 1; ++i){
        char input [2];
        input[0] = invalid_short_hand[i];
        input[1] = '\0';
        int act_result = getConfigToEdit(input);
        assert_int_equal(act_result, -1);
    }
}


void testConfigToEditLongHandOptionsLowerCase(void** state){
    (void) state;
    const InputWithExp_t input_exp [] = {
        {"audio", AUDIO_CONFIG},
        {"video", VIDEO_CONFIG},
        {"cover", COVER_CONFIG},
        {"black", BLACK_CONFIG},
    };
    int test_count = sizeof(input_exp)/sizeof(input_exp[0]);

    for(int i = 0; i < test_count; ++i){
        printf("Testing: %s\n", input_exp[i].input);
        int act_result = getConfigToEdit(input_exp[i].input);
        assert_int_equal(act_result, input_exp[i].exp_result);
    }
}

void testConfigToEditLongHandOptionsUpperCase(void** state){
    (void) state;
    const InputWithExp_t input_exp [] = {
        {"AUDIO", AUDIO_CONFIG},
        {"VIDEO", VIDEO_CONFIG},
        {"COVER", COVER_CONFIG},
        {"BLACK", BLACK_CONFIG},
    };
    int test_count = sizeof(input_exp)/sizeof(input_exp[0]);

    for(int i = 0; i < test_count; ++i){
        printf("Testing: %s\n", input_exp[i].input);
        int act_result = getConfigToEdit(input_exp[i].input);
        assert_int_equal(act_result, input_exp[i].exp_result);
    }
}

void testConfigToEditLongHandOptionsMixedCase(void** state){
    (void) state;
    const InputWithExp_t input_exp [] = {
        {"AuDiO", AUDIO_CONFIG},
        {"ViDeO", VIDEO_CONFIG},
        {"CoVeR", COVER_CONFIG},
        {"BlAcK", BLACK_CONFIG},
    };
    int test_count = sizeof(input_exp)/sizeof(input_exp[0]);

    for(int i = 0; i < test_count; ++i){
        printf("Testing: %s\n", input_exp[i].input);
        int act_result = getConfigToEdit(input_exp[i].input);
        assert_int_equal(act_result, input_exp[i].exp_result);
    }
}

void testConfigToEditInvalidLongHandOptions(void** state){
    (void) state;
    const InputWithExp_t input_exp [] = {
        {"SomeReallyLongInput", -1},
        {"audiooooooo", -1},
        {"smol", -1},
    };
    int test_count = sizeof(input_exp)/sizeof(input_exp[0]);

    for(int i = 0; i < test_count; ++i){
        printf("Testing: %s\n", input_exp[i].input);
        int act_result = getConfigToEdit(input_exp[i].input);
        assert_int_equal(act_result, -1);
    }
}

void testConfigToEditEmptyInput(void** state){
    (void) state;
    int act_result = getConfigToEdit("");
    assert_int_equal(act_result, -1);
}
