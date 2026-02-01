#include "../includes/refresh_db_integration_test.h"

//based on what the Makefile added
static const char* constant_paths [] = {"1", "2", "3"};
static const char* added_paths [] = {"1", "2", "3", "4"};
static const char* removed_paths [] = {"1", "2"};

static const Root_Init_t ROOTS [] = {
    {AUDIO_CONFIG, "constant"},
    {VIDEO_CONFIG, "added_stuff"},
    {VIDEO_CONFIG, "removed_stuff"},
    {BLACK_CONFIG, "blacklist"},
    {COVER_CONFIG, "gone"},
    {BLACK_CONFIG, "removed_black_list"}
};

static const Exp_Root_Info_t EXP_RESULTS []= {
    {ROOTS[0], 3, constant_paths},
    {ROOTS[1], 4, added_paths},
    {ROOTS[2], 2, removed_paths},
    {ROOTS[3], 0, NULL},
};

static const int NUM_OLD_ROOTS = 6;
static int root_len = 0;

static int findExpPath(const char* act_string, const Exp_Root_Info_t* exp_info){
    int is_found = NOT_FOUND;
    for(int i = 0; i < exp_info->num_of_paths && is_found == NOT_FOUND; ++i){
        if(strcmp(act_string, exp_info->paths[i]) == 0){
            is_found = i;
        }
    }

    return is_found;
}

static int indexOfExpRoot(const char* act_string){
    int is_found_at = NOT_FOUND;
    for(int i = 0; i < EXP_ROOT_COUNT && is_found_at == NOT_FOUND; ++i){
        if(strcmp(act_string + root_len + 1, EXP_RESULTS[i].root_entry.dir_name) == 0){
            is_found_at = i;
        }
    }

    return is_found_at;
}

static void assertRows(sqlite3* database){
    char sql_root_rows [] = "SELECT root_name, root_type, root_id FROM Roots";
    sqlite3_stmt* roots_statement = NULL;
    int ret_code = sqlite3_prepare_v2(database, sql_root_rows, -1, &roots_statement, NULL);
    if(ret_code != SQLITE_OK){
        fail_msg("Failed to check data due to %s", sqlite3_errmsg(database));
    }

    while((ret_code = sqlite3_step(roots_statement)) == SQLITE_ROW){
        char* act_root_name = (char*)sqlite3_column_text(roots_statement, 0);

        int index = indexOfExpRoot(act_root_name);
        assert_int_not_equal(index, NOT_FOUND);

        int act_type = sqlite3_column_int(roots_statement, 1);

        assert_int_equal(act_type, EXP_RESULTS[index].root_entry.type);

        char sql_path_rows [] = "SELECT path_name FROM Paths WHERE root_id = ?";
        sqlite3_stmt* paths_statement = NULL;
        ret_code = sqlite3_prepare_v2(database, sql_path_rows, -1, &paths_statement, NULL);
        if(ret_code != SQLITE_OK){
            fail_msg("Failed to prepare paths due to %s", sqlite3_errmsg(database));
        }

        int root_id = sqlite3_column_int(roots_statement, 2);
        if(sqlite3_bind_int(paths_statement, 1, root_id) != SQLITE_OK){
            fail_msg("Failed to bind path due to %s", sqlite3_errmsg(database));
        }

        while((ret_code = sqlite3_step(paths_statement)) == SQLITE_ROW){
            char* act_path_name = (char*)sqlite3_column_text(paths_statement, 0);
            assert_int_not_equal(findExpPath(act_path_name, &EXP_RESULTS[index]), NOT_FOUND);
        }
    }
}

static void assertCount(sqlite3* database){
    char sql_count [] = "SELECT (SELECT COUNT(root_id) FROM Roots), (SELECT COUNT(path_id) FROM Paths)";
    sqlite3_stmt* count_statement = NULL;

    int ret_code = sqlite3_prepare_v2(database, sql_count, -1, &count_statement, NULL);
    if(ret_code != SQLITE_OK){
        fail_msg("Failed to check counts due to %s", sqlite3_errmsg(database));
    }

    ret_code = sqlite3_step(count_statement);
    if(ret_code == SQLITE_ROW){
        int act_root_count = sqlite3_column_int(count_statement, 0);
        int act_path_count = sqlite3_column_int(count_statement, 1);

        assert_int_equal(act_root_count, EXP_ROOT_COUNT);
        assert_int_equal(act_path_count, EXP_PATH_COUNT);
    }else{
        fail_msg("No count information available");
    }
}

static void setUpData(sqlite3* database){
    const char root_format [] =
        "INSERT INTO Roots (root_type, root_name, root_length, root_depth)"
        "VALUES"
        "(?, ?, ?, 5)"
        "RETURNING root_id;";

    const char path_format [] =
        "INSERT INTO Paths (root_id, path_name, path_length)"
        "VALUES"
        "(?, '1', 1),"
        "(?, '2', 1),"
        "(?, '3', 1)";

    char abs_path_to_env [PATH_MAX];
    if(realpath(DIR_ENV, abs_path_to_env) == NULL){
        fail_msg("Could not get real path to dir environment\n");
    }
    root_len = strlen(abs_path_to_env);

    for(int i = 0; i < NUM_OLD_ROOTS; ++i){
        char abs_path_to_dir [PATH_MAX];
        int len = snprintf(abs_path_to_dir, sizeof(abs_path_to_dir), "%s/%s", abs_path_to_env, ROOTS[i].dir_name);
        if(len >= PATH_MAX){
            fail_msg("Absolute path to dir environment is too large for %s/%s", abs_path_to_dir, ROOTS[i].dir_name);
        }

        sqlite3_stmt* insert_root_statement = NULL;
        int ret_code = sqlite3_prepare_v2(database, root_format, -1, &insert_root_statement, NULL);
        if(ret_code != SQLITE_OK){
            fail_msg("Failed to prepare root due to %s", sqlite3_errmsg(database));
        }

        if(
            sqlite3_bind_int(insert_root_statement, 1, ROOTS[i].type) != SQLITE_OK ||
            sqlite3_bind_text(insert_root_statement, 2, abs_path_to_dir, len + 1, NULL) != SQLITE_OK ||
            sqlite3_bind_int(insert_root_statement, 3, strlen(abs_path_to_dir)) != SQLITE_OK
        ){
            fail_msg("Failed to bind root params due to %s", sqlite3_errmsg(database));
        }

        if(sqlite3_step(insert_root_statement) != SQLITE_ROW){
            fail_msg("Failed to add root due to %s", sqlite3_errmsg(database));
        }

        if(ROOTS[i].type != BLACK_CONFIG){
            int root_id = sqlite3_column_int(insert_root_statement, 0);

            sqlite3_stmt* insert_path_statement = NULL;
            ret_code = sqlite3_prepare_v2(database, path_format, -1, &insert_path_statement, NULL);
            if(ret_code != SQLITE_OK){
                fail_msg("Failed to prepare path due to %s", sqlite3_errmsg(database));
            }

            if(
                sqlite3_bind_int(insert_path_statement, 1, root_id) != SQLITE_OK ||
                sqlite3_bind_int(insert_path_statement, 2, root_id) != SQLITE_OK ||
                sqlite3_bind_int(insert_path_statement, 3, root_id) != SQLITE_OK
            ){
                fail_msg("Failed to bind path params due to %s", sqlite3_errmsg(database));
            }

            if(sqlite3_step(insert_path_statement) != SQLITE_DONE){
                fail_msg("Failed to add path due to %s", sqlite3_errmsg(database));
            }
        }
    }
}

void testRefreshDatabase(void** state){
    sqlite3* database = *state;
    setUpData(database);

    int exp_ret = NO_ERROR;

    int act_ret = refreshDatabase();

    assert_true(act_ret == exp_ret);
    assertCount(database);
    assertRows(database);
}
