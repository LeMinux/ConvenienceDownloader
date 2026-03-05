#include "../includes/repeat_unit_test.h"

void testAsktoRepeatSingleValidLowerCase(void** state){
    (void) state;
    const InputWithExp_t input_exp [] = {
        {"y", REPEAT},
        {"n", NO_REPEAT},
    };
    int test_count = sizeof(input_exp)/sizeof(input_exp[0]);

    for(int i = 0; i < test_count; ++i){
        printf("\nTesting: %s\n", input_exp[i].input);
        will_return(__wrap_boundedInput, input_exp[i].input);
        int act_result = askToRepeat();
        assert_int_equal(act_result, input_exp[i].exp_result);
    }
}

void testAsktoRepeatSingleValidUpperCase(void** state){
    (void) state;
    const InputWithExp_t input_exp [] = {
        {"Y", REPEAT},
        {"N", NO_REPEAT},
    };
    int test_count = sizeof(input_exp)/sizeof(input_exp[0]);

    for(int i = 0; i < test_count; ++i){
        printf("\nTesting: %s\n", input_exp[i].input);
        will_return(__wrap_boundedInput, input_exp[i].input);
        int act_result = askToRepeat();
        assert_int_equal(act_result, input_exp[i].exp_result);
    }
}

void testAsktoRepeatAllYesForms(void** state){
    (void) state;
    const InputWithExp_t input_exp [] = {
        {"yes", REPEAT},
        {"YES", REPEAT},
        {"yEs", REPEAT},
    };
    int test_count = sizeof(input_exp)/sizeof(input_exp[0]);

    for(int i = 0; i < test_count; ++i){
        printf("\nTesting: %s\n", input_exp[i].input);
        will_return(__wrap_boundedInput, input_exp[i].input);
        int act_result = askToRepeat();
        assert_int_equal(act_result, input_exp[i].exp_result);
    }
}

void testAsktoRepeatAllNoForms(void** state){
    (void) state;
    const InputWithExp_t input_exp [] = {
        {"no", NO_REPEAT},
        {"NO", NO_REPEAT},
        {"nO", NO_REPEAT},
    };
    int test_count = sizeof(input_exp)/sizeof(input_exp[0]);

    for(int i = 0; i < test_count; ++i){
        printf("\nTesting: %s\n", input_exp[i].input);
        will_return(__wrap_boundedInput, input_exp[i].input);
        int act_result = askToRepeat();
        assert_int_equal(act_result, input_exp[i].exp_result);
    }
}

void testAsktoRepeatBeingsWithY(void** state){
    (void) state;
    will_return(__wrap_boundedInput, "yip");
    int act_result = askToRepeat();
    assert_int_equal(act_result, ASK_AGAIN);
}

void testAsktoRepeatBeingsWithN(void** state){
    (void) state;
    will_return(__wrap_boundedInput, "noe");
    int act_result = askToRepeat();
    assert_int_equal(act_result, ASK_AGAIN);
}


void testConfigToEditInvalid(void** state){
    (void) state;
    will_return(__wrap_boundedInput, "ugh");
    int act_result = askToRepeat();
    assert_int_equal(act_result, ASK_AGAIN);
}

void testConfigToEditEmptyInput(void** state){
    (void) state;
    will_return(__wrap_boundedInput, "");
    int act_result = askToRepeat();
    assert_int_equal(act_result, ASK_AGAIN);
}
