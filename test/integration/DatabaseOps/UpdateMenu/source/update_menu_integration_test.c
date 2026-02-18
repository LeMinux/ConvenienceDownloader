#include "../includes/update_menu_integration_test.h"

static void getRealPath(const char* input, char result [PATH_MAX]){
    if(realpath(input, result) == NULL){
        fail_msg("Could not create real path");
    }
}

static void setUpStubbedInput(int index_input, int depth_input){
    expect_function_calls(__wrap_takeIndexInput, 1);
    expect_function_calls(__wrap_takeDepthInput, 1);

    will_return(__wrap_takeIndexInput, index_input);
    will_return(__wrap_takeDepthInput, depth_input);
}

static int assertRootRow(sqlite3* database, enum CONFIG exp_type, const char* exp_input, size_t exp_input_len, int exp_depth){
    char sql_check [] = "SELECT root_name, root_length, root_depth, root_id FROM Roots WHERE root_name = ? AND root_type = ?";
    sqlite3_stmt* statement = NULL;
    int ret_code = sqlite3_prepare_v2(database, sql_check, -1, &statement, NULL);

    if(ret_code != SQLITE_OK){
        fail_msg("Failed to check data due to %s", sqlite3_errmsg(database));
    }

    if(sqlite3_bind_text(statement, 1, exp_input, exp_input_len + 1, NULL) != SQLITE_OK ||
        sqlite3_bind_int(statement, 2, exp_type) != SQLITE_OK){
        fail_msg("Failed to bind data due to %s", sqlite3_errmsg(database));
    }

    int update_count = 0;
    int root_id = 0;
    while((ret_code = sqlite3_step(statement)) == SQLITE_ROW){
        ++update_count;
        if(update_count > 1){
            sqlite3_finalize(statement);
            fail_msg("Updated too many root rows. Check the SQL statement\n");
        }
        char* added_string = (char*)sqlite3_column_text(statement, 0);
        int root_length = sqlite3_column_int(statement, 1);
        int root_depth = sqlite3_column_int(statement, 2);

        assert_string_equal(added_string, exp_input);
        assert_int_equal(root_length, exp_input_len);
        assert_int_equal(root_depth, exp_depth);

        root_id = sqlite3_column_int(statement, 3);
    }

    if(update_count == 0){
        sqlite3_finalize(statement);
        fail_msg("Did not update anything\n", exp_input);
    }

    sqlite3_finalize(statement);
    return root_id;
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
static void assertPaths(sqlite3* database, int exp_id, PathCheck_t* exp_path_names, int exp_count){
    char sql_count [] = "SELECT COUNT(path_id) FROM Paths WHERE root_id = ?";
    char sql_get_paths [] = "SELECT path_name, path_length FROM Paths WHERE root_id = ?";
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

    if(
        sqlite3_bind_int(count_statement, 1, exp_id) != SQLITE_OK ||
        sqlite3_bind_int(paths_statement, 1, exp_id) != SQLITE_OK
    ){
        fail_msg("Could not bind parameters for path assertion");
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
        sqlite3_finalize(paths_statement);
    }else{
        sqlite3_finalize(count_statement);
        fail_msg("Could not get count of paths");
    }
    sqlite3_finalize(count_statement);
}

void testUpdateMenuCatchesNoRows(void** state){
    (void) state;
    
    enum CONFIG config_type = AUDIO_CONFIG;

    expect_function_calls(__wrap_takeIndexInput, 0);

    updateMenu(config_type);
}

void testUpdateMenuCatchesInvalidIndex(void** state){
    (void) state;
    int index_input = INVALID;
    enum CONFIG config_type = AUDIO_CONFIG;

    expect_function_calls(__wrap_takeIndexInput, 2);
    will_return(__wrap_takeIndexInput, index_input);

    updateMenu(config_type);
}

void testUpdateMenuCatchesInvalidDepth(void** state){
    (void) state;
    int index_input = 1;
    int depth_input = INVALID;
    enum CONFIG config_type = AUDIO_CONFIG;

    expect_function_calls(__wrap_takeIndexInput, 1);
    expect_function_calls(__wrap_takeDepthInput, 2);

    will_return(__wrap_takeIndexInput, index_input);
    will_return(__wrap_takeDepthInput, depth_input);

    updateMenu(config_type);
}

void testUpdateMenuCatchesSkippingIndex(void** state){
    (void) state;
    int index_input = SKIPPING;
    enum CONFIG config_type = AUDIO_CONFIG;

    expect_function_calls(__wrap_takeIndexInput, 1);
    will_return(__wrap_takeIndexInput, index_input);

    updateMenu(config_type);
}

void testUpdateMenuCatchesRootIsDeleted(void** state){
    sqlite3* database = *state;
    const int index_input = 1;
    const int depth_input = 3;
    const char exp_root [] = "I/Dont/Exist/Anymore/Bahahaha";
    enum CONFIG config_type = COVER_CONFIG;

    addExtraRootEntry(database, config_type, exp_root, 5);
    setUpStubbedInput(index_input, depth_input);

    updateMenu(config_type);

    char sql_deleted [] =
        "SELECT (SELECT COUNT(root_id) FROM Roots WHERE root_id = ?) AS root_count,"
        "(SELECT COUNT(path_id) FROM Paths WHERE root_id = ?) AS path_count;";

    sqlite3_stmt* total_statement = NULL;
    int ret_code = sqlite3_prepare_v2(database, sql_deleted, -1, &total_statement, NULL);
    if(ret_code != SQLITE_OK){
        fail_msg("Failed to deleteing %s", sqlite3_errmsg(database));
    }

    if(
        sqlite3_bind_int(total_statement, 1, 1) != SQLITE_OK ||
        sqlite3_bind_int(total_statement, 2, 1) != SQLITE_OK
    ){
        fail_msg("Failed to bind data due to %s", sqlite3_errmsg(database));
    }

    ret_code = sqlite3_step(total_statement);
    if(ret_code == SQLITE_ROW){
        int total_root_rows = sqlite3_column_int(total_statement, 0);
        int total_path_rows = sqlite3_column_int(total_statement, 1);
        assert_int_equal(total_root_rows, 0);
        assert_int_equal(total_path_rows, 0);
    }else{
        sqlite3_finalize(total_statement);
        fail_msg("Could not determine if updating deleted");
    }
    sqlite3_finalize(total_statement);
}

void testUpdateMenuSkippingDepth(void** state){
    sqlite3* database = *state;
    const int index_input = 1;
    const int depth_input = SKIPPING;
    PathCheck_t exp_paths [] = {
        {ROOT_ITSELF, sizeof(ROOT_ITSELF) - 1},
        {LEFT_DIR, sizeof(LEFT_DIR) - 1},
        {LEFT_DIR_LEFT, sizeof(LEFT_DIR_LEFT) - 1},
        {LEFT_DIR_RIGHT, sizeof(LEFT_DIR_RIGHT) - 1},
        {RIGHT_DIR, sizeof(RIGHT_DIR) - 1},
        {RIGHT_DIR_LEFT, sizeof(RIGHT_DIR_LEFT) - 1},
        {RIGHT_DIR_RIGHT, sizeof(RIGHT_DIR_RIGHT) - 1},
    };

    char exp_root [PATH_MAX];
    getRealPath(ROOT1, exp_root);

    enum CONFIG config_type = AUDIO_CONFIG;

    addExtraRootEntry(database, config_type, exp_root, 2);
    setUpStubbedInput(index_input, depth_input);

    updateMenu(config_type);

    int root_id = assertRootRow(database, config_type, exp_root, strlen(exp_root), 2);
    assertPaths(database, root_id, exp_paths, sizeof(exp_paths)/sizeof(exp_paths[0]));
}

void testUpdateMenuEnterInfInputOnRootDir(void** state){
    sqlite3* database = *state;
    const int index_input = 1;
    const int depth_input = INF_DEPTH;
    PathCheck_t exp_paths [] = {
        {ROOT_ITSELF, sizeof(ROOT_ITSELF) - 1},
        {LEFT_DIR, sizeof(LEFT_DIR) - 1},
        {LEFT_DIR_LEFT, sizeof(LEFT_DIR_LEFT) - 1},
        {LEFT_DIR_RIGHT, sizeof(LEFT_DIR_RIGHT) - 1},
        {LEFT_DIR_MOST_INNER, sizeof(LEFT_DIR_MOST_INNER) - 1},
        {RIGHT_DIR, sizeof(RIGHT_DIR) - 1},
        {RIGHT_DIR_LEFT, sizeof(RIGHT_DIR_LEFT) - 1},
        {RIGHT_DIR_RIGHT, sizeof(RIGHT_DIR_RIGHT) - 1},
        {RIGHT_DIR_MOST_INNER, sizeof(RIGHT_DIR_MOST_INNER) - 1},
    };

    char exp_root [PATH_MAX];
    getRealPath(ROOT1, exp_root);

    enum CONFIG config_type = AUDIO_CONFIG;

    addExtraRootEntry(database, config_type, exp_root, 5);
    setUpStubbedInput(index_input, depth_input);

    updateMenu(config_type);

    int root_id = assertRootRow(database, config_type, exp_root, strlen(exp_root), INF_DEPTH);
    assertPaths(database, root_id, exp_paths, sizeof(exp_paths)/sizeof(exp_paths[0]));
}

void testUpdateMenuLargerDepthThanWhatRootHas(void** state){
    sqlite3* database = *state;
    const int index_input = 1;
    const int depth_input = 27;
    PathCheck_t exp_paths [] = {
        {ROOT_ITSELF, sizeof(ROOT_ITSELF) - 1},
        {LEFT_DIR, sizeof(LEFT_DIR) - 1},
        {LEFT_DIR_LEFT, sizeof(LEFT_DIR_LEFT) - 1},
        {LEFT_DIR_RIGHT, sizeof(LEFT_DIR_RIGHT) - 1},
        {LEFT_DIR_MOST_INNER, sizeof(LEFT_DIR_MOST_INNER) - 1},
        {RIGHT_DIR, sizeof(RIGHT_DIR) - 1},
        {RIGHT_DIR_LEFT, sizeof(RIGHT_DIR_LEFT) - 1},
        {RIGHT_DIR_RIGHT, sizeof(RIGHT_DIR_RIGHT) - 1},
        {RIGHT_DIR_MOST_INNER, sizeof(RIGHT_DIR_MOST_INNER) - 1},
    };

    char exp_root [PATH_MAX];
    getRealPath(ROOT1, exp_root);

    enum CONFIG config_type = AUDIO_CONFIG;

    addExtraRootEntry(database, config_type, exp_root, 5);
    setUpStubbedInput(index_input, depth_input);

    updateMenu(config_type);

    int root_id = assertRootRow(database, config_type, exp_root, strlen(exp_root), depth_input);
    assertPaths(database, root_id, exp_paths, sizeof(exp_paths)/sizeof(exp_paths[0]));
}

void testUpdateMenuMaxDepth(void** state){
    sqlite3* database = *state;
    const int index_input = 1;
    const int depth_input = MAX_DEPTH;
    PathCheck_t exp_paths [] = {
        {ROOT_ITSELF, sizeof(ROOT_ITSELF) - 1},
        {LEFT_DIR, sizeof(LEFT_DIR) - 1},
        {LEFT_DIR_LEFT, sizeof(LEFT_DIR_LEFT) - 1},
        {LEFT_DIR_RIGHT, sizeof(LEFT_DIR_RIGHT) - 1},
        {LEFT_DIR_MOST_INNER, sizeof(LEFT_DIR_MOST_INNER) - 1},
        {RIGHT_DIR, sizeof(RIGHT_DIR) - 1},
        {RIGHT_DIR_LEFT, sizeof(RIGHT_DIR_LEFT) - 1},
        {RIGHT_DIR_RIGHT, sizeof(RIGHT_DIR_RIGHT) - 1},
        {RIGHT_DIR_MOST_INNER, sizeof(RIGHT_DIR_MOST_INNER) - 1},
    };

    char exp_root [PATH_MAX];
    getRealPath(ROOT1, exp_root);

    enum CONFIG config_type = AUDIO_CONFIG;

    addExtraRootEntry(database, config_type, exp_root, 5);
    setUpStubbedInput(index_input, depth_input);

    updateMenu(config_type);

    int root_id = assertRootRow(database, config_type, exp_root, strlen(exp_root), depth_input);
    assertPaths(database, root_id, exp_paths, sizeof(exp_paths)/sizeof(exp_paths[0]));
}

void testUpdateMenuSmallerDepthThanWhatRootHas(void** state){
    sqlite3* database = *state;
    const int index_input = 1;
    const int depth_input = 2;
    PathCheck_t exp_paths [] = {
        {ROOT_ITSELF, sizeof(ROOT_ITSELF) - 1},
        {LEFT_DIR, sizeof(LEFT_DIR) - 1},
        {LEFT_DIR_LEFT, sizeof(LEFT_DIR_LEFT) - 1},
        {LEFT_DIR_RIGHT, sizeof(LEFT_DIR_RIGHT) - 1},
        {RIGHT_DIR, sizeof(RIGHT_DIR) - 1},
        {RIGHT_DIR_LEFT, sizeof(RIGHT_DIR_LEFT) - 1},
        {RIGHT_DIR_RIGHT, sizeof(RIGHT_DIR_RIGHT) - 1},
    };

    char exp_root [PATH_MAX];
    getRealPath(ROOT1, exp_root);

    enum CONFIG config_type = AUDIO_CONFIG;

    addExtraRootEntry(database, config_type, exp_root, 5);
    setUpStubbedInput(index_input, depth_input);

    updateMenu(config_type);

    int root_id = assertRootRow(database, config_type, exp_root, strlen(exp_root), depth_input);
    assertPaths(database, root_id, exp_paths, sizeof(exp_paths)/sizeof(exp_paths[0]));
}

void testUpdateMenuZeroDepthOnRoot(void** state){
    sqlite3* database = *state;
    const int index_input = 1;
    const int depth_input = 0;
    PathCheck_t exp_paths [] = {
        {ROOT_ITSELF, sizeof(ROOT_ITSELF) - 1},
    };

    char exp_root [PATH_MAX];
    getRealPath(ROOT1, exp_root);

    enum CONFIG config_type = AUDIO_CONFIG;

    addExtraRootEntry(database, config_type, exp_root, 5);
    setUpStubbedInput(index_input, depth_input);

    updateMenu(config_type);

    int root_id = assertRootRow(database, config_type, exp_root, strlen(exp_root), depth_input);
    assertPaths(database, root_id, exp_paths, sizeof(exp_paths)/sizeof(exp_paths[0]));
}

void testUpdateMenuOneDepthOnRoot(void** state){
    sqlite3* database = *state;
    const int index_input = 1;
    const int depth_input = 1;
    PathCheck_t exp_paths [] = {
        {ROOT_ITSELF, sizeof(ROOT_ITSELF) - 1},
        {LEFT_DIR, sizeof(LEFT_DIR) - 1},
        {RIGHT_DIR, sizeof(RIGHT_DIR) - 1},
    };

    char exp_root [PATH_MAX];
    getRealPath(ROOT1, exp_root);

    enum CONFIG config_type = AUDIO_CONFIG;

    addExtraRootEntry(database, config_type, exp_root, 5);
    setUpStubbedInput(index_input, depth_input);

    updateMenu(config_type);

    int root_id = assertRootRow(database, config_type, exp_root, strlen(exp_root), depth_input);
    assertPaths(database, root_id, exp_paths, sizeof(exp_paths)/sizeof(exp_paths[0]));
}

void testUpdateMenuToVideoConfig(void** state){
    sqlite3* database = *state;
    const int index_input = 1;
    const int depth_input = INF_DEPTH;
    PathCheck_t exp_paths [] = {
        {ROOT_ITSELF, sizeof(ROOT_ITSELF) - 1},
        {LEFT_DIR, sizeof(LEFT_DIR) - 1},
        {LEFT_DIR_LEFT, sizeof(LEFT_DIR_LEFT) - 1},
        {LEFT_DIR_RIGHT, sizeof(LEFT_DIR_RIGHT) - 1},
        {LEFT_DIR_MOST_INNER, sizeof(LEFT_DIR_MOST_INNER) - 1},
        {RIGHT_DIR, sizeof(RIGHT_DIR) - 1},
        {RIGHT_DIR_LEFT, sizeof(RIGHT_DIR_LEFT) - 1},
        {RIGHT_DIR_RIGHT, sizeof(RIGHT_DIR_RIGHT) - 1},
        {RIGHT_DIR_MOST_INNER, sizeof(RIGHT_DIR_MOST_INNER) - 1},
    };

    char exp_root [PATH_MAX];
    getRealPath(ROOT1, exp_root);

    enum CONFIG config_type = VIDEO_CONFIG;

    addExtraRootEntry(database, config_type, exp_root, 5);
    setUpStubbedInput(index_input, depth_input);

    updateMenu(config_type);

    int root_id = assertRootRow(database, config_type, exp_root, strlen(exp_root), INF_DEPTH);
    assertPaths(database, root_id, exp_paths, sizeof(exp_paths)/sizeof(exp_paths[0]));
}

void testUpdateMenuToCoverConfig(void** state){
    sqlite3* database = *state;
    const int index_input = 1;
    const int depth_input = INF_DEPTH;
    PathCheck_t exp_paths [] = {
        {ROOT_ITSELF, sizeof(ROOT_ITSELF) - 1},
        {LEFT_DIR, sizeof(LEFT_DIR) - 1},
        {LEFT_DIR_LEFT, sizeof(LEFT_DIR_LEFT) - 1},
        {LEFT_DIR_RIGHT, sizeof(LEFT_DIR_RIGHT) - 1},
        {LEFT_DIR_MOST_INNER, sizeof(LEFT_DIR_MOST_INNER) - 1},
        {RIGHT_DIR, sizeof(RIGHT_DIR) - 1},
        {RIGHT_DIR_LEFT, sizeof(RIGHT_DIR_LEFT) - 1},
        {RIGHT_DIR_RIGHT, sizeof(RIGHT_DIR_RIGHT) - 1},
        {RIGHT_DIR_MOST_INNER, sizeof(RIGHT_DIR_MOST_INNER) - 1},
    };

    char exp_root [PATH_MAX];
    getRealPath(ROOT1, exp_root);

    enum CONFIG config_type = COVER_CONFIG;

    addExtraRootEntry(database, config_type, exp_root, 5);
    setUpStubbedInput(index_input, depth_input);

    updateMenu(config_type);

    int root_id = assertRootRow(database, config_type, exp_root, strlen(exp_root), INF_DEPTH);
    assertPaths(database, root_id, exp_paths, sizeof(exp_paths)/sizeof(exp_paths[0]));
}

void testUpdateMenuPathIsInBlackList(void** state){
    sqlite3* database = *state;
    const int index_input = 1;
    const int depth_input = INF_DEPTH;
    const char black_listed_path [] = ROOT1 LEFT_DIR;
    PathCheck_t exp_paths [] = {
        {ROOT_ITSELF, sizeof(ROOT_ITSELF) - 1},
        {RIGHT_DIR, sizeof(RIGHT_DIR) - 1},
        {RIGHT_DIR_LEFT, sizeof(RIGHT_DIR_LEFT) - 1},
        {RIGHT_DIR_RIGHT, sizeof(RIGHT_DIR_RIGHT) - 1},
        {RIGHT_DIR_MOST_INNER, sizeof(RIGHT_DIR_MOST_INNER) - 1},
    };

    char exp_root [PATH_MAX];
    char black_path [PATH_MAX];
    getRealPath(ROOT1, exp_root);
    getRealPath(black_listed_path, black_path);

    enum CONFIG config_type = VIDEO_CONFIG;

    setUpStubbedInput(index_input, depth_input);
    addExtraRootEntry(database, BLACK_CONFIG, black_path, 0);
    addExtraRootEntry(database, config_type, exp_root, 5);

    updateMenu(config_type);

    int root_id = assertRootRow(database, config_type, exp_root, strlen(exp_root), INF_DEPTH);
    assertPaths(database, root_id, exp_paths, sizeof(exp_paths)/sizeof(exp_paths[0]));
}
