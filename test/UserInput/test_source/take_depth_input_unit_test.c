#include "../test_include/take_depth_input_unit_test.h"


static int __stubbed_BoundedInput(FILE* stream, char* dest, size_t dest_size){
    //don't care about these just want to return a string
    (void)stream;
    (void)dest_size;
    dest = mock_parameter_ptr_type(dest, char*);
    return mock();
    /*
    //fgets is normally the better choice for taking user input,
    //However, On input where it's just EOF length can't be determined if previous input wasn't overwritten

    size_t amount_written = 0;
    int data = '\0';
    int found_end = 0;
    while(amount_written < dest_size - 1){
        data = getc(stream);
        if(data == '\n' || data == EOF){
            if(ferror(stream)){
                PRINT_ERROR("Encountered an error reading stream for input");
                return HAD_ERROR;
            }
            found_end = 1;
            break;
        }
        dest[amount_written++] = data;
    }

    if(!found_end) clearLine(stream);

    dest[amount_written] = '\0';
    return amount_written;
    */
}

void testTakeDepthInputEmptyInput(void** state){
    (void)state;
    char input [] = "";
    int expect_ret = INVALID;

    will_return(__stubbed_BoundedInput, input);

    int actual_ret = takeDepthInput();

    assert_true(actual_ret == expect_ret);
}

void testTakeDepthInputJustNewline(void** state){
    (void)state;
    char input [] = "\n";
    int expect_ret = INVALID;

    will_return(__stubbed_BoundedInput, input);

    int actual_ret = takeDepthInput();

    assert_true(actual_ret == expect_ret);
}

void testTakeDepthInputNegativeNumber(void** state){
    (void)state;
    char input [] = "-1";
    int expect_ret = INVALID;

    will_return(__stubbed_BoundedInput, input);

    int actual_ret = takeDepthInput();

    assert_true(actual_ret == expect_ret);
    (void)state;
}

void testTakeDepthInputReallyLargeNumber(void** state){
    (void)state;
    char input [] = "99999999999999";
    int expect_ret = INVALID;

    will_return(__stubbed_BoundedInput, input);

    int actual_ret = takeDepthInput();

    assert_true(actual_ret == expect_ret);
}

void testTakeDepthInputRandomLetters(void** state){
    (void)state;
    char input [] = "eaithaiha";
    int expect_ret = INVALID;

    will_return(__stubbed_BoundedInput, input);

    int actual_ret = takeDepthInput();

    assert_true(actual_ret == expect_ret);
}

void testTakeDepthInputNotAFullNumberLettersBefore(void** state){
    (void)state;
    char input [] = "aa7";
    int expect_ret = INVALID;

    will_return(__stubbed_BoundedInput, input);

    int actual_ret = takeDepthInput();

    assert_true(actual_ret == expect_ret);
}

void testTakeDepthInputNotAFullNumberLettersAfter(void** state){
    (void)state;
    char input [] = "7aa";
    int expect_ret = INVALID;

    will_return(__stubbed_BoundedInput, input);

    int actual_ret = takeDepthInput();

    assert_true(actual_ret == expect_ret);
}

void testTakeDepthInputZero(void** state){
    (void)state;
    char input [] = "0";
    int expect_ret = 0;

    will_return(__stubbed_BoundedInput, input);

    int actual_ret = takeDepthInput();

    assert_true(actual_ret == expect_ret);
}

void testTakeDepthInputInBoundNumber(void** state){
    (void)state;
    char input [] = "27";
    int expect_ret = 27;

    will_return(__stubbed_BoundedInput, input);

    int actual_ret = takeDepthInput();

    assert_true(actual_ret == expect_ret);
}

void testTakeDepthInputWithCommas(void** state){
    (void)state;
    char input [] = "5,000,000";
    int expect_ret = 5000000;

    will_return(__stubbed_BoundedInput, input);

    int actual_ret = takeDepthInput();

    assert_true(actual_ret == expect_ret);
}

void testTakeDepthInputInfInput(void** state){
    (void)state;
    char input [] = INF_STRING;
    int expect_ret = INF_DEPTH;

    will_return(__stubbed_BoundedInput, input);

    int actual_ret = takeDepthInput();

    assert_true(actual_ret == expect_ret);
}
