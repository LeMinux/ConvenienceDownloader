#include "./user_input_unit_test.h"

static FILE* createMemFile(const char* buffer, size_t write_size){
    FILE* temp = tmpfile();

    if(temp == NULL){
        fprintf(stderr, "Could not create temporary file for testing\n");
        return NULL;
    }

    //fwrite returns how many objects were successful
    //in this case it's one object of write_amount bytes
    //-1 since we don't need the nul byte in there
    if(fwrite(buffer, write_size - 1, 1, temp) != 1){
        fprintf(stderr, "Didn't write proper length of content to temporary file \n");
        return NULL;
    }

    (void)fflush(temp);

    if(fseek(temp, 0, SEEK_SET) != 0){
        fprintf(stderr, "Temporary file could not set itself to beginning\n");
        return NULL;
    }

    return temp;
}

static void closeMemFile(FILE* temp_file){
    (void)fclose(temp_file);
}

void testExactInputExactBoundWithNewline(void** state){
    (void) state;
    char input_text [] = "AAAAAAAAAAAAAAAAAAAA\n";
    FILE* input_stream = createMemFile(input_text, sizeof(input_text));

    char dest [21] = "";
    int amount_written = exactInput(input_stream, dest, sizeof(dest));

    closeMemFile(input_stream);

    assert_null(strchr(dest, '\n'));
    assert_int_equal(amount_written, 20);
    assert_true(dest[20] == '\0');
}

void testExactInputExactBoundWithoutNewline(void** state){
    (void) state;
    char input_text [] = "AAAAAAAAAAAAAAAAAAAAA";
    FILE* input_stream = createMemFile(input_text, sizeof(input_text));

    char dest [21] = "";
    int amount_written = exactInput(input_stream, dest, sizeof(dest));

    closeMemFile(input_stream);

    assert_int_equal(amount_written, 20);
    assert_true(dest[20] == '\0');
}

void testExactInputNewlineAtExact(void** state){
    (void) state;
    char input_text [] = "AAAAAAAAAAAAAAAAAAA\n";
    FILE* input_stream = createMemFile(input_text, sizeof(input_text));

    char dest [21] = "";
    int amount_written = exactInput(input_stream, dest, sizeof(dest));

    closeMemFile(input_stream);

    assert_null(strchr(dest, '\n'));
    assert_int_equal(amount_written, 19);
    assert_true(dest[19] == '\0');
}

void testExactInputLessThanBoundWithNewline(void** state){
    (void) state;
    char input_text [] = "AAAAAAAAAA\n";
    FILE* input_stream = createMemFile(input_text, sizeof(input_text));

    char dest [21] = "";
    int amount_written = exactInput(input_stream, dest, sizeof(dest));

    closeMemFile(input_stream);

    assert_null(strchr(dest, '\n'));
    assert_int_equal(amount_written, 10);
    assert_true(dest[10] == '\0');
}

void testExactInputLessThanBoundWithoutNewline(void** state){
    (void) state;
    char input_text [] = "AAAAAAAAAAAAAAA";
    FILE* input_stream = createMemFile(input_text, sizeof(input_text));

    char dest [21] = "";
    int amount_written = exactInput(input_stream, dest, sizeof(dest));

    closeMemFile(input_stream);

    assert_int_equal(amount_written, 15);
    assert_true(dest[15] == '\0');
}

void testExactInputGreaterThanBoundWithNewline(void** state){
    (void) state;
    char input_text [] = "AAAAAAAAAAAAAAAAAAAAAAAA\n";
    FILE* input_stream = createMemFile(input_text, sizeof(input_text));

    char dest [21] = "";
    int amount_written = exactInput(input_stream, dest, sizeof(dest));

    closeMemFile(input_stream);

    assert_null(strchr(dest, '\n'));
    assert_int_equal(amount_written, 20);
    assert_true(dest[20] == '\0');
}

void testExactInputGreaterThanBoundWithoutNewline(void** state){
    (void) state;
    char input_text [] = "AAAAAAAAAAAAAAAAAAAAAAAA";
    FILE* input_stream = createMemFile(input_text, sizeof(input_text));

    char dest [21] = "";
    int amount_written = exactInput(input_stream, dest, sizeof(dest));

    closeMemFile(input_stream);

    assert_int_equal(amount_written, 20);
    assert_true(dest[20] == '\0');
}

void testExactInputClearsLineWithNewline(void** state){
    (void) state;
    char input_text [] = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n";
    FILE* input_stream = createMemFile(input_text, sizeof(input_text));

    char dest [21] = "";
    (void)exactInput(input_stream, dest, sizeof(dest));

    assert_int_equal(ftell(input_stream), 36);

    closeMemFile(input_stream);
}

void testExactInputClearsLineWithoutNewline(void** state){
    (void) state;
    char input_text [] = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
    FILE* input_stream = createMemFile(input_text, sizeof(input_text));

    char dest [21] = "";
    (void)exactInput(input_stream, dest, sizeof(dest));

    assert_int_equal(ftell(input_stream), 35);

    closeMemFile(input_stream);
}

void testExactInputJustNewline(void** state){
    (void) state;
    char input_text [] = "\n";
    FILE* input_stream = createMemFile(input_text, sizeof(input_text));

    char dest [21] = "";
    int amount_written = exactInput(input_stream, dest, sizeof(dest));

    closeMemFile(input_stream);

    assert_null(strchr(dest, '\n'));
    assert_int_equal(amount_written, 0);
    assert_true(dest[0] == '\0');
}

void testExactInputEmptyInput(void** state){
    (void) state;
    FILE* input_stream = tmpfile();

    if(input_stream == NULL){
        fprintf(stderr, "Could not create temporary file for empty testing\n");
        fail();
    }

    char dest [21] = "";
    int amount_written = exactInput(input_stream, dest, sizeof(dest));

    closeMemFile(input_stream);

    assert_null(strchr(dest, '\n'));
    assert_int_equal(amount_written, 0);
    assert_true(dest[0] == '\0');
}
