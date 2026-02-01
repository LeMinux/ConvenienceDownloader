#include "../includes/get_id_unit_test.h"

void testGetIDFromURLValidSimpleURL(void** state){
    (void) state;
    const char id [] = "eQyP7qocptY";
    const char input [] = "https://www.youtube.com/watch?v=eQyP7qocptY";

    will_return(__wrap_boundedInput, input);

    char act_id_result [YT_ID_SIZE] = "";
    int act_result = getIDFromURL(act_id_result);

    assert_int_equal(act_result, VALID);
    assert_int_equal(act_id_result[YT_ID_LEN], '\0');
    assert_string_equal(act_id_result, id);
}

void testGetIDFromURLValidLongURLWithPlayList(void** state){
    (void) state;
    const char id [] = "baRfKi0Peq4";
    const char input [] = "https://www.youtube.com/watch?v=baRfKi0Peq4&list=OLAK5uy_mmvOJKL96v8oJfcIx7eXRKk-uz2Xr0C34&index=49";

    will_return(__wrap_boundedInput, input);

    char act_id_result [YT_ID_SIZE] = "";
    int act_result = getIDFromURL(act_id_result);

    assert_int_equal(act_result, VALID);
    assert_int_equal(act_id_result[YT_ID_LEN], '\0');
    assert_string_equal(act_id_result, id);
}

void testGetIDFromURLValidLongURLWithRadio(void** state){
    (void) state;
    const char id [] = "83MZjDuZntI";
    const char input [] = "https://www.youtube.com/watch?v=83MZjDuZntI&list=RD83MZjDuZntI&start_radio=1";

    will_return(__wrap_boundedInput, input);

    char act_id_result [YT_ID_SIZE] = "";
    int act_result = getIDFromURL(act_id_result);

    assert_int_equal(act_result, VALID);
    assert_int_equal(act_id_result[YT_ID_LEN], '\0');
    assert_string_equal(act_id_result, id);
}

void testGetIDFromURLValidURLWithDash(void** state){
    (void) state;
    const char id [] = "Q16Kx9-Ys2s";
    const char input [] = "https://www.youtube.com/watch?v=Q16Kx9-Ys2s";

    will_return(__wrap_boundedInput, input);

    char act_id_result [YT_ID_SIZE] = "";
    int act_result = getIDFromURL(act_id_result);

    assert_int_equal(act_result, VALID);
    assert_int_equal(act_id_result[YT_ID_LEN], '\0');
    assert_string_equal(act_id_result, id);
}

void testGetIDFromURLValidURLWithUnderscore(void** state){
    (void) state;
    const char id [] = "C_NpZBhv2mg";
    const char input [] = "https://www.youtube.com/watch?v=C_NpZBhv2mg";
    char act_id_result [YT_ID_SIZE] = "";

    will_return(__wrap_boundedInput, input);

    int act_result = getIDFromURL(act_id_result);
    assert_int_equal(act_result, VALID);
    assert_int_equal(act_id_result[YT_ID_LEN], '\0');
    assert_string_equal(act_id_result, id);
}

void testGetIDFromURLInvalidShortURL(void** state){
    (void) state;
    const char input [] = "https://www.tube.com/watch?v=SiNOYO6ZBsU";
    char act_id_result [YT_ID_SIZE] = "";

    will_return(__wrap_boundedInput, input);

    int act_result = getIDFromURL(act_id_result);
    assert_int_equal(act_result, INVALID);
}

void testGetIDFromURLInvalidNotAYTURL(void** state){
    (void) state;
    const char input [] = "https://www.goontube.com/watch?v=dgUbRpdUN1w";
    char act_id_result [YT_ID_SIZE] = "";

    will_return(__wrap_boundedInput, input);

    int act_result = getIDFromURL(act_id_result);
    assert_int_equal(act_result, INVALID);
}

void testGetIDFromURLInvalidIDPortion(void** state){
    (void) state;
    const char input [] = "https://www.youtube.com/watch?v=d%Ub#pdU!1w";
    char act_id_result [YT_ID_SIZE] = "";

    will_return(__wrap_boundedInput, input);

    int act_result = getIDFromURL(act_id_result);
    assert_int_equal(act_result, INVALID);
}
