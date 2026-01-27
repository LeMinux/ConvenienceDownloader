#include "../includes/add_menu_integration_test.h"

extern enum CALL call_real_function;

static void getRealPath(const char* input, char result [PATH_MAX]){
    if(realpath(input, result) == NULL){
        fail_msg("Could not create real path");
    }
}

static void setUpStubbedInput(const char* dir_input, const char* depth_input){
    expect_function_calls(__wrap_takeDirectoryInput, 1);
    expect_function_calls(__wrap_takeDepthInput, 1);

    will_return(__wrap_boundedInput, dir_input);
    will_return(__wrap_boundedInput, depth_input);
}

static void addExtraEntry(sqlite3* test_db, enum CONFIG config, const char* extra_name){
    static const char add_dup_format [] =
        "INSERT INTO Roots (root_id, root_type, root_name, root_length, root_depth) VALUES"
        "(99, ?, ?, ?, 5)";

    sqlite3_stmt* statement = NULL;
    int ret_code = sqlite3_prepare_v2(test_db, add_dup_format, -1, &statement, NULL);

    if(ret_code != SQLITE_OK){
        fail_msg("Failed to add extra entry due to %s", sqlite3_errmsg(test_db));
    }

    if(sqlite3_bind_int(statement, 1, config) != SQLITE_OK ||
        sqlite3_bind_text(statement, 2, extra_name, strlen(extra_name) + 1, NULL) != SQLITE_OK ||
        sqlite3_bind_int(statement, 3, strlen(extra_name)) != SQLITE_OK)
    {
        fail_msg("Failed to bind data for extra entry due to %s", sqlite3_errmsg(test_db));
    }

    ret_code = sqlite3_step(statement);
    if(ret_code != SQLITE_DONE){
        fail_msg("Did not add extra entry %s", extra_name);
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

static int indexOfExpString(const char* act_string, const PathCheck_t* exp_list, int length){
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
static void assertPaths(sqlite3* database, PathCheck_t* exp_path_names, int exp_count){
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
            int act_path_length = sqlite3_column_int(paths_statement, 1);

            int index = indexOfExpString(act_path_name, exp_path_names, exp_count);
            assert_int_not_equal(index, NOT_FOUND);
            assert_int_equal(act_path_length, exp_path_names[index].exp_path_len);
        }
    }else{
        fail_msg("Could not get count of paths");
    }
}

void testAddMenuCatchesInvalidPath(void** state){
    (void) state;
    call_real_function = NO_CALL;
    char* dir_input = NULL;
    enum CONFIG config_type = AUDIO_CONFIG;

    expect_function_calls(__wrap_takeDirectoryInput, 2);
    will_return(__wrap_takeDirectoryInput, dir_input);

    addMenu(config_type);
}

void testAddMenuCatchesInvalidDepth(void** state){
    (void) state;
    call_real_function = NO_CALL;
    char dir_input [] = "TotallyRealPath";
    int depth_input = INVALID;
    enum CONFIG config_type = AUDIO_CONFIG;

    expect_function_calls(__wrap_takeDirectoryInput, 1);
    expect_function_calls(__wrap_takeDepthInput, 2);

    will_return(__wrap_takeDirectoryInput, dir_input);
    will_return(__wrap_takeDepthInput, depth_input);

    addMenu(config_type);
}

void testAddMenuCatchesPathInBlackList(void** state){
    sqlite3* database = *state;
    call_real_function = NO_CALL;
    const char* dir_input = IN_BLACK_LIST;
    enum CONFIG config_type = AUDIO_CONFIG;

    expect_function_calls(__wrap_takeDirectoryInput, 2);
    will_return(__wrap_takeDirectoryInput, dir_input);

    addExtraEntry(database, BLACK_CONFIG, dir_input);

    addMenu(config_type);
}

void testAddMenuCatchesDuplicateNameAndType(void** state){
    sqlite3* database = *state;
    call_real_function = NO_CALL;
    const char dir_input [] = DUPLICATE_ENTRY;
    enum CONFIG config_type = AUDIO_CONFIG;

    expect_function_calls(__wrap_takeDirectoryInput, 2);
    will_return(__wrap_takeDirectoryInput, dir_input);

    addExtraEntry(database, config_type, dir_input);

    addMenu(config_type);
}

void testAddMenuEnterInfInputOnRootDir(void** state){
    sqlite3* database = *state;
    call_real_function = CALL;
    const char dir_input [] = ROOT1;
    PathCheck_t exp_paths [] = {
        {LEFT_DIR, sizeof(LEFT_DIR) - 1},
        {LEFT_DIR_LEFT, sizeof(LEFT_DIR_LEFT) - 1},
        {LEFT_DIR_RIGHT, sizeof(LEFT_DIR_RIGHT) - 1},
        {LEFT_DIR_MOST_INNER, sizeof(LEFT_DIR_MOST_INNER) - 1},
        {RIGHT_DIR, sizeof(RIGHT_DIR) - 1},
        {RIGHT_DIR_LEFT, sizeof(RIGHT_DIR_LEFT) - 1},
        {RIGHT_DIR_RIGHT, sizeof(RIGHT_DIR_RIGHT) - 1},
        {RIGHT_DIR_MOST_INNER, sizeof(RIGHT_DIR_MOST_INNER) - 1},
    };

    const char depth_input [] = INF_STRING;
    char exp_root [PATH_MAX];
    getRealPath(dir_input, exp_root);

    enum CONFIG config_type = AUDIO_CONFIG;

    setUpStubbedInput(dir_input, depth_input);

    addMenu(config_type);

    assertRootRow(database, config_type, exp_root, strlen(exp_root), INF_DEPTH);
    assertPaths(database, exp_paths, sizeof(exp_paths)/sizeof(exp_paths[0]));
}

void testAddMenuLargerDepthThanWhatRootHas(void** state){
    sqlite3* database = *state;
    call_real_function = CALL;
    const char dir_input [] = ROOT1;
    PathCheck_t exp_paths [] = {
        {LEFT_DIR, sizeof(LEFT_DIR) - 1},
        {LEFT_DIR_LEFT, sizeof(LEFT_DIR_LEFT) - 1},
        {LEFT_DIR_RIGHT, sizeof(LEFT_DIR_RIGHT) - 1},
        {LEFT_DIR_MOST_INNER, sizeof(LEFT_DIR_MOST_INNER) - 1},
        {RIGHT_DIR, sizeof(RIGHT_DIR) - 1},
        {RIGHT_DIR_LEFT, sizeof(RIGHT_DIR_LEFT) - 1},
        {RIGHT_DIR_RIGHT, sizeof(RIGHT_DIR_RIGHT) - 1},
        {RIGHT_DIR_MOST_INNER, sizeof(RIGHT_DIR_MOST_INNER) - 1},
    };

    const char depth_input [] = "27";
    char exp_root [PATH_MAX];
    getRealPath(dir_input, exp_root);

    enum CONFIG config_type = AUDIO_CONFIG;

    setUpStubbedInput(dir_input, depth_input);

    addMenu(config_type);

    assertRootRow(database, config_type, exp_root, strlen(exp_root), atoi(depth_input));
    assertPaths(database, exp_paths, sizeof(exp_paths)/sizeof(exp_paths[0]));
}

void testAddMenuMaxDepth(void** state){
    sqlite3* database = *state;
    call_real_function = CALL;
    const char dir_input [] = ROOT1;
    PathCheck_t exp_paths [] = {
        {LEFT_DIR, sizeof(LEFT_DIR) - 1},
        {LEFT_DIR_LEFT, sizeof(LEFT_DIR_LEFT) - 1},
        {LEFT_DIR_RIGHT, sizeof(LEFT_DIR_RIGHT) - 1},
        {LEFT_DIR_MOST_INNER, sizeof(LEFT_DIR_MOST_INNER) - 1},
        {RIGHT_DIR, sizeof(RIGHT_DIR) - 1},
        {RIGHT_DIR_LEFT, sizeof(RIGHT_DIR_LEFT) - 1},
        {RIGHT_DIR_RIGHT, sizeof(RIGHT_DIR_RIGHT) - 1},
        {RIGHT_DIR_MOST_INNER, sizeof(RIGHT_DIR_MOST_INNER) - 1},
    };

    //set up input for MAX_DEPTH
    int size = snprintf(NULL, 0, "%d", MAX_DEPTH);
    char* depth_input = malloc(size + 1);
    if(depth_input == NULL){
        fail_msg("Not enough memory to allocate depth_input for MAX_DEPTH");
    }
    snprintf(depth_input, size + 1, "%d", MAX_DEPTH);
    char exp_root [PATH_MAX];
    getRealPath(dir_input, exp_root);

    enum CONFIG config_type = AUDIO_CONFIG;

    setUpStubbedInput(dir_input, depth_input);

    addMenu(config_type);

    assertRootRow(database, config_type, exp_root, strlen(exp_root), MAX_DEPTH);
    assertPaths(database, exp_paths, sizeof(exp_paths)/sizeof(exp_paths[0]));
}

void testAddMenuSmallerDepthThanWhatRootHas(void** state){
    sqlite3* database = *state;
    call_real_function = CALL;
    const char dir_input [] = ROOT1;
    PathCheck_t exp_paths [] = {
        {LEFT_DIR, sizeof(LEFT_DIR) - 1},
        {LEFT_DIR_LEFT, sizeof(LEFT_DIR_LEFT) - 1},
        {LEFT_DIR_RIGHT, sizeof(LEFT_DIR_RIGHT) - 1},
        {RIGHT_DIR, sizeof(RIGHT_DIR) - 1},
        {RIGHT_DIR_LEFT, sizeof(RIGHT_DIR_LEFT) - 1},
        {RIGHT_DIR_RIGHT, sizeof(RIGHT_DIR_RIGHT) - 1},
    };

    const char depth_input [] = "2";
    char exp_root [PATH_MAX];
    getRealPath(dir_input, exp_root);

    enum CONFIG config_type = AUDIO_CONFIG;

    setUpStubbedInput(dir_input, depth_input);

    addMenu(config_type);

    assertRootRow(database, config_type, exp_root, strlen(exp_root), atoi(depth_input));
    assertPaths(database, exp_paths, sizeof(exp_paths)/sizeof(exp_paths[0]));
}

void testAddMenuZeroDepthOnRoot(void** state){
    sqlite3* database = *state;
    call_real_function = CALL;
    const char dir_input [] = ROOT1;

    const char depth_input [] = "0";
    char exp_root [PATH_MAX];
    getRealPath(dir_input, exp_root);

    enum CONFIG config_type = AUDIO_CONFIG;

    setUpStubbedInput(dir_input, depth_input);

    addMenu(config_type);

    assertRootRow(database, config_type, exp_root, strlen(exp_root), atoi(depth_input));
    assertPaths(database, NULL, 0);
}

void testAddMenuOneDepthOnRoot(void** state){
    sqlite3* database = *state;
    call_real_function = CALL;
    const char dir_input [] = ROOT1;
    PathCheck_t exp_paths [] = {
        {LEFT_DIR, sizeof(LEFT_DIR) - 1},
        {RIGHT_DIR, sizeof(RIGHT_DIR) - 1},
    };

    const char depth_input [] = "1";
    char exp_root [PATH_MAX];
    getRealPath(dir_input, exp_root);

    enum CONFIG config_type = AUDIO_CONFIG;

    setUpStubbedInput(dir_input, depth_input);

    addMenu(config_type);

    assertRootRow(database, config_type, exp_root, strlen(exp_root), atoi(depth_input));
    assertPaths(database, exp_paths, sizeof(exp_paths)/sizeof(exp_paths[0]));
}


void testAddMenuToBlackList(void** state){
    sqlite3* database = *state;
    call_real_function = CALL;
    const char dir_input [] = ROOT2;
    int exp_depth = 0;

    char exp_root [PATH_MAX];
    getRealPath(dir_input, exp_root);
    enum CONFIG config_type = BLACK_CONFIG;

    expect_function_calls(__wrap_takeDirectoryInput, 1);
    will_return(__wrap_boundedInput, dir_input);

    addMenu(config_type);

    assertRootRow(database, config_type, exp_root, strlen(exp_root), exp_depth);
    assertPaths(database, NULL, 0);
}

void testAddMenuToVideoConfig(void** state){
    sqlite3* database = *state;
    call_real_function = CALL;
    const char dir_input [] = ROOT1;
    PathCheck_t exp_paths [] = {
        {LEFT_DIR, sizeof(LEFT_DIR) - 1},
        {LEFT_DIR_LEFT, sizeof(LEFT_DIR_LEFT) - 1},
        {LEFT_DIR_RIGHT, sizeof(LEFT_DIR_RIGHT) - 1},
        {LEFT_DIR_MOST_INNER, sizeof(LEFT_DIR_MOST_INNER) - 1},
        {RIGHT_DIR, sizeof(RIGHT_DIR) - 1},
        {RIGHT_DIR_LEFT, sizeof(RIGHT_DIR_LEFT) - 1},
        {RIGHT_DIR_RIGHT, sizeof(RIGHT_DIR_RIGHT) - 1},
        {RIGHT_DIR_MOST_INNER, sizeof(RIGHT_DIR_MOST_INNER) - 1},
    };

    const char depth_input [] = INF_STRING;
    char exp_root [PATH_MAX];
    getRealPath(dir_input, exp_root);

    enum CONFIG config_type = VIDEO_CONFIG;

    setUpStubbedInput(dir_input, depth_input);

    addMenu(config_type);

    assertRootRow(database, config_type, exp_root, strlen(exp_root), INF_DEPTH);
    assertPaths(database, exp_paths, sizeof(exp_paths)/sizeof(exp_paths[0]));
}

void testAddMenuToCoverConfig(void** state){
    sqlite3* database = *state;
    call_real_function = CALL;
    const char dir_input [] = ROOT1;
    PathCheck_t exp_paths [] = {
        {LEFT_DIR, sizeof(LEFT_DIR) - 1},
        {LEFT_DIR_LEFT, sizeof(LEFT_DIR_LEFT) - 1},
        {LEFT_DIR_RIGHT, sizeof(LEFT_DIR_RIGHT) - 1},
        {LEFT_DIR_MOST_INNER, sizeof(LEFT_DIR_MOST_INNER) - 1},
        {RIGHT_DIR, sizeof(RIGHT_DIR) - 1},
        {RIGHT_DIR_LEFT, sizeof(RIGHT_DIR_LEFT) - 1},
        {RIGHT_DIR_RIGHT, sizeof(RIGHT_DIR_RIGHT) - 1},
        {RIGHT_DIR_MOST_INNER, sizeof(RIGHT_DIR_MOST_INNER) - 1},
    };

    const char depth_input [] = INF_STRING;
    char exp_root [PATH_MAX];
    getRealPath(dir_input, exp_root);

    enum CONFIG config_type = COVER_CONFIG;

    setUpStubbedInput(dir_input, depth_input);

    addMenu(config_type);

    assertRootRow(database, config_type, exp_root, strlen(exp_root), INF_DEPTH);
    assertPaths(database, exp_paths, sizeof(exp_paths)/sizeof(exp_paths[0]));}

void testAddMenuDuplicateNameButDiffConfigType(void** state){
    sqlite3* database = *state;
    call_real_function = CALL;
    const char dir_input [] = ROOT1;
    PathCheck_t exp_paths [] = {
        {LEFT_DIR, sizeof(LEFT_DIR) - 1},
        {LEFT_DIR_LEFT, sizeof(LEFT_DIR_LEFT) - 1},
        {LEFT_DIR_RIGHT, sizeof(LEFT_DIR_RIGHT) - 1},
        {RIGHT_DIR, sizeof(RIGHT_DIR) - 1},
        {RIGHT_DIR_LEFT, sizeof(RIGHT_DIR_LEFT) - 1},
        {RIGHT_DIR_RIGHT, sizeof(RIGHT_DIR_RIGHT) - 1},
    };

    const char depth_input [] = "2";
    char exp_root [PATH_MAX];
    getRealPath(dir_input, exp_root);

    enum CONFIG config_type = VIDEO_CONFIG;

    setUpStubbedInput(dir_input, depth_input);
    addExtraEntry(database, AUDIO_CONFIG, dir_input);

    addMenu(config_type);

    assertRootRow(database, config_type, exp_root, strlen(exp_root), atoi(depth_input));
    assertPaths(database, exp_paths, sizeof(exp_paths)/sizeof(exp_paths[0]));

}
