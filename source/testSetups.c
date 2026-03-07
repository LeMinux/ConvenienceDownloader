#include "../includes/testSetups.h"

static const char SQL_ROOT_FORMAT [] =
    "INSERT INTO Roots (root_id, root_type, root_name, root_length, root_depth)"
    "VALUES"
    "(1, %d, '%s', %d, 5),"    //audio
    "(2, %d, '%s', %d, 5),"
    "(3, %d, '%s', %d, 5),"     //video
    "(4, %d, '%s', %d, 5),"
    "(5, %d, '%s', %d, 1),"     //covers
    "(6, %d, '%s', %d, 1),"
    "(7, %d, '%s', %d, 0),"     //black list
    "(8, %d, '%s', %d, 0);";

static const char SQL_PATH_FORMAT [] =
    "INSERT INTO Paths (path_id, root_id, path_name, path_length)"
    "VALUES"
    "(1, 1, '/', 1),"
    "(2, 1, '%s', %d),"
    "(3, 1, '%s', %d),"
    "(4, 2, '/', 1),"
    "(5, 2, '%s', %d),"
    "(6, 2, '%s', %d),"
    "(7, 3, '/', 1),"
    "(8, 3, '%s', %d),"
    "(9, 3, '%s', %d),"
    "(10, 4, '/', 1),"
    "(11, 4, '%s', %d),"
    "(12, 4, '%s', %d),"
    "(13, 5, '/', 1),"
    "(14, 5, '%s', %d),"
    "(15, 5, '%s', %d),"
    "(16, 6, '/', 1),"
    "(17, 6, '%s', %d),"
    "(18, 6, '%s', %d);";


static void readScript(char** sql_init){
    FILE* init_script = fopen(init_db_path, "rb");
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
        fail_msg("Failed to execute query:%s\n", error_msg);
        sqlite3_free(error_msg);
    }
}

void addExtraRootEntry(sqlite3* test_db, enum CONFIG config, const char* extra_name, int depth){
    static const char add_extra_format [] =
        "INSERT INTO Roots (root_type, root_name, root_length, root_depth) VALUES"
        "(?, ?, ?, ?)";

    sqlite3_stmt* statement = NULL;
    int ret_code = sqlite3_prepare_v2(test_db, add_extra_format, -1, &statement, NULL);

    if(ret_code != SQLITE_OK){
        fail_msg("Failed to add extra entry due to %s", sqlite3_errmsg(test_db));
    }

    if(
        sqlite3_bind_int(statement, 1, config) != SQLITE_OK ||
        sqlite3_bind_text(statement, 2, extra_name, strlen(extra_name) + 1, NULL) != SQLITE_OK ||
        sqlite3_bind_int(statement, 3, strlen(extra_name)) != SQLITE_OK ||
        sqlite3_bind_int(statement, 4, depth) != SQLITE_OK
    )
    {
        sqlite3_finalize(statement);
        fail_msg("Failed to bind data for extra root due to %s", sqlite3_errmsg(test_db));
    }

    ret_code = sqlite3_step(statement);
    if(ret_code != SQLITE_DONE){
        sqlite3_finalize(statement);
        fail_msg("Did not add extra entry %s", extra_name);
    }
    sqlite3_finalize(statement);
}

void addExtraPathEntry(sqlite3* test_db, int root_id, const char* extra_path){
    static const char add_extra_format [] =
        "INSERT INTO Paths (root_id, path_name, path_length) VALUES"
        "(?, ?, ?)";

    sqlite3_stmt* statement = NULL;
    int ret_code = sqlite3_prepare_v2(test_db, add_extra_format, -1, &statement, NULL);

    if(ret_code != SQLITE_OK){
        fail_msg("Failed to add extra entry due to %s", sqlite3_errmsg(test_db));
    }

    if(
        sqlite3_bind_int(statement, 1, root_id) != SQLITE_OK ||
        sqlite3_bind_text(statement, 2, extra_path, strlen(extra_path) + 1, NULL) != SQLITE_OK ||
        sqlite3_bind_int(statement, 3, strlen(extra_path)) != SQLITE_OK
    )
    {
        sqlite3_finalize(statement);
        fail_msg("Failed to bind data for extra path due to %s", sqlite3_errmsg(test_db));
    }

    ret_code = sqlite3_step(statement);
    if(ret_code != SQLITE_DONE){
        sqlite3_finalize(statement);
        fail_msg("Did not add extra entry %s", extra_path);
    }
    sqlite3_finalize(statement);
}

static void setUpRoots(sqlite3* database){
    char* sql_string = sqlite3_mprintf(SQL_ROOT_FORMAT,
        AUDIO_CONFIG, ROCK_ROOT, sizeof(ROCK_ROOT) - 1,
        AUDIO_CONFIG, JAZZ_ROOT, sizeof(JAZZ_ROOT) - 1,

        VIDEO_CONFIG, KEEPS_ROOT, sizeof(KEEPS_ROOT) - 1,
        VIDEO_CONFIG, MVS_ROOT, sizeof(MVS_ROOT) - 1,

        COVER_CONFIG, CVS_ROOT, sizeof(CVS_ROOT) - 1,
        COVER_CONFIG, ARTS_ROOT, sizeof(ARTS_ROOT) - 1,

        BLACK_CONFIG, BLACK_ROOT_1, sizeof(BLACK_ROOT_1) - 1,
        BLACK_CONFIG, BLACK_ROOT_2, sizeof(BLACK_ROOT_2) - 1
    );

    executeQuery(database, sql_string);
    sqlite3_free(sql_string);
}

static void setUpPaths(sqlite3* database){
    char* sql_string = sqlite3_mprintf(SQL_PATH_FORMAT,
        ROCK_PATH_1, sizeof(ROCK_PATH_1) - 1,
        ROCK_PATH_2, sizeof(ROCK_PATH_2) - 1,

        JAZZ_PATH_1, sizeof(JAZZ_PATH_1) - 1,
        JAZZ_PATH_2, sizeof(JAZZ_PATH_2) - 1,

        KEEPS_PATH_1, sizeof(KEEPS_PATH_1) - 1,
        KEEPS_PATH_2, sizeof(KEEPS_PATH_2) - 1,

        MVS_PATH_1, sizeof(MVS_PATH_1) - 1,
        MVS_PATH_2, sizeof(MVS_PATH_2) - 1,

        CVS_PATH_1, sizeof(CVS_PATH_1) - 1,
        CVS_PATH_2, sizeof(CVS_PATH_2) - 1,

        ARTS_PATH_1, sizeof(ARTS_PATH_1) - 1,
        ARTS_PATH_2, sizeof(ARTS_PATH_2) - 1
    );

    executeQuery(database, sql_string);
    sqlite3_free(sql_string);
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
        readScript(&sql_init);
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

int createTestDBWithRoots(void** state){
    createTestDB(state);
    setUpRoots(*state);
    return 0;
}

int createTestDBWithRootsAndPaths(void** state){
    createTestDB(state);
    setUpRoots(*state);
    setUpPaths(*state);
    return 0;
}

int closeTestDB(void** state){
    (void)sqlite3_close(*state);
    return 0;
}
