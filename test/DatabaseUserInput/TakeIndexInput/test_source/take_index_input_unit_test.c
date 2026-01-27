#include "../test_includes/take_index_input_unit_test.h"

void testTakeIndexInputSendInvalidOnZeroLength(void** state){
    (void)state;
    char input [] = "";
    int expect_ret = INVALID;

    will_return(__wrap_boundedInput, input);

    int actual_ret = takeIndexInput(MAX_INDEX);

    assert_int_equal(actual_ret, expect_ret);
}

void testTakeIndexInputNegativeNumber(void** state){
    (void)state;
    char input [] = "-1";
    int expect_ret = INVALID;

    will_return(__wrap_boundedInput, input);

    int actual_ret = takeIndexInput(MAX_INDEX);

    assert_int_equal(actual_ret, expect_ret);
    (void)state;
}

void testTakeIndexInputReallyLargeNumber(void** state){
    (void)state;
    char input [] = "99999999999999";
    int expect_ret = INVALID;

    will_return(__wrap_boundedInput, input);

    int actual_ret = takeIndexInput(MAX_INDEX);

    assert_int_equal(actual_ret, expect_ret);
}

void testTakeIndexInputReallySmallNumber(void** state){
    (void)state;
    char input [] = "-9999999999999";
    int expect_ret = INVALID;

    will_return(__wrap_boundedInput, input);

    int actual_ret = takeIndexInput(MAX_INDEX);

    assert_int_equal(actual_ret, expect_ret);
}

void testTakeIndexInputRandomLetters(void** state){
    (void)state;
    char input [] = "eaithaiha";
    int expect_ret = INVALID;

    will_return(__wrap_boundedInput, input);

    int actual_ret = takeIndexInput(MAX_INDEX);

    assert_int_equal(actual_ret, expect_ret);
}

void testTakeIndexInputNotAFullNumberLettersBefore(void** state){
    (void)state;
    char input [] = "aa7";
    int expect_ret = INVALID;

    will_return(__wrap_boundedInput, input);

    int actual_ret = takeIndexInput(MAX_INDEX);

    assert_int_equal(actual_ret, expect_ret);
}

void testTakeIndexInputNotAFullNumberLettersAfter(void** state){
    (void)state;
    char input [] = "7aa";
    int expect_ret = INVALID;

    will_return(__wrap_boundedInput, input);

    int actual_ret = takeIndexInput(MAX_INDEX);

    assert_int_equal(actual_ret, expect_ret);
}

void testTakeIndexInputZero(void** state){
    (void)state;
    char input [] = "0";
    int expect_ret = INVALID;

    will_return(__wrap_boundedInput, input);

    int actual_ret = takeIndexInput(MAX_INDEX);

    assert_int_equal(actual_ret, expect_ret);
}

void testTakeIndexInputInBoundNumber(void** state){
    (void)state;
    char input [] = "20";
    int expect_ret = 20;

    will_return(__wrap_boundedInput, input);

    int actual_ret = takeIndexInput(MAX_INDEX);

    assert_int_equal(actual_ret, expect_ret);
}


void testTakeIndexInputBeyondMax(void** state){
    (void)state;
    char input [10] = "";
    snprintf(input, sizeof(input), "%d", MAX_INDEX + 10);
    int expect_ret = INVALID;

    will_return(__wrap_boundedInput, input);

    int actual_ret = takeIndexInput(MAX_INDEX);

    assert_int_equal(actual_ret, expect_ret);
}

void testTakeIndexInputWithCommas(void** state){
    (void)state;
    char input [] = "1,000";
    int expect_ret = 1000;

    will_return(__wrap_boundedInput, input);

    int actual_ret = takeIndexInput(MAX_INDEX);

    assert_int_equal(actual_ret, expect_ret);
}

void testTakeIndexInputWithSpaces(void** state){
    (void)state;
    char input [] = "1 000";
    int expect_ret = 1000;

    will_return(__wrap_boundedInput, input);

    int actual_ret = takeIndexInput(MAX_INDEX);

    assert_int_equal(actual_ret, expect_ret);
}


void testTakeIndexInputEqualMax(void** state){
    (void)state;
    char input [10] = "";
    snprintf(input, sizeof(input), "%d", MAX_INDEX);
    int expect_ret = MAX_INDEX;

    will_return(__wrap_boundedInput, input);

    int actual_ret = takeIndexInput(MAX_INDEX);

    assert_int_equal(actual_ret, expect_ret);
}
