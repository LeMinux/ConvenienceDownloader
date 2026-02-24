#include "../includes/get_file_line_unit_test.h"

static void writeData(FILE* file, const char* format, ...){
    va_list args;
    va_start(args, format);
    vfprintf(file, format, args);
    va_end(args);

    (void)fflush(file);

    if(fseek(file, 0, SEEK_SET) != 0){
        fail_msg("Temporary file could not set itself to beginning\n");
    }
}

void testReadFileLineReturnsDoneOnEmptyFile(void** state){
    FILE* test_file = *state;
    enum FILE_INPUT exp_ret = DONE;

    char act_url [YT_URL_INPUT_SIZE];
    MetaData_t act_meta = {NULL, NULL, NULL};
    enum FILE_INPUT act_ret = readFileLine(test_file, act_url, &act_meta);

    assert_int_equal(act_ret, exp_ret);
}

void testReadFileLineReturnsDoneIfOneLine(void** state){
    FILE* test_file = *state;
    enum FILE_INPUT exp_ret = DONE;

    writeData(test_file, "Information I don't care About");

    char act_url [YT_URL_INPUT_SIZE];
    MetaData_t act_meta = {NULL, NULL, NULL};
    (void)readFileLine(test_file, act_url, &act_meta);
    enum FILE_INPUT act_ret = readFileLine(test_file, act_url, &act_meta);

    assert_int_equal(act_ret, exp_ret);
}

void testReadFileLineProceedsToNextLine(void** state){
    FILE* test_file = *state;
    const char* exp_url = SHORT_URL;
    enum FILE_INPUT exp_ret = GOOD_LINE;

    writeData(test_file, "%s\n%s",  exp_url, exp_url);

    char act_url [YT_URL_INPUT_SIZE];
    MetaData_t act_meta = {NULL, NULL, NULL};

    enum FILE_INPUT act_ret = readFileLine(test_file, act_url, &act_meta);
    assert_int_equal(act_ret, exp_ret);
    assert_string_equal(act_url, exp_url);

    memset(act_url, 0, YT_URL_INPUT_SIZE);

    act_ret = readFileLine(test_file, act_url, &act_meta);
    assert_int_equal(act_ret, exp_ret);
    assert_string_equal(act_url, exp_url);
}

void testReadFileLineShortURLNoMeta(void** state){
    FILE* test_file = *state;
    const char* exp_url = SHORT_URL;
    const MetaData_t exp_meta = {.genre=NULL, .artist=NULL, .album=NULL};
    enum FILE_INPUT exp_ret = GOOD_LINE;

    writeData(test_file, "%s",  exp_url);

    char act_url [YT_URL_INPUT_SIZE];
    MetaData_t act_meta = {NULL, NULL, NULL};

    enum FILE_INPUT act_ret = readFileLine(test_file, act_url, &act_meta);

    assert_int_equal(act_ret, exp_ret);
    assert_string_equal(act_url, exp_url);
    assert_ptr_equal(act_meta.genre, exp_meta.genre);
    assert_ptr_equal(act_meta.artist, exp_meta.artist);
    assert_ptr_equal(act_meta.album, exp_meta.album);
}

void testReadFileLineShortURLNoMetaUsingSyntax(void** state){
    FILE* test_file = *state;
    const char* exp_url = SHORT_URL;
    const MetaData_t exp_meta = {.genre=NULL, .artist=NULL, .album=NULL};
    enum FILE_INPUT exp_ret = GOOD_LINE;

    writeData(test_file, "%s|:::",  exp_url);

    char act_url [YT_URL_INPUT_SIZE];
    MetaData_t act_meta = {NULL, NULL, NULL};

    enum FILE_INPUT act_ret = readFileLine(test_file, act_url, &act_meta);

    assert_int_equal(act_ret, exp_ret);
    assert_string_equal(act_url, exp_url);
    assert_ptr_equal(act_meta.genre, exp_meta.genre);
    assert_ptr_equal(act_meta.artist, exp_meta.artist);
    assert_ptr_equal(act_meta.album, exp_meta.album);
}

void testReadFileLineShortURLNoMetaWithInitialSeperator(void** state){
    FILE* test_file = *state;
    const char* exp_url = SHORT_URL;
    const MetaData_t exp_meta = {.genre=NULL, .artist=NULL, .album=NULL};
    enum FILE_INPUT exp_ret = GOOD_LINE;

    writeData(test_file, "%s|",  exp_url);

    char act_url [YT_URL_INPUT_SIZE];
    MetaData_t act_meta = {NULL, NULL, NULL};

    enum FILE_INPUT act_ret = readFileLine(test_file, act_url, &act_meta);

    assert_int_equal(act_ret, exp_ret);
    assert_string_equal(act_url, exp_url);
    assert_ptr_equal(act_meta.genre, exp_meta.genre);
    assert_ptr_equal(act_meta.artist, exp_meta.artist);
    assert_ptr_equal(act_meta.album, exp_meta.album);
}

void testReadFileLineDetectsEmptyLine(void** state){
    FILE* test_file = *state;
    const char* exp_url = SHORT_URL;
    enum FILE_INPUT exp_ret1 = GOOD_LINE;
    enum FILE_INPUT exp_ret2 = BAD_LINE;
    enum FILE_INPUT exp_ret3 = GOOD_LINE;
    MetaData_t act_meta = {NULL, NULL, NULL};

    writeData(test_file, "%s\n\n%s",  exp_url, exp_url);

    char act_url [YT_URL_INPUT_SIZE];

    enum FILE_INPUT act_ret = readFileLine(test_file, act_url, &act_meta);
    assert_int_equal(act_ret, exp_ret1);
    assert_string_equal(act_url, exp_url);

    act_ret = readFileLine(test_file, act_url, &act_meta);
    assert_int_equal(act_ret, exp_ret2);

    act_ret = readFileLine(test_file, act_url, &act_meta);
    assert_int_equal(act_ret, exp_ret3);
    assert_string_equal(act_url, exp_url);
}

void testReadFileLineShortURLJustGenre(void** state){
    FILE* test_file = *state;
    const char* exp_url = SHORT_URL;
    const MetaData_t exp_meta = {.genre="ReallyCool", .artist=NULL, .album=NULL};
    enum FILE_INPUT exp_ret = GOOD_LINE;

    writeData(test_file, "%s|%s:",  exp_url, exp_meta.genre);

    char act_url [YT_URL_INPUT_SIZE];
    MetaData_t act_meta = {NULL, NULL, NULL};

    enum FILE_INPUT act_ret = readFileLine(test_file, act_url, &act_meta);

    assert_int_equal(act_ret, exp_ret);
    assert_string_equal(act_url, exp_url);
    assert_string_equal(act_meta.genre, exp_meta.genre);
    assert_ptr_equal(act_meta.artist, exp_meta.artist);
    assert_ptr_equal(act_meta.album, exp_meta.album);
}

void testReadFileLineShortURLJustArtist(void** state){
    FILE* test_file = *state;
    const char* exp_url = SHORT_URL;
    const MetaData_t exp_meta = {.genre=NULL, .artist="CoolDude", .album=NULL};
    enum FILE_INPUT exp_ret = GOOD_LINE;

    writeData(test_file, "%s|:%s:",  exp_url, exp_meta.artist);

    char act_url [YT_URL_INPUT_SIZE];
    MetaData_t act_meta = {NULL, NULL, NULL};

    enum FILE_INPUT act_ret = readFileLine(test_file, act_url, &act_meta);

    assert_int_equal(act_ret, exp_ret);
    assert_string_equal(act_url, exp_url);
    assert_ptr_equal(act_meta.genre, exp_meta.genre);
    assert_string_equal(act_meta.artist, exp_meta.artist);
    assert_ptr_equal(act_meta.album, exp_meta.album);
}

void testReadFileLineShortURLJustAlbum(void** state){
    FILE* test_file = *state;
    const char* exp_url = SHORT_URL;
    const MetaData_t exp_meta = {.genre=NULL, .artist=NULL, .album="CoolSongs"};
    enum FILE_INPUT exp_ret = GOOD_LINE;

    writeData(test_file, "%s|::%s:",  exp_url, exp_meta.album);

    char act_url [YT_URL_INPUT_SIZE];
    MetaData_t act_meta = {NULL, NULL, NULL};

    enum FILE_INPUT act_ret = readFileLine(test_file, act_url, &act_meta);

    assert_int_equal(act_ret, exp_ret);
    assert_string_equal(act_url, exp_url);
    assert_ptr_equal(act_meta.genre, exp_meta.genre);
    assert_ptr_equal(act_meta.artist, exp_meta.artist);
    assert_string_equal(act_meta.album, exp_meta.album);
}

void testReadFileLineShortURLGenreArtistVerboseEndingColon(void** state){
    FILE* test_file = *state;
    const char* exp_url = SHORT_URL;
    const MetaData_t exp_meta = {.genre="BallSlammer", .artist="Larry", .album=NULL};
    enum FILE_INPUT exp_ret = GOOD_LINE;

    writeData(test_file, "%s|%s:%s::",  exp_url, exp_meta.genre, exp_meta.artist);

    char act_url [YT_URL_INPUT_SIZE];
    MetaData_t act_meta = {NULL, NULL, NULL};

    enum FILE_INPUT act_ret = readFileLine(test_file, act_url, &act_meta);

    assert_int_equal(act_ret, exp_ret);
    assert_string_equal(act_url, exp_url);
    assert_string_equal(act_meta.genre, exp_meta.genre);
    assert_string_equal(act_meta.artist, exp_meta.artist);
    assert_ptr_equal(act_meta.album, exp_meta.album);
}

void testReadFileLineShortURLGenreArtist(void** state){
    FILE* test_file = *state;
    const char* exp_url = SHORT_URL;
    const MetaData_t exp_meta = {.genre="BallSlammer", .artist="Larry", .album=NULL};
    enum FILE_INPUT exp_ret = GOOD_LINE;

    writeData(test_file, "%s|%s:%s:",  exp_url, exp_meta.genre, exp_meta.artist);

    char act_url [YT_URL_INPUT_SIZE];
    MetaData_t act_meta = {NULL, NULL, NULL};

    enum FILE_INPUT act_ret = readFileLine(test_file, act_url, &act_meta);

    assert_int_equal(act_ret, exp_ret);
    assert_string_equal(act_url, exp_url);
    assert_string_equal(act_meta.genre, exp_meta.genre);
    assert_string_equal(act_meta.artist, exp_meta.artist);
    assert_ptr_equal(act_meta.album, exp_meta.album);
}

void testReadFileLineShortURLGenreAlbum(void** state){
    FILE* test_file = *state;
    const char* exp_url = SHORT_URL;
    const MetaData_t exp_meta = {.genre="BallSlammer", .artist=NULL, .album="Evil"};
    enum FILE_INPUT exp_ret = GOOD_LINE;

    writeData(test_file, "%s|%s::%s:",  exp_url, exp_meta.genre, exp_meta.album);

    char act_url [YT_URL_INPUT_SIZE];
    MetaData_t act_meta = {NULL, NULL, NULL};

    enum FILE_INPUT act_ret = readFileLine(test_file, act_url, &act_meta);

    assert_int_equal(act_ret, exp_ret);
    assert_string_equal(act_url, exp_url);
    assert_string_equal(act_meta.genre, exp_meta.genre);
    assert_ptr_equal(act_meta.artist, exp_meta.artist);
    assert_string_equal(act_meta.album, exp_meta.album);
}

void testReadFileLineShortURLArtistAlbum(void** state){
    FILE* test_file = *state;
    const char* exp_url = SHORT_URL;
    const MetaData_t exp_meta = {.genre=NULL, .artist="Larry", .album="Evil"};
    enum FILE_INPUT exp_ret = GOOD_LINE;

    writeData(test_file, "%s|:%s:%s:",  exp_url, exp_meta.artist, exp_meta.album);

    char act_url [YT_URL_INPUT_SIZE];
    MetaData_t act_meta = {NULL, NULL, NULL};

    enum FILE_INPUT act_ret = readFileLine(test_file, act_url, &act_meta);

    assert_int_equal(act_ret, exp_ret);
    assert_string_equal(act_url, exp_url);
    assert_ptr_equal(act_meta.genre, exp_meta.genre);
    assert_string_equal(act_meta.artist, exp_meta.artist);
    assert_string_equal(act_meta.album, exp_meta.album);
}

void testReadFileLineShortURLAllMeta(void** state){
    FILE* test_file = *state;
    const char* exp_url = SHORT_URL;
    const MetaData_t exp_meta = {.genre="Neon", .artist="Argon", .album="Radeon"};
    enum FILE_INPUT exp_ret = GOOD_LINE;

    writeData(test_file, "%s|%s:%s:%s:",  exp_url, exp_meta.genre, exp_meta.artist, exp_meta.album);

    char act_url [YT_URL_INPUT_SIZE];
    MetaData_t act_meta = {NULL, NULL, NULL};

    enum FILE_INPUT act_ret = readFileLine(test_file, act_url, &act_meta);

    assert_int_equal(act_ret, exp_ret);
    assert_string_equal(act_url, exp_url);
    assert_string_equal(act_meta.genre, exp_meta.genre);
    assert_string_equal(act_meta.artist, exp_meta.artist);
    assert_string_equal(act_meta.album, exp_meta.album);
}

void testReadFileLineShortURLAllMetaWithSpaces(void** state){
    FILE* test_file = *state;
    const char* exp_url = SHORT_URL;
    const MetaData_t exp_meta = {.genre="Neon Nox", .artist="Argon Aux", .album="Radeon Rocks"};
    enum FILE_INPUT exp_ret = GOOD_LINE;

    writeData(test_file, "%s|%s:%s:%s:",  exp_url, exp_meta.genre, exp_meta.artist, exp_meta.album);

    char act_url [YT_URL_INPUT_SIZE];
    MetaData_t act_meta = {NULL, NULL, NULL};

    enum FILE_INPUT act_ret = readFileLine(test_file, act_url, &act_meta);

    assert_int_equal(act_ret, exp_ret);
    assert_string_equal(act_url, exp_url);
    assert_string_equal(act_meta.genre, exp_meta.genre);
    assert_string_equal(act_meta.artist, exp_meta.artist);
    assert_string_equal(act_meta.album, exp_meta.album);
}

void testReadFileLineShortURLJustGenreNoEndingColon(void** state){
    FILE* test_file = *state;
    const char* exp_url = SHORT_URL;
    const MetaData_t exp_meta = {.genre="Barbeque", .artist=NULL, .album=NULL};
    enum FILE_INPUT exp_ret = GOOD_LINE;

    writeData(test_file, "%s|%s",  exp_url, exp_meta.genre);

    char act_url [YT_URL_INPUT_SIZE];
    MetaData_t act_meta = {NULL, NULL, NULL};

    enum FILE_INPUT act_ret = readFileLine(test_file, act_url, &act_meta);

    assert_int_equal(act_ret, exp_ret);
    assert_string_equal(act_url, exp_url);
    assert_string_equal(act_meta.genre, exp_meta.genre);
    assert_ptr_equal(act_meta.artist, exp_meta.artist);
    assert_ptr_equal(act_meta.album, exp_meta.album);
}

void testReadFileLineShortURLJustArtistNoEndingColon(void** state){
    FILE* test_file = *state;
    const char* exp_url = SHORT_URL;
    const MetaData_t exp_meta = {.genre=NULL, .artist="BadDaddy", .album=NULL};
    enum FILE_INPUT exp_ret = GOOD_LINE;

    writeData(test_file, "%s|:%s",  exp_url, exp_meta.artist);

    char act_url [YT_URL_INPUT_SIZE];
    MetaData_t act_meta = {NULL, NULL, NULL};

    enum FILE_INPUT act_ret = readFileLine(test_file, act_url, &act_meta);

    assert_int_equal(act_ret, exp_ret);
    assert_string_equal(act_url, exp_url);
    assert_ptr_equal(act_meta.genre, exp_meta.genre);
    assert_string_equal(act_meta.artist, exp_meta.artist);
    assert_ptr_equal(act_meta.album, exp_meta.album);
}

void testReadFileLineShortURLJustAlbumNoEndingColon(void** state){
    FILE* test_file = *state;
    const char* exp_url = SHORT_URL;
    const MetaData_t exp_meta = {.genre=NULL, .artist=NULL, .album="Louie"};
    enum FILE_INPUT exp_ret = GOOD_LINE;

    writeData(test_file, "%s|::%s",  exp_url, exp_meta.album);

    char act_url [YT_URL_INPUT_SIZE];
    MetaData_t act_meta = {NULL, NULL, NULL};

    enum FILE_INPUT act_ret = readFileLine(test_file, act_url, &act_meta);

    assert_int_equal(act_ret, exp_ret);
    assert_string_equal(act_url, exp_url);
    assert_ptr_equal(act_meta.genre, exp_meta.genre);
    assert_ptr_equal(act_meta.artist, exp_meta.artist);
    assert_string_equal(act_meta.album, exp_meta.album);
}

void testReadFileLineShortURLGenreArtistNoEndingColon(void** state){
    FILE* test_file = *state;
    const char* exp_url = SHORT_URL;
    const MetaData_t exp_meta = {.genre="BallSlammer", .artist="Larry", .album=NULL};
    enum FILE_INPUT exp_ret = GOOD_LINE;

    writeData(test_file, "%s|%s:%s",  exp_url, exp_meta.genre, exp_meta.artist);

    char act_url [YT_URL_INPUT_SIZE];
    MetaData_t act_meta = {NULL, NULL, NULL};

    enum FILE_INPUT act_ret = readFileLine(test_file, act_url, &act_meta);

    assert_int_equal(act_ret, exp_ret);
    assert_string_equal(act_url, exp_url);
    assert_string_equal(act_meta.genre, exp_meta.genre);
    assert_string_equal(act_meta.artist, exp_meta.artist);
    assert_ptr_equal(act_meta.album, exp_meta.album);
}

void testReadFileLineShortURLGenreAlbumNoEndingColon(void** state){
    FILE* test_file = *state;
    const char* exp_url = SHORT_URL;
    const MetaData_t exp_meta = {.genre="BallSlammer", .artist=NULL, .album="Evil"};
    enum FILE_INPUT exp_ret = GOOD_LINE;

    writeData(test_file, "%s|%s::%s",  exp_url, exp_meta.genre, exp_meta.album);

    char act_url [YT_URL_INPUT_SIZE];
    MetaData_t act_meta = {NULL, NULL, NULL};

    enum FILE_INPUT act_ret = readFileLine(test_file, act_url, &act_meta);

    assert_int_equal(act_ret, exp_ret);
    assert_string_equal(act_url, exp_url);
    assert_string_equal(act_meta.genre, exp_meta.genre);
    assert_ptr_equal(act_meta.artist, exp_meta.artist);
    assert_string_equal(act_meta.album, exp_meta.album);
}

void testReadFileLineShortURLArtistAlbumNoEndingColon(void** state){
    FILE* test_file = *state;
    const char* exp_url = SHORT_URL;
    const MetaData_t exp_meta = {.genre=NULL, .artist="Larry", .album="Evil"};
    enum FILE_INPUT exp_ret = GOOD_LINE;

    writeData(test_file, "%s|:%s:%s",  exp_url, exp_meta.artist, exp_meta.album);

    char act_url [YT_URL_INPUT_SIZE];
    MetaData_t act_meta = {NULL, NULL, NULL};

    enum FILE_INPUT act_ret = readFileLine(test_file, act_url, &act_meta);

    assert_int_equal(act_ret, exp_ret);
    assert_string_equal(act_url, exp_url);
    assert_ptr_equal(act_meta.genre, exp_meta.genre);
    assert_string_equal(act_meta.artist, exp_meta.artist);
    assert_string_equal(act_meta.album, exp_meta.album);
}

void testReadFileLineShortURLAllMetaNoEndingColon(void** state){
    FILE* test_file = *state;
    const char* exp_url = SHORT_URL;
    const MetaData_t exp_meta = {.genre="Neon", .artist="Argon", .album="Radeon"};
    enum FILE_INPUT exp_ret = GOOD_LINE;

    writeData(test_file, "%s|%s:%s:%s",  exp_url, exp_meta.genre, exp_meta.artist, exp_meta.album);

    char act_url [YT_URL_INPUT_SIZE];
    MetaData_t act_meta = {NULL, NULL, NULL};

    enum FILE_INPUT act_ret = readFileLine(test_file, act_url, &act_meta);

    assert_int_equal(act_ret, exp_ret);
    assert_string_equal(act_url, exp_url);
    assert_string_equal(act_meta.genre, exp_meta.genre);
    assert_string_equal(act_meta.artist, exp_meta.artist);
    assert_string_equal(act_meta.album, exp_meta.album);
}

void testReadFileLineLongURLNoMeta(void** state){
    FILE* test_file = *state;
    const char* exp_url = EXP_LONG;
    const MetaData_t exp_meta = {.genre=NULL, .artist=NULL, .album=NULL};
    enum FILE_INPUT exp_ret = GOOD_LINE;

    writeData(test_file, "%s",  LONG_URL);

    char act_url [YT_URL_INPUT_SIZE];
    MetaData_t act_meta = {NULL, NULL, NULL};

    enum FILE_INPUT act_ret = readFileLine(test_file, act_url, &act_meta);

    assert_int_equal(act_ret, exp_ret);
    assert_string_equal(act_url, exp_url);
    assert_ptr_equal(act_meta.genre, exp_meta.genre);
    assert_ptr_equal(act_meta.artist, exp_meta.artist);
    assert_ptr_equal(act_meta.album, exp_meta.album);
}

void testReadFileLineLongURLNoMetaUsingSyntax(void** state){
    FILE* test_file = *state;
    const char* exp_url = EXP_LONG;
    const MetaData_t exp_meta = {.genre=NULL, .artist=NULL, .album=NULL};
    enum FILE_INPUT exp_ret = GOOD_LINE;

    writeData(test_file, "%s|:::",  LONG_URL);

    char act_url [YT_URL_INPUT_SIZE];
    MetaData_t act_meta = {NULL, NULL, NULL};

    enum FILE_INPUT act_ret = readFileLine(test_file, act_url, &act_meta);

    assert_int_equal(act_ret, exp_ret);
    assert_string_equal(act_url, exp_url);
    assert_ptr_equal(act_meta.genre, exp_meta.genre);
    assert_ptr_equal(act_meta.artist, exp_meta.artist);
    assert_ptr_equal(act_meta.album, exp_meta.album);
}

void testReadFileLineLongURLNoMetaWithInitialSeperator(void** state){
    FILE* test_file = *state;
    const char* exp_url = EXP_LONG;
    const MetaData_t exp_meta = {.genre=NULL, .artist=NULL, .album=NULL};
    enum FILE_INPUT exp_ret = GOOD_LINE;

    writeData(test_file, "%s|",  LONG_URL);

    char act_url [YT_URL_INPUT_SIZE];
    MetaData_t act_meta = {NULL, NULL, NULL};

    enum FILE_INPUT act_ret = readFileLine(test_file, act_url, &act_meta);

    assert_int_equal(act_ret, exp_ret);
    assert_string_equal(act_url, exp_url);
    assert_ptr_equal(act_meta.genre, exp_meta.genre);
    assert_ptr_equal(act_meta.artist, exp_meta.artist);
    assert_ptr_equal(act_meta.album, exp_meta.album);
}

void testReadFileLineLongURLJustGenre(void** state){
    FILE* test_file = *state;
    const char* exp_url = EXP_LONG;
    const MetaData_t exp_meta = {.genre="ReallyCool", .artist=NULL, .album=NULL};
    enum FILE_INPUT exp_ret = GOOD_LINE;

    writeData(test_file, "%s|%s:",  LONG_URL, exp_meta.genre);

    char act_url [YT_URL_INPUT_SIZE];
    MetaData_t act_meta = {NULL, NULL, NULL};

    enum FILE_INPUT act_ret = readFileLine(test_file, act_url, &act_meta);

    assert_int_equal(act_ret, exp_ret);
    assert_string_equal(act_url, exp_url);
    assert_string_equal(act_meta.genre, exp_meta.genre);
    assert_ptr_equal(act_meta.artist, exp_meta.artist);
    assert_ptr_equal(act_meta.album, exp_meta.album);
}

void testReadFileLineLongURLJustArtist(void** state){
    FILE* test_file = *state;
    const char* exp_url = EXP_LONG;
    const MetaData_t exp_meta = {.genre=NULL, .artist="CoolDude", .album=NULL};
    enum FILE_INPUT exp_ret = GOOD_LINE;

    writeData(test_file, "%s|:%s:",  LONG_URL, exp_meta.artist);

    char act_url [YT_URL_INPUT_SIZE];
    MetaData_t act_meta = {NULL, NULL, NULL};

    enum FILE_INPUT act_ret = readFileLine(test_file, act_url, &act_meta);

    assert_int_equal(act_ret, exp_ret);
    assert_string_equal(act_url, exp_url);
    assert_ptr_equal(act_meta.genre, exp_meta.genre);
    assert_string_equal(act_meta.artist, exp_meta.artist);
    assert_ptr_equal(act_meta.album, exp_meta.album);
}

void testReadFileLineLongURLJustAlbum(void** state){
    FILE* test_file = *state;
    const char* exp_url = EXP_LONG;
    const MetaData_t exp_meta = {.genre=NULL, .artist=NULL, .album="CoolSongs"};
    enum FILE_INPUT exp_ret = GOOD_LINE;

    writeData(test_file, "%s|::%s:",  LONG_URL, exp_meta.album);

    char act_url [YT_URL_INPUT_SIZE];
    MetaData_t act_meta = {NULL, NULL, NULL};

    enum FILE_INPUT act_ret = readFileLine(test_file, act_url, &act_meta);

    assert_int_equal(act_ret, exp_ret);
    assert_string_equal(act_url, exp_url);
    assert_ptr_equal(act_meta.genre, exp_meta.genre);
    assert_ptr_equal(act_meta.artist, exp_meta.artist);
    assert_string_equal(act_meta.album, exp_meta.album);
}

void testReadFileLineLongURLGenreArtist(void** state){
    FILE* test_file = *state;
    const char* exp_url = EXP_LONG;
    const MetaData_t exp_meta = {.genre="BallSlammer", .artist="Larry", .album=NULL};
    enum FILE_INPUT exp_ret = GOOD_LINE;

    writeData(test_file, "%s|%s:%s:",  LONG_URL, exp_meta.genre, exp_meta.artist);

    char act_url [YT_URL_INPUT_SIZE];
    MetaData_t act_meta = {NULL, NULL, NULL};

    enum FILE_INPUT act_ret = readFileLine(test_file, act_url, &act_meta);

    assert_int_equal(act_ret, exp_ret);
    assert_string_equal(act_url, exp_url);
    assert_string_equal(act_meta.genre, exp_meta.genre);
    assert_string_equal(act_meta.artist, exp_meta.artist);
    assert_ptr_equal(act_meta.album, exp_meta.album);
}

void testReadFileLineLongURLGenreArtistVerboseEndingColon(void** state){
    FILE* test_file = *state;
    const char* exp_url = EXP_LONG;
    const MetaData_t exp_meta = {.genre="BallSlammer", .artist="Larry", .album=NULL};
    enum FILE_INPUT exp_ret = GOOD_LINE;

    writeData(test_file, "%s|%s:%s::",  LONG_URL, exp_meta.genre, exp_meta.artist);

    char act_url [YT_URL_INPUT_SIZE];
    MetaData_t act_meta = {NULL, NULL, NULL};

    enum FILE_INPUT act_ret = readFileLine(test_file, act_url, &act_meta);

    assert_int_equal(act_ret, exp_ret);
    assert_string_equal(act_url, exp_url);
    assert_string_equal(act_meta.genre, exp_meta.genre);
    assert_string_equal(act_meta.artist, exp_meta.artist);
    assert_ptr_equal(act_meta.album, exp_meta.album);
}


void testReadFileLineLongURLGenreAlbum(void** state){
    FILE* test_file = *state;
    const char* exp_url = EXP_LONG;
    const MetaData_t exp_meta = {.genre="BallSlammer", .artist=NULL, .album="Evil"};
    enum FILE_INPUT exp_ret = GOOD_LINE;

    writeData(test_file, "%s|%s::%s:",  LONG_URL, exp_meta.genre, exp_meta.album);

    char act_url [YT_URL_INPUT_SIZE];
    MetaData_t act_meta = {NULL, NULL, NULL};

    enum FILE_INPUT act_ret = readFileLine(test_file, act_url, &act_meta);

    assert_int_equal(act_ret, exp_ret);
    assert_string_equal(act_url, exp_url);
    assert_string_equal(act_meta.genre, exp_meta.genre);
    assert_ptr_equal(act_meta.artist, exp_meta.artist);
    assert_string_equal(act_meta.album, exp_meta.album);
}

void testReadFileLineLongURLArtistAlbum(void** state){
    FILE* test_file = *state;
    const char* exp_url = EXP_LONG;
    const MetaData_t exp_meta = {.genre=NULL, .artist="Larry", .album="Evil"};
    enum FILE_INPUT exp_ret = GOOD_LINE;

    writeData(test_file, "%s|:%s:%s:",  LONG_URL, exp_meta.artist, exp_meta.album);

    char act_url [YT_URL_INPUT_SIZE];
    MetaData_t act_meta = {NULL, NULL, NULL};

    enum FILE_INPUT act_ret = readFileLine(test_file, act_url, &act_meta);

    assert_int_equal(act_ret, exp_ret);
    assert_string_equal(act_url, exp_url);
    assert_ptr_equal(act_meta.genre, exp_meta.genre);
    assert_string_equal(act_meta.artist, exp_meta.artist);
    assert_string_equal(act_meta.album, exp_meta.album);
}

void testReadFileLineLongURLAllMeta(void** state){
    FILE* test_file = *state;
    const char* exp_url = EXP_LONG;
    const MetaData_t exp_meta = {.genre="Neon", .artist="Argon", .album="Radeon"};
    enum FILE_INPUT exp_ret = GOOD_LINE;

    writeData(test_file, "%s|%s:%s:%s:",  LONG_URL, exp_meta.genre, exp_meta.artist, exp_meta.album);

    char act_url [YT_URL_INPUT_SIZE];
    MetaData_t act_meta = {NULL, NULL, NULL};

    enum FILE_INPUT act_ret = readFileLine(test_file, act_url, &act_meta);

    assert_int_equal(act_ret, exp_ret);
    assert_string_equal(act_url, exp_url);
    assert_string_equal(act_meta.genre, exp_meta.genre);
    assert_string_equal(act_meta.artist, exp_meta.artist);
    assert_string_equal(act_meta.album, exp_meta.album);
}

void testReadFileLineLongURLJustGenreNoEndingColon(void** state){
    FILE* test_file = *state;
    const char* exp_url = EXP_LONG;
    const MetaData_t exp_meta = {.genre="Barbeque", .artist=NULL, .album=NULL};
    enum FILE_INPUT exp_ret = GOOD_LINE;

    writeData(test_file, "%s|%s",  LONG_URL, exp_meta.genre);

    char act_url [YT_URL_INPUT_SIZE];
    MetaData_t act_meta = {NULL, NULL, NULL};

    enum FILE_INPUT act_ret = readFileLine(test_file, act_url, &act_meta);

    assert_int_equal(act_ret, exp_ret);
    assert_string_equal(act_url, exp_url);
    assert_string_equal(act_meta.genre, exp_meta.genre);
    assert_ptr_equal(act_meta.artist, exp_meta.artist);
    assert_ptr_equal(act_meta.album, exp_meta.album);
}

void testReadFileLineLongURLJustArtistNoEndingColon(void** state){
    FILE* test_file = *state;
    const char* exp_url = EXP_LONG;
    const MetaData_t exp_meta = {.genre=NULL, .artist="BadDaddy", .album=NULL};
    enum FILE_INPUT exp_ret = GOOD_LINE;

    writeData(test_file, "%s|:%s",  LONG_URL, exp_meta.artist);

    char act_url [YT_URL_INPUT_SIZE];
    MetaData_t act_meta = {NULL, NULL, NULL};

    enum FILE_INPUT act_ret = readFileLine(test_file, act_url, &act_meta);

    assert_int_equal(act_ret, exp_ret);
    assert_string_equal(act_url, exp_url);
    assert_ptr_equal(act_meta.genre, exp_meta.genre);
    assert_string_equal(act_meta.artist, exp_meta.artist);
    assert_ptr_equal(act_meta.album, exp_meta.album);
}

void testReadFileLineLongURLJustAlbumNoEndingColon(void** state){
    FILE* test_file = *state;
    const char* exp_url = EXP_LONG;
    const MetaData_t exp_meta = {.genre=NULL, .artist=NULL, .album="Louie"};
    enum FILE_INPUT exp_ret = GOOD_LINE;

    writeData(test_file, "%s|::%s",  LONG_URL, exp_meta.album);

    char act_url [YT_URL_INPUT_SIZE];
    MetaData_t act_meta = {NULL, NULL, NULL};

    enum FILE_INPUT act_ret = readFileLine(test_file, act_url, &act_meta);

    assert_int_equal(act_ret, exp_ret);
    assert_string_equal(act_url, exp_url);
    assert_ptr_equal(act_meta.genre, exp_meta.genre);
    assert_ptr_equal(act_meta.artist, exp_meta.artist);
    assert_string_equal(act_meta.album, exp_meta.album);
}

void testReadFileLineLongURLGenreArtistNoEndingColon(void** state){
    FILE* test_file = *state;
    const char* exp_url = EXP_LONG;
    const MetaData_t exp_meta = {.genre="BallSlammer", .artist="Larry", .album=NULL};
    enum FILE_INPUT exp_ret = GOOD_LINE;

    writeData(test_file, "%s|%s:%s",  LONG_URL, exp_meta.genre, exp_meta.artist);

    char act_url [YT_URL_INPUT_SIZE];
    MetaData_t act_meta = {NULL, NULL, NULL};

    enum FILE_INPUT act_ret = readFileLine(test_file, act_url, &act_meta);

    assert_int_equal(act_ret, exp_ret);
    assert_string_equal(act_url, exp_url);
    assert_string_equal(act_meta.genre, exp_meta.genre);
    assert_string_equal(act_meta.artist, exp_meta.artist);
    assert_ptr_equal(act_meta.album, exp_meta.album);
}

void testReadFileLineLongURLGenreAlbumNoEndingColon(void** state){
    FILE* test_file = *state;
    const char* exp_url = EXP_LONG;
    const MetaData_t exp_meta = {.genre="BallSlammer", .artist=NULL, .album="Evil"};
    enum FILE_INPUT exp_ret = GOOD_LINE;

    writeData(test_file, "%s|%s::%s",  LONG_URL, exp_meta.genre, exp_meta.album);

    char act_url [YT_URL_INPUT_SIZE];
    MetaData_t act_meta = {NULL, NULL, NULL};

    enum FILE_INPUT act_ret = readFileLine(test_file, act_url, &act_meta);

    assert_int_equal(act_ret, exp_ret);
    assert_string_equal(act_url, exp_url);
    assert_string_equal(act_meta.genre, exp_meta.genre);
    assert_ptr_equal(act_meta.artist, exp_meta.artist);
    assert_string_equal(act_meta.album, exp_meta.album);
}

void testReadFileLineLongURLArtistAlbumNoEndingColon(void** state){
    FILE* test_file = *state;
    const char* exp_url = EXP_LONG;
    const MetaData_t exp_meta = {.genre=NULL, .artist="Larry", .album="Evil"};
    enum FILE_INPUT exp_ret = GOOD_LINE;

    writeData(test_file, "%s|:%s:%s",  LONG_URL, exp_meta.artist, exp_meta.album);

    char act_url [YT_URL_INPUT_SIZE];
    MetaData_t act_meta = {NULL, NULL, NULL};

    enum FILE_INPUT act_ret = readFileLine(test_file, act_url, &act_meta);

    assert_int_equal(act_ret, exp_ret);
    assert_string_equal(act_url, exp_url);
    assert_ptr_equal(act_meta.genre, exp_meta.genre);
    assert_string_equal(act_meta.artist, exp_meta.artist);
    assert_string_equal(act_meta.album, exp_meta.album);
}

void testReadFileLineLongURLAllMetaNoEndingColon(void** state){
    FILE* test_file = *state;
    const char* exp_url = EXP_LONG;
    const MetaData_t exp_meta = {.genre="Neon", .artist="Argon", .album="Radeon"};
    enum FILE_INPUT exp_ret = GOOD_LINE;

    writeData(test_file, "%s|%s:%s:%s",  LONG_URL, exp_meta.genre, exp_meta.artist, exp_meta.album);

    char act_url [YT_URL_INPUT_SIZE];
    MetaData_t act_meta = {NULL, NULL, NULL};

    enum FILE_INPUT act_ret = readFileLine(test_file, act_url, &act_meta);

    assert_int_equal(act_ret, exp_ret);
    assert_string_equal(act_url, exp_url);
    assert_string_equal(act_meta.genre, exp_meta.genre);
    assert_string_equal(act_meta.artist, exp_meta.artist);
    assert_string_equal(act_meta.album, exp_meta.album);
}

void testReadFileLineInvalidURL(void** state){
    FILE* test_file = *state;
    enum FILE_INPUT exp_ret = BAD_LINE;

    writeData(test_file, "%s",  "ayiaaaa");

    char act_url [YT_URL_INPUT_SIZE];
    MetaData_t act_meta = {NULL, NULL, NULL};

    enum FILE_INPUT act_ret = readFileLine(test_file, act_url, &act_meta);

    assert_int_equal(act_ret, exp_ret);
}

void testReadFileLineLongURLAllMetaWithSpaces(void** state){
    FILE* test_file = *state;
    const char* exp_url = EXP_LONG;
    const MetaData_t exp_meta = {.genre="Super Shiny", .artist="Kinda Dull", .album="Maybe Meh"};
    enum FILE_INPUT exp_ret = GOOD_LINE;

    writeData(test_file, "%s|%s:%s:%s:",  LONG_URL, exp_meta.genre, exp_meta.artist, exp_meta.album);

    char act_url [YT_URL_INPUT_SIZE];
    MetaData_t act_meta = {NULL, NULL, NULL};

    enum FILE_INPUT act_ret = readFileLine(test_file, act_url, &act_meta);

    assert_int_equal(act_ret, exp_ret);
    assert_string_equal(act_url, exp_url);
    assert_string_equal(act_meta.genre, exp_meta.genre);
    assert_string_equal(act_meta.artist, exp_meta.artist);
    assert_string_equal(act_meta.album, exp_meta.album);
}


void testReadFileLineTooShortID(void** state){
    FILE* test_file = *state;
    enum FILE_INPUT exp_ret = BAD_LINE;

    writeData(test_file, "%s",  "https://www.youtube.com/watch?v=ui1DpWyYeA");

    char act_url [YT_URL_INPUT_SIZE];
    MetaData_t act_meta = {NULL, NULL, NULL};

    enum FILE_INPUT act_ret = readFileLine(test_file, act_url, &act_meta);

    assert_int_equal(act_ret, exp_ret);
}

void testReadFileLineValidURLInWrongSpot(void** state){
    FILE* test_file = *state;
    enum FILE_INPUT exp_ret = BAD_LINE;

    writeData(test_file, "extra_%s",  "https://www.youtube.com/watch?v=ui1DpWyYeA");

    char act_url [YT_URL_INPUT_SIZE];
    MetaData_t act_meta = {NULL, NULL, NULL};

    enum FILE_INPUT act_ret = readFileLine(test_file, act_url, &act_meta);

    assert_int_equal(act_ret, exp_ret);
}

void testReadFileLineInvalidIDPortion(void** state){
    FILE* test_file = *state;
    enum FILE_INPUT exp_ret = BAD_LINE;

    writeData(test_file, "%s",  "https://www.youtube.com/watch?v=ui1DpW|:eiA");

    char act_url [YT_URL_INPUT_SIZE];
    MetaData_t act_meta = {NULL, NULL, NULL};

    enum FILE_INPUT act_ret = readFileLine(test_file, act_url, &act_meta);

    assert_int_equal(act_ret, exp_ret);

}

void testReadFileLineMoreSeperatorsThanNeeded(void** state){
    FILE* test_file = *state;
    const char* exp_url = SHORT_URL;
    const MetaData_t exp_meta = {.genre="Neon", .artist="Argon", .album="Radeon"};
    enum FILE_INPUT exp_ret = GOOD_LINE;

    writeData(test_file, "%s|%s:%s:%s:%s:%s",  exp_url, exp_meta.genre, exp_meta.artist, exp_meta.album,
              "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
              "GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG");

    char act_url [YT_URL_INPUT_SIZE];
    MetaData_t act_meta = {NULL, NULL, NULL};

    enum FILE_INPUT act_ret = readFileLine(test_file, act_url, &act_meta);

    assert_int_equal(act_ret, exp_ret);
    assert_string_equal(act_url, exp_url);
    assert_string_equal(act_meta.genre, exp_meta.genre);
    assert_string_equal(act_meta.artist, exp_meta.artist);
    assert_string_equal(act_meta.album, exp_meta.album);
}

void testReadFileLineUsesShorterLen(void** state){
    FILE* test_file = *state;
    const char* exp_url = SHORT_URL;
    char long_meta [GENRE_LEN + 10];
    memset(long_meta, 'M', GENRE_LEN + 10);
    enum FILE_INPUT exp_ret = GOOD_LINE;

    writeData(test_file, "%s|%s:",  exp_url, long_meta);
    long_meta[GENRE_LEN] = '\0'; //set the nul byte as string comparision should stop
    const MetaData_t exp_meta = {.genre=long_meta, .artist=NULL, .album=NULL};
    MetaData_t act_meta = {NULL, NULL, NULL};
    char act_url [YT_URL_INPUT_SIZE];

    enum FILE_INPUT act_ret = readFileLine(test_file, act_url, &act_meta);

    assert_int_equal(act_ret, exp_ret);
    assert_string_equal(act_url, exp_url);
    assert_string_equal(act_meta.genre, exp_meta.genre);
}

void testReadFileLineSanitizes(void** state){
    FILE* test_file = *state;
    const char* exp_url = SHORT_URL;
    const MetaData_t exp_meta = {.genre="%genres", .artist=NULL, .album=NULL};
    enum FILE_INPUT exp_ret = GOOD_LINE;

    writeData(test_file, "%s|%s:",  exp_url, "%(genre)s");

    char act_url [YT_URL_INPUT_SIZE];
    MetaData_t act_meta = {NULL, NULL, NULL};

    enum FILE_INPUT act_ret = readFileLine(test_file, act_url, &act_meta);

    assert_int_equal(act_ret, exp_ret);
    assert_string_equal(act_url, exp_url);
    assert_string_equal(act_meta.genre, exp_meta.genre);
    assert_ptr_equal(act_meta.artist, exp_meta.artist);
    assert_ptr_equal(act_meta.album, exp_meta.album);
}

void testReadFileLineLineOutOfBufferIsBadLine(void** state){
    FILE* test_file = *state;
    enum FILE_INPUT exp_ret = BAD_LINE;
    char long_meta [150];
    memset(long_meta, 'A', sizeof(long_meta) - 1);
    long_meta[sizeof(long_meta) - 1] = '\0';

    writeData(test_file, "%s|%s:%s:%s:", LONG_URL, long_meta, long_meta, long_meta);

    char act_url [YT_URL_INPUT_SIZE];
    MetaData_t act_meta = {NULL, NULL, NULL};

    enum FILE_INPUT act_ret = readFileLine(test_file, act_url, &act_meta);

    assert_int_equal(act_ret, exp_ret);
}
