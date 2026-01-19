#include "../test_include/update_menu_unit_test.h"

static void assertUpdateData(sqlite3* database, enum CONFIG exp_type, const char* exp_input, size_t exp_input_len, int exp_depth){
    char sql_check [] = "SELECT root_name, root_length, root_depth FROM Roots WHERE root_name = ? AND root_type = ?;";
    sqlite3_stmt* statement = NULL;
    int ret_code = sqlite3_prepare_v2(database, sql_check, -1, &statement, NULL);

    if(ret_code != SQLITE_OK){
        fail_msg("Failed to check data due to %s", sqlite3_errmsg(database));
    }

    if(sqlite3_bind_text(statement, 1, exp_input, exp_input_len + 1, NULL) != SQLITE_OK ||
        sqlite3_bind_int(statement, 2, exp_type) != SQLITE_OK){
        fail_msg("Failed to bind data due to %s", sqlite3_errmsg(database));
    }

    ;
    int update_count = 0;
    while((ret_code = sqlite3_step(statement)) == SQLITE_ROW){
        ++update_count;
        char* added_string = (char*)sqlite3_column_text(statement, 0);
        int root_length = sqlite3_column_int(statement, 1);
        int root_depth = sqlite3_column_int(statement, 2);

        assert_string_equal(added_string, exp_input);
        assert_int_equal(root_length, exp_input_len);
        assert_int_equal(root_depth, exp_depth);
    }

    if(update_count == 0){
        fail_msg("Did not update anything\n", exp_input);
    }

    if(update_count > 1){
        fail_msg("Updated too many root rows. Check the SQL statement\n");
    }
}

void testUpdateEntryCatchesInvalidIndex(void** state){
    (void) state;
    char edit_select [] = {UPT_OPT, '\0'};
    int index_input = INVALID;
    enum CONFIG config_type = AUDIO_CONFIG;

    expect_function_calls(__wrap_takeIndexInput, 2);
    will_return(__wrap_boundedInput, edit_select);
    will_return(__wrap_boundedInput, 1);
    will_return(__wrap_takeIndexInput, index_input);

    editMenu(config_type);
}

void testUpdateEntryCatchesInvalidPath(void** state){
    (void) state;
    char edit_select [] = {UPT_OPT, '\0'};
    int index_input = 1;
    char* dir_input = NULL;
    enum CONFIG config_type = AUDIO_CONFIG;

    expect_function_calls(__wrap_takeIndexInput, 1);
    expect_function_calls(__wrap_takeDirectoryInput, 2);
    will_return(__wrap_boundedInput, edit_select);
    will_return(__wrap_boundedInput, 1);
    will_return(__wrap_takeIndexInput, index_input);
    will_return(__wrap_takeDirectoryInput, dir_input);

    editMenu(config_type);
}

void testUpdateEntryCatchesInvalidDepth(void** state){
    (void) state;
    char edit_select [] = {UPT_OPT, '\0'};
    int index_input = 1;
    char dir_input [] = "IDunnoSomePath/";
    int depth_input = INVALID;
    enum CONFIG config_type = AUDIO_CONFIG;

    expect_function_calls(__wrap_takeIndexInput, 1);
    expect_function_calls(__wrap_takeDirectoryInput, 1);
    expect_function_calls(__wrap_takeDepthInput, 2);
    will_return(__wrap_boundedInput, edit_select);
    will_return(__wrap_boundedInput, 1);
    will_return(__wrap_takeIndexInput, index_input);
    will_return(__wrap_takeDirectoryInput, dir_input);
    will_return(__wrap_takeDepthInput, depth_input);

    editMenu(config_type);
}

void testUpdateEntryDiffPathAndDiffDepth(void** state){
    (void) state;
    sqlite3* database = *state;
    char edit_select [] = {UPT_OPT, '\0'};
    char exit_select [] = {EXT_OPT, '\0'};
    int index_input = 1;
    const char dir_input [] = "UpdatedAudioRoot/";
    int depth_input = 7;
    enum CONFIG config_type = AUDIO_CONFIG;

    expect_function_calls(__wrap_takeIndexInput, 1);
    expect_function_calls(__wrap_takeDirectoryInput, 1);
    expect_function_calls(__wrap_takeDepthInput, 1);
    will_return(__wrap_boundedInput, edit_select);
    will_return(__wrap_boundedInput, 1);
    will_return(__wrap_takeIndexInput, index_input);
    will_return(__wrap_takeDirectoryInput, dir_input);
    will_return(__wrap_takeDepthInput, depth_input);
    will_return(__wrap_boundedInput, exit_select);
    will_return(__wrap_boundedInput, 1);

    editMenu(config_type);

    assertUpdateData(database, config_type, dir_input, sizeof(dir_input) - 1, depth_input);
}

void testUpdateEntryDiffPathAndSameDepth(void** state){
    (void) state;
    sqlite3* database = *state;
    char edit_select [] = {UPT_OPT, '\0'};
    char exit_select [] = {EXT_OPT, '\0'};
    int index_input = 1;
    const char dir_input [] = "UpdatedAudioRoot/";
    int depth_input = INF_DEPTH;
    enum CONFIG config_type = AUDIO_CONFIG;

    expect_function_calls(__wrap_takeIndexInput, 1);
    expect_function_calls(__wrap_takeDirectoryInput, 1);
    expect_function_calls(__wrap_takeDepthInput, 1);
    will_return(__wrap_boundedInput, edit_select);
    will_return(__wrap_boundedInput, 1);
    will_return(__wrap_takeIndexInput, index_input);
    will_return(__wrap_takeDirectoryInput, dir_input);
    will_return(__wrap_takeDepthInput, depth_input);
    will_return(__wrap_boundedInput, exit_select);
    will_return(__wrap_boundedInput, 1);

    editMenu(config_type);

    assertUpdateData(database, config_type, dir_input, sizeof(dir_input) - 1, depth_input);
}

void testUpdateEntrySamePathAndDiffDepth(void** state){
    (void) state;
    sqlite3* database = *state;
    char edit_select [] = {UPT_OPT, '\0'};
    char exit_select [] = {EXT_OPT, '\0'};
    int index_input = 1;
    const char dir_input [] = AUDIO_ROOT_2;
    int depth_input = 82;
    enum CONFIG config_type = AUDIO_CONFIG;

    expect_function_calls(__wrap_takeIndexInput, 1);
    expect_function_calls(__wrap_takeDirectoryInput, 1);
    expect_function_calls(__wrap_takeDepthInput, 1);
    will_return(__wrap_boundedInput, edit_select);
    will_return(__wrap_boundedInput, 1);
    will_return(__wrap_takeIndexInput, index_input);
    will_return(__wrap_takeDirectoryInput, dir_input);
    will_return(__wrap_takeDepthInput, depth_input);
    will_return(__wrap_boundedInput, exit_select);
    will_return(__wrap_boundedInput, 1);

    editMenu(config_type);

    assertUpdateData(database, config_type, dir_input, sizeof(dir_input) - 1, depth_input);
}

void testUpdateEntrySamePathAndSameDepth(void** state){
    (void) state;
    sqlite3* database = *state;
    char edit_select [] = {UPT_OPT, '\0'};
    char exit_select [] = {EXT_OPT, '\0'};
    int index_input = 1;
    const char dir_input [] = "/home/user/Music/Rock/";
    int depth_input = INF_DEPTH;
    enum CONFIG config_type = AUDIO_CONFIG;

    expect_function_calls(__wrap_takeIndexInput, 1);
    expect_function_calls(__wrap_takeDirectoryInput, 1);
    expect_function_calls(__wrap_takeDepthInput, 1);
    will_return(__wrap_boundedInput, edit_select);
    will_return(__wrap_boundedInput, 1);
    will_return(__wrap_takeIndexInput, index_input);
    will_return(__wrap_takeDirectoryInput, dir_input);
    will_return(__wrap_takeDepthInput, depth_input);
    will_return(__wrap_boundedInput, exit_select);
    will_return(__wrap_boundedInput, 1);

    editMenu(config_type);

    assertUpdateData(database, config_type, dir_input, sizeof(dir_input) - 1, depth_input);
}

void testUpdateEntryBlackList(void** state){
    (void) state;
    sqlite3* database = *state;
    char edit_select [] = {UPT_OPT, '\0'};
    char exit_select [] = {EXT_OPT, '\0'};
    int index_input = 1;
    const char dir_input [] = "/SomePathIDon'tWant";
    enum CONFIG config_type = BLACK_CONFIG;

    expect_function_calls(__wrap_takeIndexInput, 1);
    expect_function_calls(__wrap_takeDirectoryInput, 1);
    will_return(__wrap_boundedInput, edit_select);
    will_return(__wrap_boundedInput, 1);
    will_return(__wrap_takeIndexInput, index_input);
    will_return(__wrap_takeDirectoryInput, dir_input);
    will_return(__wrap_boundedInput, exit_select);
    will_return(__wrap_boundedInput, 1);

    editMenu(config_type);

    assertUpdateData(database, config_type, dir_input, sizeof(dir_input) - 1, 0);
}

void testUpdateEntryVideoConfig(void** state){
    (void) state;
    sqlite3* database = *state;
    char edit_select [] = {UPT_OPT, '\0'};
    char exit_select [] = {EXT_OPT, '\0'};
    int index_input = 1;
    const char dir_input [] = "UpdatedVideoRoot/";
    int depth_input = 7;
    enum CONFIG config_type = VIDEO_CONFIG;

    expect_function_calls(__wrap_takeIndexInput, 1);
    expect_function_calls(__wrap_takeDirectoryInput, 1);
    expect_function_calls(__wrap_takeDepthInput, 1);
    will_return(__wrap_boundedInput, edit_select);
    will_return(__wrap_boundedInput, 1);
    will_return(__wrap_takeIndexInput, index_input);
    will_return(__wrap_takeDirectoryInput, dir_input);
    will_return(__wrap_takeDepthInput, depth_input);
    will_return(__wrap_boundedInput, exit_select);
    will_return(__wrap_boundedInput, 1);

    editMenu(config_type);

    assertUpdateData(database, config_type, dir_input, sizeof(dir_input) - 1, depth_input);
}

void testUpdateEntryCoverConfig(void** state){
    (void) state;
    sqlite3* database = *state;
    char edit_select [] = {UPT_OPT, '\0'};
    char exit_select [] = {EXT_OPT, '\0'};
    int index_input = 1;
    const char dir_input [] = "UpdatedCoverRoot/";
    int depth_input = 7;
    enum CONFIG config_type = COVER_CONFIG;

    expect_function_calls(__wrap_takeIndexInput, 1);
    expect_function_calls(__wrap_takeDirectoryInput, 1);
    expect_function_calls(__wrap_takeDepthInput, 1);
    will_return(__wrap_boundedInput, edit_select);
    will_return(__wrap_boundedInput, 1);
    will_return(__wrap_takeIndexInput, index_input);
    will_return(__wrap_takeDirectoryInput, dir_input);
    will_return(__wrap_takeDepthInput, depth_input);
    will_return(__wrap_boundedInput, exit_select);
    will_return(__wrap_boundedInput, 1);

    editMenu(config_type);

    assertUpdateData(database, config_type, dir_input, sizeof(dir_input) - 1, depth_input);
}

void testUpdateEntryDuplicateNameInSameConfig(void** state){
    (void) state;
    char edit_select [] = {UPT_OPT, '\0'};
    int index_input = 1;
    const char* dir_input = AUDIO_ROOT_1;
    enum CONFIG config_type = AUDIO_CONFIG;

    expect_function_calls(__wrap_takeIndexInput, 1);
    expect_function_calls(__wrap_takeDirectoryInput, 2);
    will_return(__wrap_boundedInput, edit_select);
    will_return(__wrap_boundedInput, 1);
    will_return(__wrap_takeIndexInput, index_input);
    will_return(__wrap_takeDirectoryInput, dir_input);

    editMenu(config_type);
}

void testUpdateEntryDuplicateNameInDiffConfig(void** state){
    (void) state;
    sqlite3* database = *state;
    char edit_select [] = {UPT_OPT, '\0'};
    char exit_select [] = {EXT_OPT, '\0'};
    int index_input = 1;
    const char dir_input [] = AUDIO_ROOT_2;
    int depth_input = 7;
    enum CONFIG config_type = VIDEO_CONFIG;

    expect_function_calls(__wrap_takeIndexInput, 1);
    expect_function_calls(__wrap_takeDirectoryInput, 1);
    expect_function_calls(__wrap_takeDepthInput, 1);
    will_return(__wrap_boundedInput, edit_select);
    will_return(__wrap_boundedInput, 1);
    will_return(__wrap_takeIndexInput, index_input);
    will_return(__wrap_takeDirectoryInput, dir_input);
    will_return(__wrap_takeDepthInput, depth_input);
    will_return(__wrap_boundedInput, exit_select);
    will_return(__wrap_boundedInput, 1);

    editMenu(config_type);

    assertUpdateData(database, config_type, dir_input, sizeof(dir_input) - 1, depth_input);
}

void testUpdateEntryAddingExistingToBlackList(void** state){
    (void) state;
    char edit_select [] = {UPT_OPT, '\0'};
    int index_input = 1;
    const char dir_input [] = AUDIO_ROOT_1;
    enum CONFIG config_type = BLACK_CONFIG;

    expect_function_calls(__wrap_takeIndexInput, 1);
    expect_function_calls(__wrap_takeDirectoryInput, 2);
    will_return(__wrap_boundedInput, edit_select);
    will_return(__wrap_boundedInput, 1);
    will_return(__wrap_takeIndexInput, index_input);
    will_return(__wrap_takeDirectoryInput, dir_input);

    editMenu(config_type);
}
