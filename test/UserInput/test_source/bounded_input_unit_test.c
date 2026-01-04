#include "../test_includes/bounded_input_unit_test.h"

static void assertCorrect(char* test_text, int test_length, const char* act_text, int act_bound, int exp_bound){

    if(test_text[test_length - 1] == '\n'){
        assert_null(strchr(act_text, '\n'));
    }

    assert_int_equal(act_bound, exp_bound);
    assert_true(act_text[act_bound] == '\0');

    test_text[exp_bound] = '\0';
    assert_string_equal(act_text, test_text);
}

static void writeData(const char* init_text, size_t init_size, FILE* file){
    //fwrite returns how many objects were successful
    //in this case it's one object of write_amount bytes
    //-1 since we don't need the nul byte in there
    if(fwrite(init_text, init_size - 1, 1, file) != 1){
        fail_msg("Didn't write proper length of content to temporary file \n");
    }

    (void)fflush(file);

    if(fseek(file, 0, SEEK_SET) != 0){
        fail_msg("Temporary file could not set itself to beginning\n");
    }
}

void testExactInputExactBoundWithNewline(void** state){
    FILE* test_file = *state;
    char input_text [] = "AAAAAAAAAAAAAAAAAAAA\n";

    writeData(input_text, sizeof(input_text), test_file);

    char dest [TEST_BUFFER_SIZE] = "";
    int input_length = strlen(input_text);
    int expected_length = sizeof(dest) - 1;

    int amount_written = boundedInput(test_file, dest, sizeof(dest));

    assertCorrect(input_text, input_length, dest, amount_written, expected_length);
}

void testExactInputExactBoundWithoutNewline(void** state){
    FILE* test_file = *state;
    char input_text [] = "AAAAAAAAAAAAAAAAAAAAA";

    writeData(input_text, sizeof(input_text), test_file);

    char dest [TEST_BUFFER_SIZE] = "";
    int input_length = strlen(input_text);
    int expected_length = sizeof(dest) - 1;

    int amount_written = boundedInput(test_file, dest, sizeof(dest));
    assertCorrect(input_text, input_length, dest, amount_written, expected_length);
}

void testExactInputNewlineAtExact(void** state){
    FILE* test_file = *state;
    char input_text [] = "AAAAAAAAAAAAAAAAAAA\n";

    writeData(input_text, sizeof(input_text), test_file);

    char dest [TEST_BUFFER_SIZE] = "";
    int input_length = strlen(input_text);
    int expected_length = strlen(input_text) - 1;

    int amount_written = boundedInput(test_file, dest, sizeof(dest));
    assertCorrect(input_text, input_length, dest, amount_written, expected_length);
}

void testExactInputLessThanBoundWithNewline(void** state){
    FILE* test_file = *state;
    char input_text [] = "AAAAAAAAAA\n";

    writeData(input_text, sizeof(input_text), test_file);

    char dest [TEST_BUFFER_SIZE] = "";
    int input_length = strlen(input_text);
    int expected_length = strlen(input_text) - 1;

    int amount_written = boundedInput(test_file, dest, sizeof(dest));
    assertCorrect(input_text, input_length, dest, amount_written, expected_length);
}

void testExactInputTakingOneCharWithNewline(void** state){
    FILE* test_file = *state;
    char input_text [] = "A\n";

    writeData(input_text, sizeof(input_text), test_file);

    char dest [TEST_BUFFER_SIZE] = "";
    int input_length = strlen(input_text);
    int expected_length = strlen(input_text) - 1;

    int amount_written = boundedInput(test_file, dest, sizeof(dest));
    assertCorrect(input_text, input_length, dest, amount_written, expected_length);
}

void testExactInputLessThanBoundWithoutNewline(void** state){
    FILE* test_file = *state;
    char input_text [] = "AAAAAAAAAAAAAAA";

    writeData(input_text, sizeof(input_text), test_file);

    char dest [TEST_BUFFER_SIZE] = "";
    int input_length = strlen(input_text);
    int expected_length = strlen(input_text);

    int amount_written = boundedInput(test_file, dest, sizeof(dest));
    assertCorrect(input_text, input_length, dest, amount_written, expected_length);
}

void testExactInputGreaterThanBoundWithNewline(void** state){
    FILE* test_file = *state;
    char input_text [] = "AAAAAAAAAAAAAAAAAAAAAAAA\n";

    writeData(input_text, sizeof(input_text), test_file);

    char dest [TEST_BUFFER_SIZE] = "";
    int input_length = strlen(input_text);
    int expected_length = TEST_BUFFER_SIZE - 1;


    int amount_written = boundedInput(test_file, dest, sizeof(dest));
    assertCorrect(input_text, input_length, dest, amount_written, expected_length);
}

void testExactInputGreaterThanBoundWithoutNewline(void** state){
    FILE* test_file = *state;
    char input_text [] = "AAAAAAAAAAAAAAAAAAAAAAAA";

    writeData(input_text, sizeof(input_text), test_file);

    char dest [TEST_BUFFER_SIZE] = "";
    int input_length = strlen(input_text);
    int expected_length = TEST_BUFFER_SIZE - 1;

    int amount_written = boundedInput(test_file, dest, sizeof(dest));
    assertCorrect(input_text, input_length, dest, amount_written, expected_length);
}

void testExactInputTakingOneCharWithoutNewline(void** state){
    FILE* test_file = *state;
    char input_text [] = "A";

    writeData(input_text, sizeof(input_text), test_file);

    char dest [TEST_BUFFER_SIZE] = "";
    int input_length = strlen(input_text);
    int expected_length = strlen(input_text);

    int amount_written = boundedInput(test_file, dest, sizeof(dest));
    assertCorrect(input_text, input_length, dest, amount_written, expected_length);
}

void testExactInputClearsLineToNewline(void** state){
    FILE* test_file = *state;
    char input_text [] = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n";

    writeData(input_text, sizeof(input_text), test_file);

    char dest [TEST_BUFFER_SIZE] = "";
    int expected_end = strlen(input_text);

    (void)boundedInput(test_file, dest, sizeof(dest));
    assert_int_equal(ftell(test_file), expected_end);
}

void testExactInputClearsLineToEOF(void** state){
    FILE* test_file = *state;
    char input_text [] = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";

    writeData(input_text, sizeof(input_text), test_file);

    char dest [TEST_BUFFER_SIZE] = "";
    int expected_end = strlen(input_text);

    (void)boundedInput(test_file, dest, sizeof(dest));
    assert_int_equal(ftell(test_file), expected_end);
}

void testExactInputJustNewline(void** state){
    FILE* test_file = *state;
    char input_text [] = "\n";

    writeData(input_text, sizeof(input_text), test_file);

    char dest [TEST_BUFFER_SIZE] = "";
    int input_length = strlen(input_text);
    int expected_length = strlen(input_text) - 1;

    int amount_written = boundedInput(test_file, dest, sizeof(dest));
    assertCorrect(input_text, input_length, dest, amount_written, expected_length);
}

//testing EOF
void testExactInputEmptyInput(void** state){
    FILE* test_file = *state;
    char dest [TEST_BUFFER_SIZE] = "";

    int amount_written = boundedInput(test_file, dest, sizeof(dest));

    assert_int_equal(amount_written, 0);
    assert_true(dest[0] == '\0');
}

void testExactInputEmptyInputResetsPrev(void** state){
    FILE* test_file = *state;

    char dest [TEST_BUFFER_SIZE] = "AAA";
    int amount_written = boundedInput(test_file, dest, sizeof(dest));

    assert_int_equal(amount_written, 0);
    assert_true(dest[0] == '\0');
}

/*
 *
 * It isn't necessary to test exact string comparisons here because
 * previous tests do that given a certain bound.
 * It can then be reasonable to say if the unit can read correctly with the bounds
 * as long as the FILE* is stopped accordingly the expected text would be correct.
 *
 */

void testExactInputMultipleLinesExactBound(void** state){
    FILE* test_file = *state;

    enum {size=TEST_BUFFER_SIZE};
    enum Fp_Spots {fp1 = 21, fp2 = 42, fp3 = 63};

    char input_text [] = "AAAAAAAAAAAAAAAAAAAA\nBBBBBBBBBBBBBBBBBBBB\nCCCCCCCCCCCCCCCCCCCC\n";

    writeData(input_text, sizeof(input_text), test_file);

    char dest [size] = "";
    int expected_length = size - 1;
    int exp_file_pos = 0;

    for(int i = 0; i < 3; ++i){
        int amount_written = boundedInput(test_file, dest, sizeof(dest));
        exp_file_pos += size;
        assert_int_equal(amount_written, expected_length);
        switch(i){
            case 0:
                assert_int_equal(ftell(test_file), fp1);

            break;
            case 1:
                assert_int_equal(ftell(test_file), fp2);
            break;

            case 2:
                assert_int_equal(ftell(test_file), fp3);
            break;
        }
    }
}

void testExactInputMultipleLinesBelowBound(void** state){
    FILE* test_file = *state;
    enum {size=11};
    enum Fp_Spots {fp1 = 11, fp2 = 22, fp3 = 33};

    char input_text [] = "AAAAAAAAAA\nBBBBBBBBBB\nCCCCCCCCCC\n";
    writeData(input_text, sizeof(input_text), test_file);

    char dest [TEST_BUFFER_SIZE] = "";
    int expected_length = size - 1;
    int exp_file_pos = 0;
    for(int i = 0; i < 3; ++i){
        int amount_written = boundedInput(test_file, dest, sizeof(dest));
        exp_file_pos += size;
        assert_int_equal(amount_written, expected_length);
        switch(i){
            case 0:
                assert_int_equal(ftell(test_file), fp1);

            break;
            case 1:
                assert_int_equal(ftell(test_file), fp2);
            break;

            case 2:
                assert_int_equal(ftell(test_file), fp3);
            break;
        }
    }
}

void testExactInputMultipleLinesAboveBound(void** state){
    FILE* test_file = *state;
    enum {size=26};
    enum Fp_Spots {fp1 = 26, fp2 = 52, fp3 = 78};
    char input_text [] = "AAAAAAAAAAAAAAAAAAAAAAAAA\nBBBBBBBBBBBBBBBBBBBBBBBBB\nCCCCCCCCCCCCCCCCCCCCCCCCC\n";
    writeData(input_text, sizeof(input_text), test_file);

    char dest [TEST_BUFFER_SIZE] = "";
    int expected_length = sizeof(dest) - 1;
    int exp_file_pos = 0;
    for(int i = 0; i < 3; ++i){
        int amount_written = boundedInput(test_file, dest, sizeof(dest));
        exp_file_pos += size;

        assert_int_equal(amount_written, expected_length);
        switch(i){
            case 0:
                assert_int_equal(ftell(test_file), fp1);

            break;
            case 1:
                assert_int_equal(ftell(test_file), fp2);
            break;

            case 2:
                assert_int_equal(ftell(test_file), fp3);
            break;
        }
    }
}

void testExactInputMultipleLinesMixedBound(void** state){
    FILE* test_file = *state;
    enum Exp_Amount {amount1 = 13, amount2 = TEST_BUFFER_SIZE - 1, amount3 = TEST_BUFFER_SIZE - 1};
    enum Fp_Spots {fp1 = 14, fp2 = 35, fp3 = 61};

    char input_text [] = "AAAAAAAAAAAAA\nBBBBBBBBBBBBBBBBBBBB\nCCCCCCCCCCCCCCCCCCCCCCCCC\n";

    writeData(input_text, sizeof(input_text), test_file);

    char dest [TEST_BUFFER_SIZE] = "";
    for(int i = 0; i < 3; ++i){
        int amount_written = boundedInput(test_file, dest, sizeof(dest));
        switch(i){
            case 0:
                assert_int_equal(amount_written, amount1);
                assert_int_equal(ftell(test_file), fp1);

            break;
            case 1:
                assert_int_equal(amount_written, amount2);
                assert_int_equal(ftell(test_file), fp2);
            break;

            case 2:
                assert_int_equal(amount_written, amount3);
                assert_int_equal(ftell(test_file), fp3);
            break;
        }
    }
}
