#include "../test_include/read_configs_unit_test.h"

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
