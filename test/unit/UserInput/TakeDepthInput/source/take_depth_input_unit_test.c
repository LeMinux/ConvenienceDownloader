#include "../includes/take_depth_input_unit_test.h"

void testTakeDepthInputSendSkippingOnZeroLength(void** state){
    (void)state;
    char input [] = "";
    int expect_ret = SKIPPING;

    will_return(__wrap_boundedInput, input);

    int actual_ret = takeDepthInput();

    assert_int_equal(actual_ret, expect_ret);
}

void testTakeDepthInputNegativeNumber(void** state){
    (void)state;
    char input [] = "-1";
    int expect_ret = INVALID;

    will_return(__wrap_boundedInput, input);

    int actual_ret = takeDepthInput();

    assert_int_equal(actual_ret, expect_ret);
    (void)state;
}

void testTakeDepthInputReallyLargeNumber(void** state){
    (void)state;
    char input [] = "99999999999999";
    int expect_ret = INVALID;

    will_return(__wrap_boundedInput, input);

    int actual_ret = takeDepthInput();

    assert_int_equal(actual_ret, expect_ret);
}

void testTakeDepthInputReallySmallNumber(void** state){
    (void)state;
    char input [] = "-9999999999999";
    int expect_ret = INVALID;

    will_return(__wrap_boundedInput, input);

    int actual_ret = takeDepthInput();

    assert_int_equal(actual_ret, expect_ret);
}

void testTakeDepthInputRandomLetters(void** state){
    (void)state;
    char input [] = "eaithaiha";
    int expect_ret = INVALID;

    will_return(__wrap_boundedInput, input);

    int actual_ret = takeDepthInput();

    assert_int_equal(actual_ret, expect_ret);
}

void testTakeDepthInputNotAFullNumberLettersBefore(void** state){
    (void)state;
    char input [] = "aa7";
    int expect_ret = INVALID;

    will_return(__wrap_boundedInput, input);

    int actual_ret = takeDepthInput();

    assert_int_equal(actual_ret, expect_ret);
}

void testTakeDepthInputNotAFullNumberLettersAfter(void** state){
    (void)state;
    char input [] = "7aa";
    int expect_ret = INVALID;

    will_return(__wrap_boundedInput, input);

    int actual_ret = takeDepthInput();

    assert_int_equal(actual_ret, expect_ret);
}

void testTakeDepthInputZero(void** state){
    (void)state;
    char input [] = "0";
    int expect_ret = 0;

    will_return(__wrap_boundedInput, input);

    int actual_ret = takeDepthInput();

    assert_int_equal(actual_ret, expect_ret);
}

void testTakeDepthInputInBoundNumber(void** state){
    (void)state;
    char input [] = "27";
    int expect_ret = 27;

    will_return(__wrap_boundedInput, input);

    int actual_ret = takeDepthInput();

    assert_int_equal(actual_ret, expect_ret);
}

void testTakeDepthInputWithCommas(void** state){
    (void)state;
    char input [] = "1,000";
    int expect_ret = 1000;

    will_return(__wrap_boundedInput, input);

    int actual_ret = takeDepthInput();

    assert_int_equal(actual_ret, expect_ret);
}

void testTakeDepthInputWithSpaces(void** state){
    (void)state;
    char input [] = "1 000";
    int expect_ret = 1000;

    will_return(__wrap_boundedInput, input);

    int actual_ret = takeDepthInput();

    assert_int_equal(actual_ret, expect_ret);
}

void testTakeDepthInputInfInput(void** state){
    (void)state;
    char input [] = INF_STRING;
    int expect_ret = INF_DEPTH;

    will_return(__wrap_boundedInput, input);

    int actual_ret = takeDepthInput();

    assert_int_equal(actual_ret, expect_ret);
}
