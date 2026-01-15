#include "../includes/databaseOps.h"

static sqlite3* single_database_connection = NULL;

static int getNumOfRowsForConfig(enum CONFIG config){
    assert(single_database_connection != NULL);
    assert(config == AUDIO_CONFIG ||
           config == VIDEO_CONFIG ||
           config == COVER_CONFIG ||
           config == BLACK_CONFIG);

    //root_id is indexed so the count should be faster
    const char sql_statement [] = "SELECT COUNT(root_id) FROM Roots";
    sqlite3_stmt* results = NULL;
    int ret_code = sqlite3_prepare_v2(single_database_connection, sql_statement, -1, &results, NULL);

    if(ret_code != SQLITE_OK){
        PRINT_FORMAT_ERROR("Failed to obtain count of rows: %s", sqlite3_errmsg(single_database_connection));
        return HAD_ERROR;
    }

    ret_code = sqlite3_step(results);
    if(ret_code != SQLITE_ROW){
        PRINT_FORMAT_ERROR("Could not obtain count of rows: %s", sqlite3_errmsg(single_database_connection));
        return HAD_ERROR;
    }

    return sqlite3_column_int(results, 0);
}

static enum ERROR addEntry(enum CONFIG config, const char* entry, int depth){
    assert(single_database_connection != NULL);
    assert(entry != NULL && entry[0] != '\0');
    assert(depth >= 0 || depth == INF_DEPTH);
    assert(config == AUDIO_CONFIG ||
           config == VIDEO_CONFIG ||
           config == COVER_CONFIG ||
           config == BLACK_CONFIG);


    const char sql_statement [] = "INSERT INTO Roots (root_type, root_name, root_length, root_depth) VALUES (?, ?, ?, ?)";
    sqlite3_stmt* results = NULL;
    int ret_code = sqlite3_prepare_v2(single_database_connection, sql_statement, -1, &results, NULL);

    if(ret_code != SQLITE_OK){
        PRINT_FORMAT_ERROR("Failed to prepare adding %s", sqlite3_errmsg(single_database_connection));
        return HAD_ERROR;
    }

    if(sqlite3_bind_int(results, 1, config) != SQLITE_OK ||
    sqlite3_bind_text(results, 2, entry, strlen(entry), NULL) != SQLITE_OK ||
    sqlite3_bind_int(results, 3, strlen(entry)) != SQLITE_OK ||
    sqlite3_bind_int(results, 4, depth) != SQLITE_OK){
        PRINT_ERROR("Failed to bind parameters. No changes have been made");
        return HAD_ERROR;
    }

    ret_code = sqlite3_step(results);
    enum ERROR func_return = HAD_ERROR;
    switch(ret_code){
        case SQLITE_DONE: ADVISE_USER("Added entry!"); func_return = NO_ERROR; break;
        case SQLITE_MISUSE: PRINT_ERROR("Dumb idiot programmer made a mistake"); break;
        case SQLITE_ERROR: PRINT_FORMAT_ERROR("Database error: %s", sqlite3_errmsg(single_database_connection)); break;
        case SQLITE_BUSY: ADVISE_USER("Database was busy. Did not add entry. You can try again."); break;
        default: PRINT_ERROR("Error in database prevented adding the entry"); break;
    }

    return func_return;
}

static void addMenu(enum CONFIG config_type){
    assert(single_database_connection != NULL);
    assert(config_type == AUDIO_CONFIG ||
           config_type == VIDEO_CONFIG ||
           config_type == COVER_CONFIG ||
           config_type == BLACK_CONFIG);

    char* path_input;
    int depth = 0;
    while((path_input = takeDirectoryInput()) == NULL) {}

    if(config_type != BLACK_CONFIG){
        while((depth = takeDepthInput()) == INVALID);
    }

    (void)addEntry(config_type, path_input, depth);
    free(path_input);
    path_input = NULL;
}

static enum ERROR updateEntry(int index, const char* new_value, int new_depth){
    assert(single_database_connection != NULL);
    assert(index > 0);
    assert(new_value != NULL);
    assert(new_depth > 0 || new_depth == INF_DEPTH);

    const char sql_statement [] = "UPDATE FROM Roots SET root_name = ?, root_depth = ? WHERE root_id = ?";

    sqlite3_stmt* results = NULL;
    int ret_code = sqlite3_prepare_v2(single_database_connection, sql_statement, -1, &results, NULL);

    if(ret_code != SQLITE_OK){
        PRINT_FORMAT_ERROR("Failed to prepare updating statement: %s", sqlite3_errmsg(single_database_connection));
        return HAD_ERROR;
    }

    sqlite3_bind_text(results, 1, new_value, strlen(new_value), NULL);
    sqlite3_bind_int(results, 2, new_depth);
    sqlite3_bind_int(results, 3, index);

    ret_code = sqlite3_step(results);
    enum ERROR func_return = HAD_ERROR;
    switch(ret_code){
        case SQLITE_DONE: ADVISE_USER("Updated entry!"); func_return = NO_ERROR; break;
        case SQLITE_MISUSE: PRINT_ERROR("Dumb idiot programmer made a mistake"); break;
        case SQLITE_ERROR: PRINT_FORMAT_ERROR("Database error: %s", sqlite3_errmsg(single_database_connection)); break;
        case SQLITE_BUSY: ADVISE_USER("Database was busy. Did not update the entry. You can try again."); break;
        default: PRINT_ERROR("Error in database prevented updating the entry"); break;
    }

    return func_return;
}

static enum ERROR removeEntry(int index){
    assert(single_database_connection != NULL);
    assert(index > 0);

    const char sql_statement [] = "DELETE FROM Roots WHERE root_id = ?";

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
        case SQLITE_DONE: ADVISE_USER("Deleted entry!"); func_return = NO_ERROR; break;
        case SQLITE_MISUSE: PRINT_ERROR("Dumb idiot programmer made a mistake"); break;
        case SQLITE_ERROR: PRINT_FORMAT_ERROR("Database error: %s", sqlite3_errmsg(single_database_connection)); break;
        case SQLITE_BUSY: ADVISE_USER("Database was busy. Did not delete the entry. You can try again."); break;
        default: PRINT_ERROR("Error in database prevented deleting the entry"); break;
    }

    return func_return;

    return NO_ERROR;

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

static void updateMenu(enum CONFIG config_type){
    assert(single_database_connection != NULL);
    assert(config_type == AUDIO_CONFIG ||
           config_type == VIDEO_CONFIG ||
           config_type == COVER_CONFIG ||
           config_type == BLACK_CONFIG);

    int max_index = getNumOfRowsForConfig(config_type);
    if(max_index == HAD_ERROR) return;
    int select_row = INVALID;
    while((select_row = takeIndexInput(max_index)) == INVALID){}

    char* path_input;
    int depth = 0;
    while((path_input = takeDirectoryInput()) == NULL) {}

    if(config_type != BLACK_CONFIG){
        while((depth = takeDepthInput()) == INVALID);
    }

    (void)updateEntry(config_type, path_input, depth);
    free(path_input);
    path_input = NULL;
    return NO_ERROR;
}

static enum ERROR deleteMenu(enum CONFIG config_type){

    return NO_ERROR;
}


static int translateOptionToRow(int user_selection){
/*
 *SELECT *
FROM my_table
ORDER BY some_column
LIMIT 1 OFFSET ?;
 *
 * ? is user_selection - 1
 */
}

enum ERROR initDatabase(void){
    if(sqlite3_open(CONFIG_DATABASE, &single_database_connection) != SQLITE_OK){
        PRINT_ERROR("Could not open configuration database. Have you done make init?");
        return HAD_ERROR;
    }

    return NO_ERROR;
}

void editMenu(enum CONFIG config){
    assert(single_database_connection != NULL);
    assert(config == AUDIO_CONFIG ||
           config == VIDEO_CONFIG ||
           config == COVER_CONFIG ||
           config == BLACK_CONFIG);

    enum ERROR err = NO_ERROR;
    err = listConfigRoots(config);

    if(err){
        PRINT_ERROR("Could not list paths nothing has been changed");
        return;
    }

    int editing = 1;
    char buffer [5] = ""; //buffer of 5 just to see if length is greater than 1
    while(editing){
        printf("What action do you want to take?\n%c) Add\n%c) Update\n%c) Delete\n%c) Exit\nEnter the number: ", ADD_OPT, UPT_OPT, DEL_OPT, EXT_OPT);
        if(boundedInput(stdin, buffer, 5) != 1){
            puts("Input only needs to be a single digit");
            continue;
        }

        switch(buffer[0]){
            case ADD_OPT: addMenu(config); break;

            case UPT_OPT: updateMenu(config); break;

            case DEL_OPT: deleteMenu(config); break;

            case EXT_OPT: editing = 0; break;

            default: ADVISE_USER("Enter an available number"); break;
        }
    }
}

/*
enum ERROR findEntry(const char* entry){
    assert(single_database_connection != NULL);


}
*/

//Figure out how you want to figure out the index once a user inputs
//returning the statement might be easier
enum ERROR listConfigRoots(enum CONFIG config_type){
    assert(single_database_connection != NULL);
    assert(config_type == AUDIO_CONFIG ||
           config_type == VIDEO_CONFIG ||
           config_type == COVER_CONFIG ||
           config_type == BLACK_CONFIG);

    const char sql_statement [] = "SELECT root_name, root_depth FROM Roots WHERE root_type == ?;";
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
        printf("%d) %s | %d\n", count, root, depth);
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
             "ORDER BY r.root_id;";

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

    const char sql_statement [] = "SELECT root_type, root_name, root_depth FROM Roots ORDER BY root_type;";
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
             "ORDER BY r.root_type, r.root_id;";

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

