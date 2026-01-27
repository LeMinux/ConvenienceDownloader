#include "../test_include/delete_menu_unit_test.h"

static void assertDeleteData(sqlite3* database, int root_id, int exp_remaining){
    char sql_check_total [] =
        "SELECT (SELECT COUNT(root_id) FROM Roots WHERE root_id = ?) AS root_count,"
        "(SELECT COUNT(path_index) FROM Paths WHERE root_id = ?) AS path_count,"
        "((SELECT COUNT(root_id) FROM Roots) + (SELECT COUNT(path_index) FROM Paths)) AS remaining;";

    sqlite3_stmt* total_statement = NULL;

    int ret_code = sqlite3_prepare_v2(database, sql_check_total, -1, &total_statement, NULL);
    if(ret_code != SQLITE_OK){
        fail_msg("Failed to prepare total check due to %s", sqlite3_errmsg(database));
    }

    if(
        sqlite3_bind_int(total_statement, 1, root_id) != SQLITE_OK ||
        sqlite3_bind_int(total_statement, 2, root_id) != SQLITE_OK
    ){
        fail_msg("Failed to bind data due to %s", sqlite3_errmsg(database));
    }

    //check if only the necessary rows got deleted
    ret_code = sqlite3_step(total_statement);
    if(ret_code == SQLITE_ROW){
        int total_root_rows = sqlite3_column_int(total_statement, 0);
        int total_path_rows = sqlite3_column_int(total_statement, 1);
        int total_remaining = sqlite3_column_int(total_statement, 2);
        assert_int_equal(total_root_rows, 0);
        assert_int_equal(total_path_rows, 0);
        assert_int_equal(total_remaining, exp_remaining);
    }else{
        fail_msg("Could not determine total delete count");
    }
}

void testDeleteEntryCatchesInvalidIndex(void** state){
    (void) state;
    int index_input = INVALID;
    enum CONFIG config_type = AUDIO_CONFIG;

    expect_function_calls(__wrap_takeIndexInput, 2);
    will_return(__wrap_takeIndexInput, index_input);

    deleteMenu(config_type);
}

void testDeleteEntryAudioConfig(void** state){
    sqlite3* database = *state;
    int index_input = 1;
    enum CONFIG config_type = AUDIO_CONFIG;

    expect_function_calls(__wrap_takeIndexInput, 1);
    will_return(__wrap_takeIndexInput, index_input);

    deleteMenu(config_type);

    assertDeleteData(database, JAZZY_ROOT_ID, TOTAL_ROWS - 3);
}

void testDeleteEntryVideoConfig(void** state){
    sqlite3* database = *state;
    int index_input = 1;
    enum CONFIG config_type = VIDEO_CONFIG;

    expect_function_calls(__wrap_takeIndexInput, 1);
    will_return(__wrap_takeIndexInput, index_input);

    deleteMenu(config_type);

    assertDeleteData(database, KEEPS_ROOT_ID, TOTAL_ROWS - 3);
}

void testDeleteEntryCoverConfig(void** state){
    sqlite3* database = *state;
    int index_input = 1;
    enum CONFIG config_type = COVER_CONFIG;

    expect_function_calls(__wrap_takeIndexInput, 1);
    will_return(__wrap_takeIndexInput, index_input);

    deleteMenu(config_type);
    listAllRootWithPaths();

    assertDeleteData(database, ARTS_ROOT_ID, TOTAL_ROWS - 3);
}

void testDeleteEntryBlackList(void** state){
    sqlite3* database = *state;
    int index_input = 1;
    enum CONFIG config_type = BLACK_CONFIG;

    expect_function_calls(__wrap_takeIndexInput, 1);
    will_return(__wrap_takeIndexInput, index_input);

    deleteMenu(config_type);

    assertDeleteData(database, PERSONAL_ROOT_ID, TOTAL_ROWS - 1);
}
