#include "../includes/databaseOps.h"

static sqlite3* single_database_connection = NULL;

static void beginTransaction(void);
static void rollbackTransaction(void);
static void commitTransaction(void);

static enum ERROR addRootEntry(enum CONFIG config, const char* entry, int depth);
static enum ERROR updateRootEntry(int index, const char* new_value, int new_depth);
static enum ERROR deleteRootEntry(int index);
static enum ERROR deletePaths(int root_id);

static enum ERROR addPathEntry(int root_id, const char* entry, size_t path_size, size_t root_len);
static enum ERROR addSubDirs(const int root_id, const char* root_path, const size_t root_len, int depth);

static int getNumOfRowsForConfig(enum CONFIG config);
static int translateIndexToRow(int user_selection, enum CONFIG config_type);

static void beginTransaction(void){
    assert(single_database_connection != NULL);
    const char sql_begin [] = "BEGIN;";
    (void)sqlite3_exec(single_database_connection, sql_begin, NULL, NULL, NULL);
}

static void rollbackTransaction(void){
    assert(single_database_connection != NULL);
    const char sql_rollback [] = "ROLLBACK;";
    (void)sqlite3_exec(single_database_connection, sql_rollback, NULL, NULL, NULL);
}

static void commitTransaction(void){
    assert(single_database_connection != NULL);
    const char sql_commit [] = "COMMIT;";
    (void)sqlite3_exec(single_database_connection, sql_commit, NULL, NULL, NULL);
}


static int getNumOfRowsForConfig(enum CONFIG config){
    assert(single_database_connection != NULL);
    assert(config == AUDIO_CONFIG ||
           config == VIDEO_CONFIG ||
           config == COVER_CONFIG ||
           config == BLACK_CONFIG);

    //root_id is indexed so the count should be faster
    const char sql_statement [] = "SELECT COUNT(root_id) FROM Roots WHERE root_type = ?;";
    sqlite3_stmt* results = NULL;
    int ret_code = sqlite3_prepare_v2(single_database_connection, sql_statement, -1, &results, NULL);

    if(ret_code != SQLITE_OK){
        PRINT_FORMAT_ERROR("Failed to obtain count of rows: %s", sqlite3_errmsg(single_database_connection));
        return HAD_ERROR;
    }

    if(sqlite3_bind_int(results, 1, config) != SQLITE_OK){
        PRINT_ERROR("Failed to bind config to obtain row count");
        return HAD_ERROR;
    }

    ret_code = sqlite3_step(results);
    if(ret_code != SQLITE_ROW){
        PRINT_FORMAT_ERROR("Could not obtain count of rows: %s", sqlite3_errmsg(single_database_connection));
        return HAD_ERROR;
    }

    return sqlite3_column_int(results, 0);
}

static int translateIndexToRow(int user_selection, enum CONFIG config_type){
    assert(user_selection > 0);
    assert(user_selection <= getNumOfRowsForConfig(config_type));
    assert(config_type == AUDIO_CONFIG ||
           config_type == VIDEO_CONFIG ||
           config_type == COVER_CONFIG ||
           config_type == BLACK_CONFIG);

    const char sql_statement [] = "SELECT root_id FROM Roots WHERE root_type = ? ORDER BY root_name LIMIT 1 OFFSET ?;";

    sqlite3_stmt* results = NULL;
    int ret_code = sqlite3_prepare_v2(single_database_connection, sql_statement, -1, &results, NULL);

    if(ret_code != SQLITE_OK){
        PRINT_FORMAT_ERROR("Failed to translate index to row: %s", sqlite3_errmsg(single_database_connection));
        return HAD_ERROR;
    }


    if(
        sqlite3_bind_int(results, 1, config_type) != SQLITE_OK ||
        sqlite3_bind_int(results, 2, user_selection - 1) != SQLITE_OK
    ){
        PRINT_ERROR("Failed to bind parameters to translate index to row");
        return HAD_ERROR;
    }

    ret_code = sqlite3_step(results);
    if(ret_code != SQLITE_ROW){
        PRINT_FORMAT_ERROR("Could not obtain index translated row: %s", sqlite3_errmsg(single_database_connection));
        return HAD_ERROR;
    }

    return sqlite3_column_int(results, 0);
}


static enum ERROR addPathEntry(int root_id, const char* entry, size_t path_size, size_t root_len){
    assert(single_database_connection != NULL);
    assert(entry != NULL && entry[0] != '\0');
    assert(path_size <= PATH_MAX);
    assert(root_len < PATH_MAX);

    const char sql_statement [] = "INSERT INTO Paths (root_id, path_name, path_length) VALUES (?, ?, ?);";
    sqlite3_stmt* results = NULL;
    int ret_code = sqlite3_prepare_v2(single_database_connection, sql_statement, -1, &results, NULL);

    if(ret_code != SQLITE_OK){
        PRINT_FORMAT_ERROR("Failed to prepare adding %s", sqlite3_errmsg(single_database_connection));
        return HAD_ERROR;
    }

    if(
        sqlite3_bind_int(results, 1, root_id) != SQLITE_OK ||
        sqlite3_bind_text(results, 2, entry + root_len, path_size - root_len, NULL) != SQLITE_OK ||
        sqlite3_bind_int(results, 3, path_size - root_len - 1) != SQLITE_OK
    ){
        PRINT_ERROR("Failed to bind path parameters.");
        return HAD_ERROR;
    }

    ret_code = sqlite3_step(results);
    enum ERROR func_return = HAD_ERROR;
    switch(ret_code){
        case SQLITE_DONE: func_return = NO_ERROR; break;
        case SQLITE_MISUSE: PRINT_ERROR("Dumb idiot programmer made a mistake"); break;
        default: PRINT_FORMAT_ERROR("Error prevented adding path entry: %s", sqlite3_errmsg(single_database_connection)); break;
    }

    return func_return;
}

static enum ERROR addSubDirs(const int root_id, const char* root_path, const size_t root_len, int depth){
    assert(root_path != NULL);
    enum ERROR error_status = NO_ERROR;

    if(depth == 0) return error_status;

    DIR* dir_stream = opendir(root_path);
    if(dir_stream == NULL) return error_status;

    struct dirent* dir_entry;
    while((dir_entry = readdir(dir_stream)) != NULL){
        if( strcmp(dir_entry->d_name, ".") == 0 || strcmp(dir_entry->d_name, "..") == 0){
            continue;
        }

        char full_path [PATH_MAX];
        int path_len = snprintf(full_path, PATH_MAX, "%s/%s", root_path, dir_entry->d_name);
        if(path_len >= PATH_MAX){
            ADVISE_USER_FORMAT("Can't add path '%s/%s' because it's too long", full_path, dir_entry->d_name);
            continue;
        }

        if(findEntry(BLACK_CONFIG, full_path) == FOUND){
            continue;
        }

        struct stat file_stat = {0};
        if(lstat(full_path, &file_stat) != 0){
            continue;
        }

        if(!S_ISDIR(file_stat.st_mode)){
            continue;
        }

        //+1 to root_len for the extra '/' added
        if(addPathEntry(root_id, full_path, path_len + 1, root_len + 1) == HAD_ERROR){
            error_status = HAD_ERROR;
            break;
        }
        addSubDirs(root_id, full_path, root_len, (depth == INF_DEPTH) ? INF_DEPTH : depth - 1);
    }

    closedir(dir_stream);
    return error_status;
}

static enum ERROR addRootEntry(enum CONFIG config, const char* entry, int depth){
    assert(single_database_connection != NULL);
    assert(entry != NULL && entry[0] != '\0');
    assert(depth >= 0 || depth == INF_DEPTH);
    assert(config == AUDIO_CONFIG ||
           config == VIDEO_CONFIG ||
           config == COVER_CONFIG ||
           config == BLACK_CONFIG);

    const char sql_statement [] = "INSERT INTO Roots (root_type, root_name, root_length, root_depth) VALUES (?, ?, ?, ?) RETURNING root_id;";
    sqlite3_stmt* results = NULL;
    int ret_code = sqlite3_prepare_v2(single_database_connection, sql_statement, -1, &results, NULL);

    if(ret_code != SQLITE_OK){
        PRINT_FORMAT_ERROR("Failed to prepare adding %s", sqlite3_errmsg(single_database_connection));
        return HAD_ERROR;
    }

    int entry_size = strlen(entry) + 1;
    if(
        sqlite3_bind_int(results, 1, config) != SQLITE_OK ||
        sqlite3_bind_text(results, 2, entry, entry_size, NULL) != SQLITE_OK ||
        sqlite3_bind_int(results, 3, entry_size - 1) != SQLITE_OK ||
        sqlite3_bind_int(results, 4, depth) != SQLITE_OK
    ){
        PRINT_ERROR("Failed to bind parameters. No changes have been made");
        return HAD_ERROR;
    }

    beginTransaction();
    ret_code = sqlite3_step(results);
    int inserted_id = 0;
    int err_ret = HAD_ERROR;
    switch(ret_code){
        case SQLITE_ROW:
            inserted_id = sqlite3_column_int(results, 0);
            if(addSubDirs(inserted_id, entry, entry_size - 1, depth) == NO_ERROR){
                ADVISE_USER("Added entry and it's paths!");
                err_ret = NO_ERROR;
            }
        break;
        case SQLITE_MISUSE: PRINT_ERROR("Dumb idiot programmer made a mistake"); break;
        default: PRINT_FORMAT_ERROR("Error in database prevented adding root entry: %s", sqlite3_errmsg(single_database_connection)); break;
    }

    if(err_ret == HAD_ERROR){
        rollbackTransaction();
    }else{
        commitTransaction();
    }
    return err_ret;
}

static enum ERROR updateRootEntry(int id, const char* new_entry, int new_depth){
    assert(single_database_connection != NULL);
    assert(id > 0);
    assert(new_entry != NULL);
    assert(new_depth >= 0 || new_depth == INF_DEPTH);

    const char sql_update_statement [] = "UPDATE Roots SET root_name = ?, root_length = ?, root_depth = ? WHERE root_id = ?;";
    const char sql_delete_children [] = "DELETE FROM Paths WHERE root_id = ?";

    sqlite3_stmt* update_results = NULL;
    sqlite3_stmt* delete_results = NULL;

    int ret_code = sqlite3_prepare_v2(single_database_connection, sql_update_statement, -1, &update_results, NULL);
    if(ret_code != SQLITE_OK){
        PRINT_FORMAT_ERROR("Failed to prepare updating statement: %s", sqlite3_errmsg(single_database_connection));
        return HAD_ERROR;
    }

    ret_code = sqlite3_prepare_v2(single_database_connection, sql_delete_children, -1, &delete_results, NULL);
    if(ret_code != SQLITE_OK){
        PRINT_FORMAT_ERROR("Failed to prepare deletion in updating statement: %s", sqlite3_errmsg(single_database_connection));
        return HAD_ERROR;
    }

    int entry_size = strlen(new_entry) + 1;
    if(
        sqlite3_bind_text(update_results, 1, new_entry, entry_size, NULL) != SQLITE_OK ||
        sqlite3_bind_int(update_results, 2, entry_size - 1) != SQLITE_OK ||
        sqlite3_bind_int(update_results, 3, new_depth) != SQLITE_OK ||
        sqlite3_bind_int(update_results, 4, id) != SQLITE_OK
    ){
        PRINT_ERROR("Failed to bind update query");
        return HAD_ERROR;
    }

    if(
        sqlite3_bind_int(delete_results, 1, id) != SQLITE_OK
    ){
        PRINT_ERROR("Failed to bind deletion for update query");
        return HAD_ERROR;
    }

    beginTransaction();
    ret_code = sqlite3_step(update_results);
    enum ERROR func_return = HAD_ERROR;
    switch(ret_code){
        case SQLITE_DONE:
            ret_code = sqlite3_step(delete_results);
            if(ret_code == SQLITE_DONE){
                if(addSubDirs(id, new_entry, entry_size - 1, new_depth) == NO_ERROR){
                    ADVISE_USER("Updated entry!");
                    func_return = NO_ERROR;
                }
            }else{
                PRINT_FORMAT_ERROR("Error in database prevented updating the entry: %s", sqlite3_errmsg(single_database_connection));
            }
        break;
        case SQLITE_MISUSE: PRINT_ERROR("Dumb idiot programmer made a mistake"); break;
        default: PRINT_FORMAT_ERROR("Error in database prevented updating the entry: %s", sqlite3_errmsg(single_database_connection)); break;
    }

    if(func_return == HAD_ERROR){
        rollbackTransaction();
    }else{
        commitTransaction();
    }
    return func_return;
}

static enum ERROR deleteRootEntry(int index){
    assert(single_database_connection != NULL);
    assert(index > 0);

    //ON DELETE CASCADE does most of the work
    const char sql_statement [] = "DELETE FROM Roots WHERE root_id = ?;";

    sqlite3_stmt* results = NULL;
    int ret_code = sqlite3_prepare_v2(single_database_connection, sql_statement, -1, &results, NULL);

    if(ret_code != SQLITE_OK){
        PRINT_FORMAT_ERROR("Failed to prepare delete statement: %s", sqlite3_errmsg(single_database_connection));
        return HAD_ERROR;
    }

    sqlite3_bind_int(results, 1, index);

    ret_code = sqlite3_step(results);
    enum ERROR func_return = HAD_ERROR;
    switch(ret_code){
        case SQLITE_DONE: ADVISE_USER("Deleted entry and associated rows!"); func_return = NO_ERROR; break;
        case SQLITE_MISUSE: PRINT_ERROR("Dumb idiot programmer made a mistake"); break;
        default: PRINT_FORMAT_ERROR("Error in database prevented deleting the entry: %s", sqlite3_errmsg(single_database_connection)); break;
    }

    return func_return;
}

static enum ERROR deletePaths(int root_id){
    assert(single_database_connection != NULL);
    assert(root_id > 0);

    const char sql_statement [] = "DELETE FROM Paths WHERE root_id = ?;";

    sqlite3_stmt* results = NULL;
    int ret_code = sqlite3_prepare_v2(single_database_connection, sql_statement, -1, &results, NULL);

    if(ret_code != SQLITE_OK){
        PRINT_FORMAT_ERROR("Failed to prepare delete statement: %s", sqlite3_errmsg(single_database_connection));
        return HAD_ERROR;
    }

    sqlite3_bind_int(results, 1, root_id);

    ret_code = sqlite3_step(results);
    enum ERROR func_return = HAD_ERROR;
    switch(ret_code){
        case SQLITE_DONE: func_return = NO_ERROR; break;
        case SQLITE_MISUSE: PRINT_ERROR("Dumb idiot programmer made a mistake"); break;
        default: PRINT_FORMAT_ERROR("Error in database prevented deleting paths: %s", sqlite3_errmsg(single_database_connection)); break;
    }

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

    if(config_type != BLACK_CONFIG){
        while((depth = takeDepthInput()) == INVALID);
    }

    if(addRootEntry(config_type, path_input, depth) == HAD_ERROR){
        ADVISE_USER("No changes have been made");
    }

    free(path_input);
    path_input = NULL;
}

void updateMenu(enum CONFIG config_type){
    assert(single_database_connection != NULL);
    assert(config_type == AUDIO_CONFIG ||
           config_type == VIDEO_CONFIG ||
           config_type == COVER_CONFIG ||
           config_type == BLACK_CONFIG);

    int max_index = getNumOfRowsForConfig(config_type);
    if(max_index == HAD_ERROR){
        ADVISE_USER("No changes have been made.");
        return;
    }else if(max_index == 0){
        ADVISE_USER("There is nothing to update.");
        return;
    }
    int select_index = INVALID;
    while((select_index = takeIndexInput(max_index)) == INVALID){}

    char* path_input;
    int depth = 0;
    int valid_path = INVALID;
    while(valid_path == INVALID){
        path_input = takeDirectoryInput();
        if(path_input != NULL){
            if(findEntry(config_type, path_input) == FOUND){
                ADVISE_USER("This path for this config already exists.");
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

    if(config_type != BLACK_CONFIG){
        while((depth = takeDepthInput()) == INVALID);
    }

    int row_id = translateIndexToRow(select_index, config_type);
    if(row_id == HAD_ERROR || updateRootEntry(row_id, path_input, depth) == HAD_ERROR){
        ADVISE_USER("No changes have been made");
    }

    free(path_input);
    path_input = NULL;
}

void deleteMenu(enum CONFIG config_type){
    int max_index = getNumOfRowsForConfig(config_type);
    if(max_index == HAD_ERROR){
        ADVISE_USER("No changes have been made.");
        return;
    }else if(max_index == 0){
        ADVISE_USER("There is nothing to delete.");
        return;
    }

    int select_index = INVALID;
    while((select_index = takeIndexInput(max_index)) == INVALID){}

    int row_id = translateIndexToRow(select_index, config_type);
    if(row_id == HAD_ERROR){
        ADVISE_USER("No changes have been made.");
        return;
    }

    deleteRootEntry(row_id);
}

static void printSectionHeader(enum CONFIG config_type){
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
    int ret_code = sqlite3_prepare_v2(single_database_connection, sql_statement, -1, &results, NULL);

    if(ret_code != SQLITE_OK){
        PRINT_FORMAT_ERROR("Failed to refresh due to %s", sqlite3_errmsg(single_database_connection));
        return HAD_ERROR;
    }

    beginTransaction();
    int error_occured = NO_ERROR;
    while((ret_code = sqlite3_step(results)) == SQLITE_ROW && !error_occured){
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

    if(error_occured){
        rollbackTransaction();
    }else{
        commitTransaction();
    }

    return error_occured;
}


enum FIND findEntry(enum CONFIG config_type, const char* entry){
    assert(entry != NULL);
    assert(config_type == AUDIO_CONFIG ||
           config_type == VIDEO_CONFIG ||
           config_type == COVER_CONFIG ||
           config_type == BLACK_CONFIG);

    const char sql_statement [] = "SELECT COUNT(root_id) FROM Roots WHERE root_type = ? AND root_name = ?;";

    sqlite3_stmt* results = NULL;
    int ret_code = sqlite3_prepare_v2(single_database_connection, sql_statement, -1, &results, NULL);

    if(ret_code != SQLITE_OK){
        PRINT_FORMAT_ERROR("Failed to find entry due to %s", sqlite3_errmsg(single_database_connection));
        return FIND_ERROR;
    }

    if(
        sqlite3_bind_int(results, 1, config_type) != SQLITE_OK ||
        sqlite3_bind_text(results, 2, entry, strlen(entry) + 1, NULL) != SQLITE_OK
    ){
        PRINT_FORMAT_ERROR("Failed to bind parameters for finding entry: %s", sqlite3_errmsg(single_database_connection));
        return FIND_ERROR;
    }

    enum FIND is_found = NOT_FOUND;
    ret_code = sqlite3_step(results);
    if(ret_code == SQLITE_ROW){
        if(sqlite3_column_int(results, 0) > 0){
            is_found = FOUND;
        }
    }

    return is_found;
}

//Figure out how you want to figure out the index once a user inputs
//returning the statement might be easier
enum ERROR listConfigRoots(enum CONFIG config_type){
    assert(single_database_connection != NULL);
    assert(config_type == AUDIO_CONFIG ||
           config_type == VIDEO_CONFIG ||
           config_type == COVER_CONFIG ||
           config_type == BLACK_CONFIG);

    int max_index = getNumOfRowsForConfig(config_type);
    if(max_index == HAD_ERROR){
        return HAD_ERROR;
    }else if(max_index == 0){
        printSectionHeader(config_type);
        puts("No root entries");
        return NO_ERROR;
    }

    const char sql_statement [] = "SELECT root_name, root_depth FROM Roots WHERE root_type == ? ORDER BY root_name;";
    sqlite3_stmt* results = NULL;
    int ret_code = sqlite3_prepare_v2(single_database_connection, sql_statement, -1, &results, NULL);

    if(ret_code != SQLITE_OK){
        PRINT_FORMAT_ERROR("Failed to list due to %s", sqlite3_errmsg(single_database_connection));
        return HAD_ERROR;
    }

    sqlite3_bind_int(results, 1, config_type);

    int count = 1;
    printSectionHeader(config_type);
    while((ret_code = sqlite3_step(results)) == SQLITE_ROW){
        char* root = (char*)sqlite3_column_text(results, 0);
        int depth = sqlite3_column_int(results, 1);
        if(depth == INF_DEPTH){
            printf("%d) %s | INF\n", count, root);
        }else{
            printf("%d) %s | %d\n", count, root, depth);
        }
        ++count;
    }

    return NO_ERROR;
}

enum ERROR listConfigRootsWithPaths(enum CONFIG config_type){
    assert(single_database_connection != NULL);
    assert(config_type == AUDIO_CONFIG ||
           config_type == VIDEO_CONFIG ||
           config_type == COVER_CONFIG);

    const char sql_statement [] =
             "SELECT r.root_id, r.root_name, p.path_name FROM Paths p "
             "RIGHT JOIN Roots r USING (root_id) "
             "WHERE r.root_type == ? "
             "ORDER BY r.root_name, p.path_name;";

    sqlite3_stmt* results = NULL;
    int ret_code = sqlite3_prepare_v2(single_database_connection, sql_statement, -1, &results, NULL);

    if(ret_code != SQLITE_OK){
        PRINT_FORMAT_ERROR("Failed to list due to %s", sqlite3_errmsg(single_database_connection));
        return HAD_ERROR;
    }

    sqlite3_bind_int(results, 1, config_type);

    printSectionHeader(config_type);
    int prev_root_id = -1;
    while((ret_code = sqlite3_step(results)) == SQLITE_ROW){
        int cur_root_id = sqlite3_column_int(results, 0);

        if(cur_root_id != prev_root_id){
            char* root = (char*)sqlite3_column_text(results, 1);
            printf("\t%s\n", root);
        }

        char* path = (char*)sqlite3_column_text(results, 2);
        if(path != NULL){
            printf("\t\t%s\n", path);
        }else{
            puts("\t\tNO ENTRIES");
        }

        prev_root_id = cur_root_id;
    }

    return NO_ERROR;
}

enum ERROR listAllRoots(){
    assert(single_database_connection != NULL);

    const char sql_statement [] = "SELECT root_type, root_name, root_depth FROM Roots ORDER BY root_type, root_name;";
    sqlite3_stmt* results = NULL;
    int ret_code = sqlite3_prepare_v2(single_database_connection, sql_statement, -1, &results, NULL);

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
        printf("\t%s     %d\n", root, depth);
        prev_type = cur_type;
    }

    return NO_ERROR;
}

enum ERROR listAllRootWithPaths(){
    assert(single_database_connection != NULL);

    const char sql_statement [] =
             "SELECT r.root_type, r.root_id, r.root_name, p.path_name, r.root_depth FROM Paths p "
             "RIGHT JOIN Roots r USING (root_id) "
             "ORDER BY r.root_type, r.root_name, p.path_name;";

    sqlite3_stmt* results = NULL;
    int ret_code = sqlite3_prepare_v2(single_database_connection, sql_statement, -1, &results, NULL);

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
                printf("\t%s\n", root);
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

    return NO_ERROR;
}

#ifdef TESTING

void __testingSetDB(sqlite3* testing_db){
    single_database_connection = testing_db;
}

#endif

