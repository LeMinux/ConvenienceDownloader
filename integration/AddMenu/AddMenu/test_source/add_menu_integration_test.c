#include "../test_include/add_menu_integration_test.h"

static const char add_dup_format [] =
    "INSERT INTO Roots (root_id, root_type, root_name, root_length, root_depth) VALUES"
    "(99, ?, ?, ?, 4)";

static void addDupEntry(sqlite3* test_db, enum CONFIG config, const char* dup){
    sqlite3_stmt* statement = NULL;
    int ret_code = sqlite3_prepare_v2(test_db, add_dup_format, -1, &statement, NULL);

    if(ret_code != SQLITE_OK){
        fail_msg("Failed to add duplicate due to %s", sqlite3_errmsg(test_db));
    }

    if(sqlite3_bind_int(statement, 1, config) != SQLITE_OK ||
        sqlite3_bind_text(statement, 2, dup, strlen(dup) + 1, NULL) != SQLITE_OK ||
        sqlite3_bind_int(statement, 3, strlen(dup)) != SQLITE_OK)
    {
        fail_msg("Failed to bind data for duplicate due to %s", sqlite3_errmsg(test_db));
    }

    ret_code = sqlite3_step(statement);
    if(ret_code != SQLITE_DONE){
        fail_msg("Did not add duplicate entry %s", dup);
    }
}

static void assertRootRow(sqlite3* database, enum CONFIG exp_type, const char* exp_input, size_t exp_input_len, int exp_depth){
    char sql_check [] = "SELECT root_type, root_name, root_length, root_depth FROM Roots WHERE root_name = ? AND root_type = ?";
    sqlite3_stmt* statement = NULL;
    int ret_code = sqlite3_prepare_v2(database, sql_check, -1, &statement, NULL);

    if(ret_code != SQLITE_OK){
        fail_msg("Failed to check data due to %s", sqlite3_errmsg(database));
    }

    if(sqlite3_bind_text(statement, 1, exp_input, exp_input_len + 1, NULL) != SQLITE_OK ||
        sqlite3_bind_int(statement, 2, exp_type) != SQLITE_OK){
        fail_msg("Failed to bind data due to %s", sqlite3_errmsg(database));
    }

    ret_code = sqlite3_step(statement);
    if(ret_code == SQLITE_ROW){
        enum CONFIG added_type = sqlite3_column_int(statement, 0);
        char* added_string = (char*)sqlite3_column_text(statement, 1);
        int root_length = sqlite3_column_int(statement, 2);
        int root_depth = sqlite3_column_int(statement, 3);

        assert_int_equal(added_type, exp_type);
        assert_string_equal(added_string, exp_input);
        assert_int_equal(root_length, exp_input_len);
        assert_int_equal(root_depth, exp_depth);
    }else{
        fail_msg("Did not add entry %s", exp_input);
    }
}

static int indexOfExpString(const char* act_string, const PathCheck_t* exp_list, size_t length){
    int is_found = NOT_FOUND;
    for(int i = 0; i < length && is_found == NOT_FOUND; ++i){
        if(strcmp(act_string, exp_list[i].exp_path_name) == 0){
            is_found = i;
        }
    }

    return is_found;
}

//Using an empty database per test makes it easier by not dealing with obtaining
//a root ID
static void assertPaths(sqlite3* database, int exp_count, PathCheck_t* exp_path_names){
    char sql_count [] = "SELECT COUNT(path_index) FROM Paths";
    char sql_get_paths [] = "SELECT path_name, path_length FROM Paths";
    sqlite3_stmt* count_statement = NULL;
    sqlite3_stmt* paths_statement = NULL;

    int ret_code = sqlite3_prepare_v2(database, sql_get_paths, -1, &paths_statement, NULL);
    if(ret_code != SQLITE_OK){
        fail_msg("Failed to check data due to %s", sqlite3_errmsg(database));
    }

    ret_code = sqlite3_prepare_v2(database, sql_count, -1, &count_statement, NULL);
    if(ret_code != SQLITE_OK){
        fail_msg("Failed to check data due to %s", sqlite3_errmsg(database));
    }

    ret_code = sqlite3_step(count_statement);
    if(ret_code == SQLITE_ROW){
        int act_count = sqlite3_column_int(count_statement, 0);
        assert_int_equal(act_count, exp_count);

        while((ret_code = sqlite3_step(paths_statement)) == SQLITE_ROW){
            char* act_path_name = (char*)sqlite3_column_text(paths_statement, 0);
            int act_path_length = sqlite3_column_int(statement, 1);

            int index = indexOfExpString(act_path_name, exp_path_names, exp_count);
            assert_int_not_equal(index, NOT_FOUND);
            assert_int_equal(act_count, exp_path_names[i].exp_path_len);
        }
    }else{
        fail_msg("Could not get count of paths");
    }
}

void testAddEntryCatchesInvalidPath(void** state){
    (void) state;
    char edit_select [] = {ADD_OPT, '\0'};
    char* dir_input = NULL;
    enum CONFIG config_type = AUDIO_CONFIG;

    expect_function_calls(__wrap_takeDirectoryInput, 2);
    will_return(__wrap_boundedInput, edit_select);
    will_return(__wrap_boundedInput, 1);
    will_return(__wrap_takeDirectoryInput, dir_input);

    editMenu(config_type);
}

void testAddEntryCatchesInvalidDepth(void** state){
    (void) state;
    char edit_select [] = {ADD_OPT, '\0'};
    char dir_input [] = "TotallyRealPath";
    int depth_input = INVALID;
    enum CONFIG config_type = AUDIO_CONFIG;

    expect_function_calls(__wrap_takeDirectoryInput, 1);
    expect_function_calls(__wrap_takeDepthInput, 2);
    will_return(__wrap_boundedInput, edit_select);
    will_return(__wrap_boundedInput, 1);
    will_return(__wrap_takeDirectoryInput, dir_input);
    will_return(__wrap_takeDepthInput, depth_input);

    editMenu(config_type);
}

void testAddEntryEnterInfInputOnRootDir(void** state){
    sqlite3* database = *state;
    char edit_select [] = {ADD_OPT, '\0'};
    char exit_select [] = {EXT_OPT, '\0'};
    const char input [] = ROOT1;
    PathCheck_t exp_stats [] = {
        {LEFT_DIR, sizeof(LEFT_DIR) - 1},
        {LEFT_DIR_LEFT, sizeof(LEFT_DIR_LEFT) - 1},
        {LEFT_DIR_RIGHT, sizeof(LEFT_DIR_RIGTH) - 1},
        {LEFT_DIR_MOST_INNER, sizeof(LEFT_DIR_MOST_INNER) - 1},
        {RIGHT_DIR, sizeof(RIGHT_DIR) - 1},
        {RIGHT_DIR_RIGHT, sizeof(RIGHT_DIR_LEFT) - 1},
        {RIGHT_DIR_RIGHT, sizeof(RIGHT_DIR_RIGTH) - 1},
        {RIGHT_DIR_MOST_INNER, sizeof(RIGHT_DIR_MOST_INNER) - 1},
    }
    int depth_input = INF;
    enum CONFIG config_type = AUDIO_CONFIG;

    expect_function_calls(__wrap_takeDirectoryInput, 1);
    expect_function_calls(__wrap_takeDepthInput, 1);
    will_return(__wrap_boundedInput, edit_select);
    will_return(__wrap_boundedInput, 1);
    will_return(__wrap_takeDirectoryInput, input);
    will_return(__wrap_takeDepthInput, depth_input);
    will_return(__wrap_boundedInput, exit_select);
    will_return(__wrap_boundedInput, 1);

    editMenu(config_type);

    assertRootrow(database, config_type, input, sizeof(input) - 1, depth_input);
    assertPaths(database, sizeof(exp_stats)/sizeof(exp_stats[0]), exp_stats);
}

void testAddEntryWithBlackList(void** state){
    sqlite3* database = *state;
    char edit_select [] = {ADD_OPT, '\0'};
    char exit_select [] = {EXT_OPT, '\0'};
    const char input [] = "IDontWantThisDir/";
    int exp_depth = 0;
    enum CONFIG config_type = BLACK_CONFIG;

    expect_function_calls(__wrap_takeDirectoryInput, 1);
    will_return(__wrap_boundedInput, edit_select);
    will_return(__wrap_boundedInput, 1);
    will_return(__wrap_takeDirectoryInput, input);
    will_return(__wrap_boundedInput, exit_select);
    will_return(__wrap_boundedInput, 1);

    editMenu(config_type);

    assertAddData(database, config_type, input, sizeof(input) - 1, exp_depth);
}

void testAddEntryToVideoConfig(void** state){
    sqlite3* database = *state;
    char edit_select [] = {ADD_OPT, '\0'};
    char exit_select [] = {EXT_OPT, '\0'};
    const char input [] = "HaHaVideoRoot/";
    int depth_input = 7;
    enum CONFIG config_type = VIDEO_CONFIG;

    expect_function_calls(__wrap_takeDirectoryInput, 1);
    expect_function_calls(__wrap_takeDepthInput, 1);
    will_return(__wrap_boundedInput, edit_select);
    will_return(__wrap_boundedInput, 1);
    will_return(__wrap_takeDirectoryInput, input);
    will_return(__wrap_takeDepthInput, depth_input);
    will_return(__wrap_boundedInput, exit_select);
    will_return(__wrap_boundedInput, 1);

    editMenu(config_type);

    assertAddData(database, config_type, input, sizeof(input) - 1, depth_input);
}

void testAddEntryToCoverConfig(void** state){
    sqlite3* database = *state;
    char edit_select [] = {ADD_OPT, '\0'};
    char exit_select [] = {EXT_OPT, '\0'};
    const char input [] = "HaHaCoverRoot/";
    int depth_input = 7;
    enum CONFIG config_type = COVER_CONFIG;

    expect_function_calls(__wrap_takeDirectoryInput, 1);
    expect_function_calls(__wrap_takeDepthInput, 1);
    will_return(__wrap_boundedInput, edit_select);
    will_return(__wrap_boundedInput, 1);
    will_return(__wrap_takeDirectoryInput, input);
    will_return(__wrap_takeDepthInput, depth_input);
    will_return(__wrap_boundedInput, exit_select);
    will_return(__wrap_boundedInput, 1);

    editMenu(config_type);

    assertAddData(database, config_type, input, sizeof(input) - 1, depth_input);
}

void testAddEntryDuplicateNameButDiffConfigType(void** state){
    sqlite3* database = *state;
    char edit_select [] = {ADD_OPT, '\0'};
    char exit_select [] = {EXT_OPT, '\0'};
    const char input [] = "HaHaAudioRoot/";
    int depth_input = 7;
    enum CONFIG config_type = VIDEO_CONFIG;

    expect_function_calls(__wrap_takeDirectoryInput, 1);
    expect_function_calls(__wrap_takeDepthInput, 1);
    will_return(__wrap_boundedInput, edit_select);
    will_return(__wrap_boundedInput, 1);
    will_return(__wrap_takeDirectoryInput, input);
    will_return(__wrap_takeDepthInput, depth_input);
    will_return(__wrap_boundedInput, exit_select);
    will_return(__wrap_boundedInput, 1);

    editMenu(config_type);

    assertAddData(database, config_type, input, sizeof(input) - 1, depth_input);
}

void testAddEntryCatchesDuplicateNameAndType(void** state){
    sqlite3* database = *state;
    char edit_select [] = {ADD_OPT, '\0'};
    const char input [] = DUPLICATE_ENTRY;
    enum CONFIG config_type = AUDIO_CONFIG;

    expect_function_calls(__wrap_takeDirectoryInput, 2);
    will_return(__wrap_boundedInput, edit_select);
    will_return(__wrap_boundedInput, 1);
    will_return(__wrap_takeDirectoryInput, input);

    addDupEntry(database, config_type, input);
    editMenu(config_type);
}

void testAddEntryCatchesPathInBlackList(void** state){
    (void) state;
    char edit_select [] = {ADD_OPT, '\0'};
    char* dir_input = ;
    enum CONFIG config_type = AUDIO_CONFIG;

    expect_function_calls(__wrap_takeDirectoryInput, 2);
    will_return(__wrap_boundedInput, edit_select);
    will_return(__wrap_boundedInput, 1);
    will_return(__wrap_takeDirectoryInput, dir_input);

    editMenu(config_type);
}
