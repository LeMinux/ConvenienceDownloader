#include "../includes/databaseOps.h"

static sqlite3* single_database_connection = NULL;

static void beginTransaction(void);
static void rollbackTransaction(void);
static void commitTransaction(void);

static enum ERROR addRootEntry(enum CONFIG config, const char* entry, int depth);
static enum ERROR updateRootEntry(int id, int new_depth);
static enum ERROR deleteRootEntry(int index);
static enum ERROR deletePaths(int root_id);

static enum ERROR addPathEntry(int root_id, const char* entry, size_t path_size, size_t root_len);
static enum ERROR addSubDirs(const int root_id, const char* root_path, const size_t root_len, int depth);

static enum FIND findEntry(enum CONFIG config_type, const char* entry);
static int getNumOfRootRowsForConfig(enum CONFIG config);
static int translateRootIndexToRow(int user_selection, enum CONFIG config_type);


static void beginTransaction(void){
    assert(single_database_connection != NULL);

    const char sql_begin [] = "BEGIN;";
    char* error = NULL;
    (void)sqlite3_exec(single_database_connection, sql_begin, NULL, NULL, &error);

    assert(error == NULL);
}

//these two can not run if the previous sql statement used has not been finalized.
//If the statement is not finalized, then an error will say there is still
//a statement running.
//Also a note for finalizing is that sqlite_prepared will set the prepared statement
//to NULL if an error occurs.
static void rollbackTransaction(void){
    assert(single_database_connection != NULL);

    const char sql_rollback [] = "ROLLBACK;";
    char* error = NULL;
    (void)sqlite3_exec(single_database_connection, sql_rollback, NULL, NULL, &error);

    assert(error == NULL);
}

static void commitTransaction(void){
    assert(single_database_connection != NULL);

    const char sql_commit [] = "COMMIT;";
    char* error = NULL;
    (void)sqlite3_exec(single_database_connection, sql_commit, NULL, NULL, &error);

    assert(error == NULL);
}

static int getNumOfRootRowsForConfig(enum CONFIG config){
    assert(single_database_connection != NULL);
    assert(config == AUDIO_CONFIG ||
           config == VIDEO_CONFIG ||
           config == COVER_CONFIG ||
           config == BLACK_CONFIG);

    //root_id is indexed so the count should be faster
    const char sql_statement [] = "SELECT COUNT(root_id) FROM Roots WHERE root_type = ?;";
    sqlite3_stmt* results = NULL;
    int ret_code = sqlite3_prepare_v2(single_database_connection, sql_statement, sizeof(sql_statement), &results, NULL);

    int num_of_rows = HAD_ERROR;
    if(ret_code != SQLITE_OK){
        PRINT_FORMAT_ERROR("Failed to obtain count of rows: %s", sqlite3_errmsg(single_database_connection));
        goto failed;
    }

    if(sqlite3_bind_int(results, 1, config) != SQLITE_OK){
        PRINT_ERROR("Failed to bind config to obtain row count");
        goto failed;
    }

    ret_code = sqlite3_step(results);
    if(ret_code != SQLITE_ROW){
        PRINT_FORMAT_ERROR("Could not obtain count of rows: %s", sqlite3_errmsg(single_database_connection));
        goto failed;
    }

    num_of_rows = sqlite3_column_int(results, 0);

    failed:
    sqlite3_finalize(results);
    return num_of_rows;
}

static int translateRootIndexToRow(int user_selection, enum CONFIG config_type){
    assert(user_selection > 0);
    assert(user_selection <= getNumOfRootRowsForConfig(config_type));
    assert(config_type == AUDIO_CONFIG ||
           config_type == VIDEO_CONFIG ||
           config_type == COVER_CONFIG ||
           config_type == BLACK_CONFIG);

    const char sql_statement [] = "SELECT root_id FROM Roots WHERE root_type = ? ORDER BY root_name LIMIT 1 OFFSET ?;";

    sqlite3_stmt* results = NULL;
    int ret_code = sqlite3_prepare_v2(single_database_connection, sql_statement, sizeof(sql_statement), &results, NULL);

    int root_id = HAD_ERROR;
    if(ret_code != SQLITE_OK){
        PRINT_FORMAT_ERROR("Failed to translate index to root row: %s", sqlite3_errmsg(single_database_connection));
        goto failed;
    }

    if(
        sqlite3_bind_int(results, 1, config_type) != SQLITE_OK ||
        sqlite3_bind_int(results, 2, user_selection - 1) != SQLITE_OK
    ){
        PRINT_ERROR("Failed to bind parameters to translate root index to root row");
        goto failed;
    }

    ret_code = sqlite3_step(results);
    if(ret_code != SQLITE_ROW){
        PRINT_FORMAT_ERROR("Could not obtain index translated root row: %s", sqlite3_errmsg(single_database_connection));
        goto failed;
    }

    root_id = sqlite3_column_int(results, 0);

    failed:
    sqlite3_finalize(results);
    return root_id;
}


static enum ERROR addPathEntry(int root_id, const char* entry, size_t path_size, size_t root_len){
    assert(single_database_connection != NULL);
    assert(entry != NULL && entry[0] != '\0');
    assert(path_size <= PATH_MAX);
    assert(root_len < PATH_MAX);

    const char sql_statement [] = "INSERT INTO Paths (root_id, path_name, path_length) VALUES (?, ?, ?);";
    sqlite3_stmt* results = NULL;
    int ret_code = sqlite3_prepare_v2(single_database_connection, sql_statement, sizeof(sql_statement), &results, NULL);

    enum ERROR err_ret = HAD_ERROR;
    if(ret_code != SQLITE_OK){
        PRINT_FORMAT_ERROR("Failed to prepare adding %s", sqlite3_errmsg(single_database_connection));
        goto failed;
    }

    if(
        sqlite3_bind_int(results, 1, root_id) != SQLITE_OK ||
        sqlite3_bind_text(results, 2, entry + root_len, path_size - root_len, NULL) != SQLITE_OK ||
        sqlite3_bind_int(results, 3, path_size - root_len - 1) != SQLITE_OK
    ){
        PRINT_ERROR("Failed to bind path parameters.");
        goto failed;
    }

    //in this case no transaction is made because this should be used within a larger context
    //of addRootEntry
    ret_code = sqlite3_step(results);
    switch(ret_code){
        case SQLITE_DONE:err_ret = NO_ERROR; break;
        case SQLITE_MISUSE: PRINT_ERROR("Dumb idiot programmer made a mistake"); break;
        default: PRINT_FORMAT_ERROR("Error prevented adding path entry: %s", sqlite3_errmsg(single_database_connection)); break;
    }

    failed:
    sqlite3_finalize(results);
    return err_ret;
}

static enum ERROR addSubDirs(const int root_id, const char* built_path, const size_t og_root_len, int depth){
    assert(root_id > 0);
    assert(og_root_len != 0);
    assert(depth >= 0 || depth == INF_DEPTH);
    assert(built_path != NULL);

    char full_path [PATH_MAX];
    const int path_len = snprintf(full_path, PATH_MAX, "%s/", built_path);
    if(path_len >= PATH_MAX){
        ADVISE_USER_FORMAT("Can't add path '%s/' because it's too long", full_path);
        return HAD_ERROR;
    }

    if(addPathEntry(root_id, full_path, path_len + 1, og_root_len) == HAD_ERROR){
        return HAD_ERROR;
    }

    if(depth == 0) return NO_ERROR;

    DIR* dir_stream = opendir(built_path);
    if(dir_stream == NULL) return NO_ERROR;

    enum ERROR error = NO_ERROR;
    struct dirent* dir_entry;
    while((dir_entry = readdir(dir_stream)) != NULL){
        if(strcmp(dir_entry->d_name, ".") == 0 || strcmp(dir_entry->d_name, "..") == 0){
            continue;
        }

        int entry_len = strlen(dir_entry->d_name);
        if(entry_len + path_len >= PATH_MAX){
            ADVISE_USER_FORMAT("Can't add path '%s%s' because it's too long", full_path, dir_entry->d_name);
            continue;
        }
        memcpy(full_path + path_len, dir_entry->d_name, entry_len);
        full_path[path_len + entry_len] = '\0';

        struct stat file_stat = {0};
        if(lstat(full_path, &file_stat) != 0){
            continue;
        }

        if(!S_ISDIR(file_stat.st_mode)){
            continue;
        }

        if(findEntry(BLACK_CONFIG, full_path) == FOUND){
            continue;
        }

        error = addSubDirs(root_id, full_path, og_root_len, (depth == INF_DEPTH) ? INF_DEPTH : depth - 1);
    }

    closedir(dir_stream);
    return error;
}

static enum ERROR addRootEntry(enum CONFIG config, const char* entry, int depth){
    assert(single_database_connection != NULL);
    assert(entry != NULL);
    assert(entry[0] != '\0');
    assert(depth >= 0 || depth == INF_DEPTH);
    assert(config == AUDIO_CONFIG ||
           config == VIDEO_CONFIG ||
           config == COVER_CONFIG ||
           config == BLACK_CONFIG);

    const char sql_statement [] = "INSERT INTO Roots (root_type, root_name, root_length, root_depth) VALUES (?, ?, ?, ?) RETURNING root_id;";
    sqlite3_stmt* results = NULL;
    int ret_code = sqlite3_prepare_v2(single_database_connection, sql_statement, sizeof(sql_statement), &results, NULL);

    enum ERROR err_ret = HAD_ERROR;
    if(ret_code != SQLITE_OK){
        PRINT_FORMAT_ERROR("Failed to prepare adding %s", sqlite3_errmsg(single_database_connection));
        goto failed;
    }

    int entry_size = strlen(entry) + 1;
    if(
        sqlite3_bind_int(results, 1, config) != SQLITE_OK ||
        sqlite3_bind_text(results, 2, entry, entry_size, NULL) != SQLITE_OK ||
        sqlite3_bind_int(results, 3, entry_size - 1) != SQLITE_OK ||
        sqlite3_bind_int(results, 4, depth) != SQLITE_OK
    ){
        PRINT_ERROR("Failed to bind parameters. No changes have been made");
        goto failed;
    }

    ret_code = sqlite3_step(results);
    int inserted_id = 0;
    switch(ret_code){
        case SQLITE_ROW:
            if(config != BLACK_CONFIG){
                inserted_id = sqlite3_column_int(results, 0);
                err_ret = addSubDirs(inserted_id, entry, entry_size - 1, depth);
            }else{
                err_ret = NO_ERROR;
            }
        break;
        case SQLITE_MISUSE: PRINT_ERROR("Dumb idiot programmer made a mistake"); break;
        default: PRINT_FORMAT_ERROR("Error in database prevented adding root entry: %s", sqlite3_errmsg(single_database_connection)); break;
    }

    failed:
    sqlite3_finalize(results);
    return err_ret;
}

static enum ERROR updateRootEntry(int id, int new_depth){
    assert(single_database_connection != NULL);
    assert(id > 0);
    assert(new_depth >= 0 || new_depth == INF_DEPTH || new_depth == SKIPPING);

    const char*  sql_update_statement = NULL;
    if(new_depth == SKIPPING){
        sql_update_statement = "SELECT root_name, root_length, root_depth FROM Roots WHERE root_id = ?;";
    }else{
        sql_update_statement = "UPDATE Roots SET root_depth = ? WHERE root_id = ? RETURNING root_name, root_length;";
    }
    sqlite3_stmt* update_results = NULL;

    enum ERROR func_return = HAD_ERROR;
    int ret_code = sqlite3_prepare_v2(single_database_connection, sql_update_statement, -1, &update_results, NULL);
    if(ret_code != SQLITE_OK){
        puts(sql_update_statement);
        PRINT_FORMAT_ERROR("Failed to prepare updating statement: %s", sqlite3_errmsg(single_database_connection));
        goto failed;
    }

    if(new_depth == SKIPPING){
        if(sqlite3_bind_int(update_results, 1, id) != SQLITE_OK){
            PRINT_ERROR("Failed to bind updating");
            goto failed;
        }
    }else{
        if(
            sqlite3_bind_int(update_results, 1, new_depth) != SQLITE_OK ||
            sqlite3_bind_int(update_results, 2, id) != SQLITE_OK
        ){
            PRINT_ERROR("Failed to bind updating");
            goto failed;
        }
    }

    ret_code = sqlite3_step(update_results);
    int depth = new_depth;
    switch(ret_code){
        case SQLITE_ROW:
            if(deletePaths(id) == NO_ERROR){
                char* name  = (char*)sqlite3_column_text(update_results, 0);
                int len = sqlite3_column_int(update_results, 1);
                if(new_depth == SKIPPING){
                    depth = sqlite3_column_int(update_results, 2);
                }
                func_return = addSubDirs(id, name, len, depth);
            }
        break;
        case SQLITE_MISUSE: PRINT_ERROR("Dumb idiot programmer made a mistake"); break;
        default: PRINT_FORMAT_ERROR("Error in database prevented updating the entry: %s", sqlite3_errmsg(single_database_connection)); break;
    }

    failed:
    sqlite3_finalize(update_results);
    return func_return;
}

static enum ERROR deleteRootEntry(int index){
    assert(single_database_connection != NULL);
    assert(index > 0);

    //ON DELETE CASCADE does most of the work
    const char sql_statement [] = "DELETE FROM Roots WHERE root_id = ?;";

    sqlite3_stmt* results = NULL;
    int ret_code = sqlite3_prepare_v2(single_database_connection, sql_statement, sizeof(sql_statement), &results, NULL);

    enum ERROR func_return = HAD_ERROR;
    if(ret_code != SQLITE_OK){
        PRINT_FORMAT_ERROR("Failed to prepare delete statement: %s", sqlite3_errmsg(single_database_connection));
        goto failed;
    }

    if(sqlite3_bind_int(results, 1, index) != SQLITE_OK){
        PRINT_ERROR("Could not bind parameter for deleting roots");
        goto failed;
    }

    ret_code = sqlite3_step(results);
    switch(ret_code){
        case SQLITE_DONE:
            func_return = NO_ERROR;
        break;
        case SQLITE_MISUSE: PRINT_ERROR("Dumb idiot programmer made a mistake"); break;
        default: PRINT_FORMAT_ERROR("Error in database prevented deleting the entry: %s", sqlite3_errmsg(single_database_connection)); break;
    }

    failed:
    sqlite3_finalize(results);
    return func_return;
}

//transaction control not done here because it is an inner portion of work
static enum ERROR deletePaths(int root_id){
    assert(single_database_connection != NULL);
    assert(root_id > 0);

    const char sql_statement [] = "DELETE FROM Paths WHERE root_id = ?;";

    sqlite3_stmt* results = NULL;
    int ret_code = sqlite3_prepare_v2(single_database_connection, sql_statement, sizeof(sql_statement), &results, NULL);

    enum ERROR func_return = HAD_ERROR;
    if(ret_code != SQLITE_OK){
        PRINT_FORMAT_ERROR("Failed to prepare delete statement: %s", sqlite3_errmsg(single_database_connection));
        goto failed;
    }

    if(sqlite3_bind_int(results, 1, root_id) != SQLITE_OK){
        PRINT_ERROR("Could not bind parameter for deleting paths");
        goto failed;
    }

    ret_code = sqlite3_step(results);
    switch(ret_code){
        case SQLITE_DONE: func_return = NO_ERROR; break;
        case SQLITE_MISUSE: PRINT_ERROR("Dumb idiot programmer made a mistake"); break;
        default: PRINT_FORMAT_ERROR("Error in database prevented deleting paths: %s", sqlite3_errmsg(single_database_connection)); break;
    }

    failed:
    sqlite3_finalize(results);
    return func_return;
}

void addMenu(enum CONFIG config_type){
    assert(single_database_connection != NULL);
    assert(config_type == AUDIO_CONFIG ||
           config_type == VIDEO_CONFIG ||
           config_type == COVER_CONFIG ||
           config_type == BLACK_CONFIG);

    char* path_input = NULL;
    int depth = 0;
    int valid_path = INVALID;
    while(valid_path == INVALID){
        path_input = takeDirectoryInput();
        if(path_input != NULL){
            if(findEntry(config_type, path_input) == FOUND){
                ADVISE_USER("This root path for this config already exists.");
                free(path_input);
                path_input = NULL;
            }else if(findEntry(BLACK_CONFIG, path_input) == FOUND){
                ADVISE_USER("This root path exists in your blacklist.");
                free(path_input);
                path_input = NULL;
            }else{
                valid_path = VALID;
            }
        }
    }

    if(path_input[0] == '\0'){
        ADVISE_USER("Returning to menu. No changes done");
        goto finished;
    }

    if(config_type != BLACK_CONFIG){
        while((depth = takeDepthInput()) == INVALID);
    }

    if(depth == SKIPPING){
        ADVISE_USER("Returning to menu. No changes done");
        goto finished;
    }

    beginTransaction();
    if(addRootEntry(config_type, path_input, depth) == NO_ERROR){
        commitTransaction();
        ADVISE_USER("Added entry and it's paths!");
    }else{
        rollbackTransaction();
        ADVISE_USER("No changes have been made");
    }

    finished:
    free(path_input);
    path_input = NULL;
}

void updateMenu(enum CONFIG config_type){
    assert(single_database_connection != NULL);
    assert(config_type == AUDIO_CONFIG ||
           config_type == VIDEO_CONFIG ||
           config_type == COVER_CONFIG ||
           config_type == BLACK_CONFIG);

    if(config_type == BLACK_CONFIG){
        ADVISE_USER("Updating is for depths, and blacklist entries don't have this.");
        return;
    }

    int max_index = getNumOfRootRowsForConfig(config_type);
    if(max_index == HAD_ERROR){
        ADVISE_USER("No changes have been made.");
        return;
    }else if(max_index == 0){
        ADVISE_USER("There is nothing to update.");
        return;
    }

    int select_index = INVALID;
    while((select_index = takeIndexInput(max_index)) == INVALID){}

    if(select_index == SKIPPING){
        ADVISE_USER("Returning to menu. No changes done");
        return;
    }

    int depth = 0;
    while((depth = takeDepthInput()) == INVALID);

    int row_id = translateRootIndexToRow(select_index, config_type);
    beginTransaction();
    if(row_id == HAD_ERROR || updateRootEntry(row_id, depth) == HAD_ERROR){
        rollbackTransaction();
        ADVISE_USER("No changes have been made");
    }else{
        commitTransaction();
        ADVISE_USER("Updated entry!");
    }
}

void deleteMenu(enum CONFIG config_type){
    int max_index = getNumOfRootRowsForConfig(config_type);
    if(max_index == HAD_ERROR){
        ADVISE_USER("No changes have been made.");
        return;
    }else if(max_index == 0){
        ADVISE_USER("There is nothing to delete.");
        return;
    }

    int select_index = INVALID;
    while((select_index = takeIndexInput(max_index)) == INVALID){}

    if(select_index == SKIPPING){
        ADVISE_USER("Returning to menu. No changes done");
        return;
    }

    int row_id = translateRootIndexToRow(select_index, config_type);
    beginTransaction();
    if(row_id == HAD_ERROR || deleteRootEntry(row_id) == HAD_ERROR){
        rollbackTransaction();
        ADVISE_USER("No changes have been made");
    }else{
        commitTransaction();
        ADVISE_USER("Deleted entry and associated paths!");
    }
}

static void printSectionHeader(enum CONFIG config_type){
    assert(config_type == AUDIO_CONFIG ||
           config_type == VIDEO_CONFIG ||
           config_type == COVER_CONFIG ||
           config_type == BLACK_CONFIG);

    switch(config_type){
        case AUDIO_CONFIG:
            puts("\nAUDIO ROOTS");
        break;

        case VIDEO_CONFIG:
            puts("\nVIDEO ROOTS");
        break;

        case COVER_CONFIG:
            puts("\nCOVER ROOTS");
        break;

        case BLACK_CONFIG:
            puts("\nBLACK LIST");
        break;

        default: break;
    }
}

enum ERROR initDatabase(void){
    uid_t uid;
    struct passwd* passwd_entry;

    uid = getuid();
    passwd_entry = getpwuid(uid);
    if(passwd_entry == NULL){
        PRINT_ERROR("Could not open user's information to get home directory");
        endpwent();
        return HAD_ERROR;
    }

    char home_db [PATH_MAX];
    if(snprintf(home_db, sizeof(home_db), "%s/%s", passwd_entry->pw_dir, CONFIG_DATABASE) >= PATH_MAX){
        PRINT_ERROR("Home directory path is too long to get database. Why is your home path so large though? I mean seriously why is not not just /home/<username>/");
        endpwent();
        return HAD_ERROR;
    }

    endpwent();

    if(sqlite3_open(home_db, &single_database_connection) != SQLITE_OK){
        PRINT_ERROR("Could not open configuration database. Have you done make init?");
        return HAD_ERROR;
    }

    return NO_ERROR;
}


enum ERROR refreshDatabase(void){
    assert(single_database_connection != NULL);
    const char sql_statement [] = "SELECT root_id, root_type, root_name, root_length, root_depth FROM Roots ORDER BY root_type, root_name;";
    sqlite3_stmt* results = NULL;
    int ret_code = sqlite3_prepare_v2(single_database_connection, sql_statement, sizeof(sql_statement), &results, NULL);

    int error_occured = NO_ERROR;
    if(ret_code != SQLITE_OK){
        PRINT_FORMAT_ERROR("Failed to refresh due to %s", sqlite3_errmsg(single_database_connection));
        error_occured = HAD_ERROR;
    }

    beginTransaction();
    while(error_occured == NO_ERROR && (ret_code = sqlite3_step(results)) == SQLITE_ROW){
        int id = sqlite3_column_int(results, 0);
        enum CONFIG type = sqlite3_column_int(results, 1);
        char* root = (char*)sqlite3_column_text(results, 2);

        //It's a lot easier to just delete everything and fill in what is there.
        //the database can be thought to hold old data and the file system holds
        //new data.
        DIR* dir_stream = opendir(root);
        if(dir_stream == NULL){
            error_occured = deleteRootEntry(id);
        }else if(type != BLACK_CONFIG){
            error_occured = deletePaths(id);
            if(!error_occured){
                int root_len = sqlite3_column_int(results, 3);
                int root_depth = sqlite3_column_int(results, 4);
                error_occured = addSubDirs(id, root, root_len, root_depth);
            }
        }
    }

    sqlite3_finalize(results);
    if(error_occured){
        rollbackTransaction();
    }else{
        commitTransaction();
    }

    return error_occured;
}


static enum FIND findEntry(enum CONFIG config_type, const char* entry){
    assert(entry != NULL);
    assert(config_type == AUDIO_CONFIG ||
           config_type == VIDEO_CONFIG ||
           config_type == COVER_CONFIG ||
           config_type == BLACK_CONFIG);

    const char sql_statement [] = "SELECT COUNT(root_id) FROM Roots WHERE root_type = ? AND root_name = ?;";

    sqlite3_stmt* results = NULL;
    int ret_code = sqlite3_prepare_v2(single_database_connection, sql_statement, sizeof(sql_statement), &results, NULL);

    enum FIND is_found = FIND_ERROR;
    if(ret_code != SQLITE_OK){
        PRINT_FORMAT_ERROR("Failed to find entry due to %s", sqlite3_errmsg(single_database_connection));
        goto failed;
    }

    if(
        sqlite3_bind_int(results, 1, config_type) != SQLITE_OK ||
        sqlite3_bind_text(results, 2, entry, strlen(entry) + 1, NULL) != SQLITE_OK
    ){
        PRINT_FORMAT_ERROR("Failed to bind parameters for finding entry: %s", sqlite3_errmsg(single_database_connection));
        goto failed;
    }

    is_found = NOT_FOUND;
    ret_code = sqlite3_step(results);
    if(ret_code == SQLITE_ROW){
        if(sqlite3_column_int(results, 0) > 0){
            is_found = FOUND;
        }
    }

    failed:
    sqlite3_finalize(results);
    return is_found;
}

int getNumOfPathRowsForConfig(enum CONFIG config){
    assert(single_database_connection != NULL);
    assert(config == AUDIO_CONFIG ||
           config == VIDEO_CONFIG ||
           config == COVER_CONFIG ||
           config == BLACK_CONFIG);

    //root_id is indexed so the count should be faster
    const char sql_statement [] = "SELECT COUNT(p.path_id) FROM Paths p RIGHT JOIN Roots r WHERE r.root_type = ?;";
    sqlite3_stmt* results = NULL;
    int ret_code = sqlite3_prepare_v2(single_database_connection, sql_statement, sizeof(sql_statement), &results, NULL);

    int num_of_rows = HAD_ERROR;
    if(ret_code != SQLITE_OK){
        PRINT_FORMAT_ERROR("Failed to obtain count of rows: %s", sqlite3_errmsg(single_database_connection));
        goto failed;
    }

    if(sqlite3_bind_int(results, 1, config) != SQLITE_OK){
        PRINT_ERROR("Failed to bind config to obtain row count");
        goto failed;
    }

    ret_code = sqlite3_step(results);
    if(ret_code != SQLITE_ROW){
        PRINT_FORMAT_ERROR("Could not obtain count of rows: %s", sqlite3_errmsg(single_database_connection));
        goto failed;
    }

    num_of_rows = sqlite3_column_int(results, 0);

    failed:
    sqlite3_finalize(results);
    return num_of_rows;
}

int translatePathIndexToRow(int user_selection, enum CONFIG config_type){
    assert(user_selection > 0);
    assert(user_selection <= getNumOfPathRowsForConfig(config_type));
    assert(config_type == AUDIO_CONFIG ||
           config_type == VIDEO_CONFIG ||
           config_type == COVER_CONFIG ||
           config_type == BLACK_CONFIG);

    const char sql_statement [] =
             "SELECT p.path_id FROM Paths p "
             "RIGHT JOIN Roots r USING (root_id) "
             "WHERE r.root_type = ? "
             "ORDER BY r.root_name, p.path_name "
             "LIMIT 1 OFFSET ?;";

    sqlite3_stmt* results = NULL;
    int ret_code = sqlite3_prepare_v2(single_database_connection, sql_statement, sizeof(sql_statement), &results, NULL);

    int path_id = HAD_ERROR;
    if(ret_code != SQLITE_OK){
        PRINT_FORMAT_ERROR("Failed to translate index to row: %s", sqlite3_errmsg(single_database_connection));
        goto failed;
    }

    if(
        sqlite3_bind_int(results, 1, config_type) != SQLITE_OK ||
        sqlite3_bind_int(results, 2, user_selection - 1) != SQLITE_OK
    ){
        PRINT_ERROR("Failed to bind parameters to translate index to row");
        goto failed;
    }

    ret_code = sqlite3_step(results);
    if(ret_code != SQLITE_ROW){
        PRINT_FORMAT_ERROR("Could not obtain index translated row: %s", sqlite3_errmsg(single_database_connection));
        goto failed;
    }

    path_id = sqlite3_column_int(results, 0);

    failed:
    sqlite3_finalize(results);
    return path_id;
}

enum ERROR pathIDToPath(int path_id, char* full_path){
    assert(single_database_connection != NULL);
    assert(path_id >= 0);

    const char sql_statement [] =
             "SELECT concat(r.root_name, p.path_name), (r.root_length + p.path_length) FROM Paths p "
             "RIGHT JOIN Roots r USING (root_id) "
             "WHERE p.path_id = ?";

    sqlite3_stmt* results = NULL;
    int ret_code = sqlite3_prepare_v2(single_database_connection, sql_statement, sizeof(sql_statement), &results, NULL);

    enum ERROR err_ret = HAD_ERROR;
    if(ret_code != SQLITE_OK){
        PRINT_FORMAT_ERROR("Failed to prepare path id to path due to %s", sqlite3_errmsg(single_database_connection));
        goto failed;
    }

    if(sqlite3_bind_int(results, 1, path_id) != SQLITE_OK){
        PRINT_ERROR("Could not bind parameter for listing roots with paths in config");
        goto failed;
    }

    ret_code = sqlite3_step(results);
    if(ret_code != SQLITE_ROW){
        PRINT_FORMAT_ERROR("Could not obtain path name from path id: %s", sqlite3_errmsg(single_database_connection));
        goto failed;
    }

    char* path = (char*)sqlite3_column_text(results, 0);
    int total_len = sqlite3_column_int(results, 1);
    memcpy(full_path, path, total_len);
    full_path[total_len] = '\0';
    err_ret = NO_ERROR;

    failed:
    sqlite3_finalize(results);
    return err_ret;
}

//Figure out how you want to figure out the index once a user inputs
//returning the statement might be easier
enum ERROR listConfigRoots(enum CONFIG config_type){
    assert(single_database_connection != NULL);
    assert(config_type == AUDIO_CONFIG ||
           config_type == VIDEO_CONFIG ||
           config_type == COVER_CONFIG ||
           config_type == BLACK_CONFIG);

    int max_index = getNumOfRootRowsForConfig(config_type);
    if(max_index == HAD_ERROR){
        return HAD_ERROR;
    }else if(max_index == 0){
        printSectionHeader(config_type);
        puts("No root entries");
        return NO_ERROR;
    }

    const char sql_statement [] = "SELECT root_name, root_depth FROM Roots WHERE root_type = ? ORDER BY root_name;";
    sqlite3_stmt* results = NULL;
    int ret_code = sqlite3_prepare_v2(single_database_connection, sql_statement, sizeof(sql_statement), &results, NULL);

    enum ERROR err_ret = HAD_ERROR;
    if(ret_code != SQLITE_OK){
        PRINT_FORMAT_ERROR("Failed to list due to %s", sqlite3_errmsg(single_database_connection));
        goto failed;
    }

    if(sqlite3_bind_int(results, 1, config_type) != SQLITE_OK){
        PRINT_ERROR("Could not bind parameter for listing roots in config");
        goto failed;
    }

    int count = 1;
    printSectionHeader(config_type);
    while((ret_code = sqlite3_step(results)) == SQLITE_ROW){
        char* root = (char*)sqlite3_column_text(results, 0);
        int depth = sqlite3_column_int(results, 1);
        if(depth == INF_DEPTH){
            printf("%d) %s/ | INF\n", count, root);
        }else{
            printf("%d) %s/ | %d\n", count, root, depth);
        }
        ++count;
    }
    err_ret = NO_ERROR;

    failed:
    sqlite3_finalize(results);
    return err_ret;
}

enum ERROR listConfigRootsWithPaths(enum CONFIG config_type){
    assert(single_database_connection != NULL);
    assert(config_type == AUDIO_CONFIG ||
           config_type == VIDEO_CONFIG ||
           config_type == COVER_CONFIG);

    const char sql_statement [] =
             "SELECT r.root_id, r.root_name, p.path_name FROM Paths p "
             "RIGHT JOIN Roots r USING (root_id) "
             "WHERE r.root_type = ? "
             "ORDER BY r.root_name, p.path_name;";

    sqlite3_stmt* results = NULL;
    int ret_code = sqlite3_prepare_v2(single_database_connection, sql_statement, sizeof(sql_statement), &results, NULL);

    enum ERROR err_ret = HAD_ERROR;
    if(ret_code != SQLITE_OK){
        PRINT_FORMAT_ERROR("Failed to list due to %s", sqlite3_errmsg(single_database_connection));
        goto failed;
    }

    if(sqlite3_bind_int(results, 1, config_type) != SQLITE_OK){
        PRINT_ERROR("Could not bind parameter for listing roots with paths in config");
        goto failed;
    }

    printSectionHeader(config_type);
    int prev_root_id = -1;
    int count = 1;
    while((ret_code = sqlite3_step(results)) == SQLITE_ROW){
        int cur_root_id = sqlite3_column_int(results, 0);

        if(cur_root_id != prev_root_id){
            char* root = (char*)sqlite3_column_text(results, 1);
            printf("\t%s\n", root);
        }

        char* path = (char*)sqlite3_column_text(results, 2);
        if(path != NULL){
            printf("%d)\t\t%s\n", count++, path);
        }else{
            puts("\t\tNO ENTRIES");
        }

        prev_root_id = cur_root_id;
    }
    err_ret = NO_ERROR;

    failed:
    sqlite3_finalize(results);
    return err_ret;
}

enum ERROR listAllRoots(){
    assert(single_database_connection != NULL);

    const char sql_statement [] = "SELECT root_type, root_name, root_depth FROM Roots ORDER BY root_type, root_name;";
    sqlite3_stmt* results = NULL;
    int ret_code = sqlite3_prepare_v2(single_database_connection, sql_statement, sizeof(sql_statement), &results, NULL);

    if(ret_code != SQLITE_OK){
        PRINT_FORMAT_ERROR("Failed to list due to %s", sqlite3_errmsg(single_database_connection));
        return HAD_ERROR;
    }

    enum CONFIG prev_type = -1;
    while((ret_code = sqlite3_step(results)) == SQLITE_ROW){
        enum CONFIG cur_type = sqlite3_column_int(results, 0);
        char* root = (char*)sqlite3_column_text(results, 1);
        int depth = sqlite3_column_int(results, 2);
        if(cur_type != prev_type){
            printSectionHeader(cur_type);
        }
        printf("\t%s/     %d\n", root, depth);
        prev_type = cur_type;
    }

    sqlite3_finalize(results);
    return NO_ERROR;
}

enum ERROR listAllRootWithPaths(){
    assert(single_database_connection != NULL);

    const char sql_statement [] =
             "SELECT r.root_type, r.root_id, r.root_name, p.path_name, r.root_depth FROM Paths p "
             "RIGHT JOIN Roots r USING (root_id) "
             "ORDER BY r.root_type, r.root_name, p.path_name;";

    sqlite3_stmt* results = NULL;
    int ret_code = sqlite3_prepare_v2(single_database_connection, sql_statement, sizeof(sql_statement), &results, NULL);

    if(ret_code != SQLITE_OK){
        PRINT_FORMAT_ERROR("Failed to list due to %s", sqlite3_errmsg(single_database_connection));
        return HAD_ERROR;
    }

    enum CONFIG prev_type = -1;
    int prev_root_id = -1;
    while((ret_code = sqlite3_step(results)) == SQLITE_ROW){
        enum CONFIG cur_type = sqlite3_column_int(results, 0);
        int cur_root_id = sqlite3_column_int(results, 1);

        if(cur_type != prev_type){
            printSectionHeader(cur_type);
        }

        if(cur_root_id != prev_root_id){
            char* root = (char*)sqlite3_column_text(results, 2);
            if(cur_type == BLACK_CONFIG){
                printf("\t%s/\n", root);
            }else{
                int depth = sqlite3_column_int(results, 4);
                printf("\t%s\t%d\n", root, depth);
            }
        }

        if(cur_type != BLACK_CONFIG){
            char* path = (char*)sqlite3_column_text(results, 3);
            if(path != NULL){
                printf("\t\t%s\n", path);
            }else{
                puts("\t\tNO ENTRIES");
            }
        }

        prev_root_id = cur_root_id;
        prev_type = cur_type;
    }

    sqlite3_finalize(results);
    return NO_ERROR;
}

#ifdef TESTING

void __testingSetDB(sqlite3* testing_db){
    single_database_connection = testing_db;
}

#endif

