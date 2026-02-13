#include "../includes/sanitize_meta_unit_test.h"

void testSanitizeMetaStringNoChange(void** state){
    (void) state;
    const char* exp_string = "IDunnoSomeArtist";
    const int exp_len = strlen(exp_string);
    char input [] = "IDunnoSomeArtist";

    int act_len = sanitizeMetaString(input);

    assert_string_equal(input, exp_string);
    assert_int_equal(act_len, exp_len);
}

void testSanitizeMetaStringChangesOutputFormat(void** state){
    (void) state;
    const char* exp_string = "%titles.%exts";
    const int exp_len = strlen(exp_string);;
    char input [] = "%(title)s.%(ext)s";

    int act_len = sanitizeMetaString(input);

    assert_string_equal(input, exp_string);
    assert_int_equal(act_len, exp_len);
}

void testSanitizeMetaStringChangesOutputFormatHasSpaces(void** state){
    (void) state;
    const char* exp_string = "%playlist_index& - |s%titles.%exts";
    const int exp_len = strlen(exp_string);;
    char input [] = "%(playlist_index&{} - |)s%(title)s.%(ext)s";

    int act_len = sanitizeMetaString(input);

    assert_string_equal(input, exp_string);
    assert_int_equal(act_len, exp_len);
}

void testSanitizeMetaStringChangesOutputFormatHasSemicolon(void** state){
    (void) state;
    const char* exp_string = "title%artists-%titles";
    const int exp_len = strlen(exp_string);;
    char input [] = "title:%(artist)s-%(title)s";

    int act_len = sanitizeMetaString(input);

    assert_string_equal(input, exp_string);
    assert_int_equal(act_len, exp_len);
}

void testSanitizeMetaStringBrackets(void** state){
    (void) state;
    const char* exp_string = "?P<meta_synopsis>";

    const int exp_len = strlen(exp_string);;
    char input [] = ":(?P<meta_synopsis>)";

    int act_len = sanitizeMetaString(input);

    assert_string_equal(input, exp_string);
    assert_int_equal(act_len, exp_len);
}

void testSanitizeMetaStringGiveZeroLength(void** state){
    (void) state;
    const char* exp_string = "";
    const int exp_len = strlen(exp_string);;
    char input [] = ":()()()()";

    int act_len = sanitizeMetaString(input);

    assert_string_equal(input, exp_string);
    assert_int_equal(act_len, exp_len);
}
