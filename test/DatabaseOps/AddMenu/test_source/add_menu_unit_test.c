#include "../test_include/add_menu_unit_test.h"

static const char add_dup_format [] =
    "INSERT INTO Roots (root_id, root_type, root_name, root_length, root_depth) VALUES"
    "(99, ?, ?, ?, 4)";

static void readScript(const char* file_path, char** sql_init){
    FILE* init_script = fopen(file_path, "rb");
    if(init_script == NULL){
        fail_msg("Could not open initDB.sql\n");
    }

    int fd = fileno(init_script);
    if(fd < 0){
        fclose(init_script);
        fail_msg("Could not get fd from file");
    }

    struct stat file_stats;
    if (fstat(fd, &file_stats) < 0) {
        fclose(init_script);
        fail_msg("Could not get size info from file");
    }

    int init_size = file_stats.st_size;
    char* sql_text = malloc(init_size + 1);

    if(sql_text == NULL){
        fclose(init_script);
        fail_msg("Could not allocate space to place init script");
    }

    if(fread(sql_text, init_size, 1, init_script) != 1){
        fclose(init_script);
        fail_msg("Could not read init script");
    }

    (void)fclose(init_script);

    sql_text[init_size] = '\0';
    *sql_init = sql_text;
}

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

static void assertAddData(sqlite3* database, enum CONFIG exp_type, const char* exp_input, size_t exp_input_len, int exp_depth){
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
int closeDB(void** state){
    (void)sqlite3_close(*state);
    return 0;
}

int createTestDB(void** state){
    //don't want to be constantly opening the file for initalizing
    //and only this method needs to init
    static char* sql_init = NULL;

    sqlite3* database = NULL;
    int rc = sqlite3_open(TESTING_CONFIG_DB, &database);

    if(rc != SQLITE_OK){
        fail_msg("Unable to create in memory database\n");
        return 1;
    }

    if(sql_init == NULL){
        readScript("../../../source/initDB.sql", &sql_init);
    }

    char* error_msg = NULL;
    sqlite3_exec(database, sql_init, NULL, NULL, &error_msg);
    if(error_msg){
        fail_msg("Failed to create tables for in memory database:%s\n", error_msg);
        return 1;
    }

    __testingSetDB(database);
    *state = database;
    return 0;
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

void testAddEntryEnterBothValidInput(void** state){
    sqlite3* database = *state;
    char edit_select [] = {ADD_OPT, '\0'};
    char exit_select [] = {EXT_OPT, '\0'};
    const char input [] = "HaHaAudioRoot/";
    int depth_input = 7;
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

    assertAddData(database, config_type, input, sizeof(input) - 1, depth_input);
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
