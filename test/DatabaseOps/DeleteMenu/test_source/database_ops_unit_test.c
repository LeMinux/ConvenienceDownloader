#include "../test_include/database_ops_unit_test.h"

//sizeof this will be used which would be the length of the raw string
//numbers used shouldn't be beyond two digits so it should be fine
static const char sql_root_format [] =
    "INSERT INTO Roots (root_id, root_type, root_name, root_length, root_depth)"
    "VALUES"
    "(1, %d, '/home/user/Music/Rock/', 22, %d),"       //audio
    "(2, %d, '/home/user/Music/Jazzy/', 23, %d),"
    "(3, %d, '/home/user/Videos/MVs/', 22, %d),"       //video
    "(4, %d, '/home/user/Videos/Keeps/', 24, %d),"
    "(5, %d, '/home/user/Pictures/CVs/', 24, 1),"      //covers
    "(6, %d, '/home/user/Pictures/Arts/', 25, 1),"
    "(7, %d, '/home/user/Pictures/Personal/', 29, 0)," //black list
    "(8, %d, '/home/user/Pictures/Sensitives/', 31, 0);";

static const char* sql_paths =
    "INSERT INTO Paths (path_index, root_id, path_name, path_length)"
    "VALUES"
    "(1, 1, 'Men At Work', 11),"
    "(2, 1, 'Rainbow', 7),"
    "(3, 1, 'Motorhead', 9),"
    "(4, 2, 'Big Bad Voodoo Daddy', 20),"
    "(5, 3, 'Youtube', 7),"
    "(6, 3, 'Documentary', 11),"
    "(7, 4, 'Memes', 5);";


static void readScript(const char* file_path, char** sql_init){
    FILE* init_script = fopen(file_path, "rb");
    if(init_script == NULL){
        fail_msg("Could open initDB.sql\n");
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

static void executeQuery(sqlite3* test_db, const char* sql_statement){
    char* error_msg = NULL;
    sqlite3_exec(test_db, sql_statement, NULL, NULL, &error_msg);
    if(error_msg){
        fail_msg("Failed to add entries for reading in memory database:%s\n", error_msg);
    }
}

static void setUpReadOnlyRoots(sqlite3* database){
    char sql_string [sizeof(sql_root_format)];

    //an init script would be better, but if the arbritary numbers for INF_DEPTH
    //and config numbers changed the file would need to manually change
    snprintf(sql_string, sizeof(sql_root_format), sql_root_format,
             AUDIO_CONFIG, INF_DEPTH,
             AUDIO_CONFIG, INF_DEPTH,
             VIDEO_CONFIG, INF_DEPTH,
             VIDEO_CONFIG, INF_DEPTH,
             COVER_CONFIG,
             COVER_CONFIG,
             BLACK_CONFIG,
             BLACK_CONFIG);

    executeQuery(database, sql_string);
}

static void setupReadOnlyPaths(sqlite3* database){
    executeQuery(database, sql_paths);
}


static void assertAddData(sqlite3* database, enum CONFIG exp_type, const char* exp_input, size_t exp_input_len, int exp_depth){
    char sql_check [] = "SELECT root_type, root_name, root_length, root_depth FROM Roots WHERE root_name = ?";
    sqlite3_stmt* statement = NULL;
    int ret_code = sqlite3_prepare_v2(database, sql_check, -1, &statement, NULL);

    if(ret_code != SQLITE_OK){
        fail_msg("Failed to check data due to %s", sqlite3_errmsg(database));
    }

    if(sqlite3_bind_text(statement, 1, exp_input, exp_input_len, NULL) != SQLITE_OK){
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
        readScript("../../source/initDB.sql", &sql_init);
    }

    char* error_msg = NULL;
    sqlite3_exec(database, sql_init, NULL, NULL, &error_msg);
    if(error_msg){
        fail_msg("Failed to create tables for in memory database:%s\n", error_msg);
        return 1;
    }

    setUpReadOnlyRoots(database);
    setupReadOnlyPaths(database);
    __testingSetDB(database);
    *state = database;
    return 0;
}

void testListAllRoots(void **state){
    (void) state;
    int ret = listAllRoots();
    assert(ret == NO_ERROR);
}

void testListRootsForConfig(void **state){
    (void) state;
    int ret = listConfigRoots(AUDIO_CONFIG);
    assert(ret == NO_ERROR);
}

void testListAllRootsWithPaths(void **state){
    (void) state;
    int ret = listAllRootWithPaths();
    assert(ret == NO_ERROR);
}

void testListRootAndPathsForConfig(void **state){
    (void) state;
    int ret = listConfigRootsWithPaths(AUDIO_CONFIG);
    assert(ret == NO_ERROR);
}

int __wrap_takeIndexInput(int max){
    puts("EEEEK");
    function_called();
    if(has_mock()){
        return mock_type(int);
    }else{
        stop();
    }
}

char* __wrap_takeDirectoryInput(void){
    function_called();
    if(has_mock()){
        return mock_type(char*);
    }else{
        stop();
    }
}

//the mock type should be a pointer in the HEAP as sourct implementation uses malloc
//A stack pointer may be attempted to be freed and crash the test
int __wrap_takeDepthInput(){
    function_called();
    if(has_mock()){
        return mock_type(int);
    }else{
        stop();
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

void testAddEntryEnterBothValidInput(void** state){
    sqlite3* database = *state;
    char edit_select [] = {ADD_OPT, '\0'};
    char exit_select [] = {EXT_OPT, '\0'};
    const char input [] = "HaHaTotallyRealDirectory/";
    int depth_input = 7;
    enum CONFIG config_type = AUDIO_CONFIG;

    char* malloc_input = malloc(sizeof(input));
    memcpy(malloc_input, input, sizeof(input));

    expect_function_calls(__wrap_takeDirectoryInput, 1);
    expect_function_calls(__wrap_takeDepthInput, 1);
    will_return(__wrap_boundedInput, edit_select);
    will_return(__wrap_boundedInput, 1);
    will_return(__wrap_takeDirectoryInput, malloc_input);
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

    char* malloc_input = malloc(sizeof(input));
    memcpy(malloc_input, input, sizeof(input));

    expect_function_calls(__wrap_takeDirectoryInput, 1);
    will_return(__wrap_boundedInput, edit_select);
    will_return(__wrap_boundedInput, 1);
    will_return(__wrap_takeDirectoryInput, malloc_input);
    will_return(__wrap_boundedInput, exit_select);
    will_return(__wrap_boundedInput, 1);

    editMenu(config_type);

    assertAddData(database, config_type, input, sizeof(input) - 1, exp_depth);
}

void testAddEntryToVideoConfig(void** state){
    sqlite3* database = *state;
    char edit_select [] = {ADD_OPT, '\0'};
    char exit_select [] = {EXT_OPT, '\0'};
    const char input [] = "HaHaTotallyRealDirectory/";
    int depth_input = 7;
    enum CONFIG config_type = VIDEO_CONFIG;

    char* malloc_input = malloc(sizeof(input));
    memcpy(malloc_input, input, sizeof(input));

    expect_function_calls(__wrap_takeDirectoryInput, 1);
    expect_function_calls(__wrap_takeDepthInput, 1);
    will_return(__wrap_boundedInput, edit_select);
    will_return(__wrap_boundedInput, 1);
    will_return(__wrap_takeDirectoryInput, malloc_input);
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
    const char input [] = "HaHaTotallyRealDirectory/";
    int depth_input = 7;
    enum CONFIG config_type = COVER_CONFIG;

    char* malloc_input = malloc(sizeof(input));
    memcpy(malloc_input, input, sizeof(input));

    expect_function_calls(__wrap_takeDirectoryInput, 1);
    expect_function_calls(__wrap_takeDepthInput, 1);
    will_return(__wrap_boundedInput, edit_select);
    will_return(__wrap_boundedInput, 1);
    will_return(__wrap_takeDirectoryInput, malloc_input);
    will_return(__wrap_takeDepthInput, depth_input);
    will_return(__wrap_boundedInput, exit_select);
    will_return(__wrap_boundedInput, 1);

    editMenu(config_type);

    assertAddData(database, config_type, input, sizeof(input) - 1, depth_input);
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

}

void testUpdateEntryDiffPathAndSameDepth(void** state){

}

void testUpdateEntrySamePathAndDiffDepth(void** state){

}

void testUpdateEntrySamePathAndSameDepth(void** state){

}

void testUpdateBlackList(void** state){

}

void testUpdateVideoConfig(void** state){

}

void testUpdateCoverConfig(void** state){

}
