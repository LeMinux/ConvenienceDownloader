#include "../includes/databaseOps.h"

static sqlite3* single_database_connection = NULL;

static enum ERROR addEntry(enum CONFIG config, const char* entry, int depth){
    assert(single_database_connection != NULL);
    assert(config == AUDIO_CONFIG || config == VIDEO_CONFIG || config == COVER_CONFIG);
    assert(entry != NULL);
    assert(depth > 0 || depth == INF_DEPTH);

    char sql_statement [100] = "";
    snprintf(sql_statement, 100,"INSERT INTO Roots (root_type, root_name, root_length) VALUES (?, ?, ?)");

    sqlite3_stmt* results = NULL;
    int ret_code = sqlite3_prepare_v2(single_database_connection, sql_statement, -1, &results, NULL);

    if(ret_code != SQLITE_OK){
        PRINT_FORMAT_ERROR("Failed to prepare adding %s", sqlite3_errmsg(single_database_connection));
        return HAD_ERROR;
    }

    sqlite3_bind_int(results, 1, config);
    sqlite3_bind_text(results, 2, entry, strlen(entry), NULL);
    sqlite3_bind_int(results, 3, depth);

    //executte sql

    return NO_ERROR;
}

static void addMenu(enum CONFIG config_type){
    assert(single_database_connection != NULL);
    assert(config_type == AUDIO_CONFIG ||
           config_type == VIDEO_CONFIG ||
           config_type == COVER_CONFIG ||
           config_type == BLACK_CONFIG);

    char* path_input;
    int depth = 0;
    while((path_input = takeDirectoryInput()) != NULL) {}

    if(config_type != BLACK_CONFIG){
        while((depth = takeDepthInput()) != INVALID);
    }

    addEntry(config_type, path_input, depth);
    free(path_input);
    path_input = NULL;
}

static enum ERROR updateEntry(int index, const char* new_value, int new_depth){
    assert(single_database_connection != NULL);
    assert(index > 0);
    assert(new_value != NULL);
    assert(new_depth > 0 || new_depth == INF_DEPTH);

    char sql_statement [] = "UPDATE FROM Roots SET root_name = ?, root_depth = ? WHERE root_id = ?";

    sqlite3_stmt* results = NULL;
    int ret_code = sqlite3_prepare_v2(single_database_connection, sql_statement, -1, &results, NULL);

    if(ret_code != SQLITE_OK){
        PRINT_FORMAT_ERROR("Failed to prepare updating statement %s", sqlite3_errmsg(single_database_connection));
        return HAD_ERROR;
    }

    sqlite3_bind_text(results, 1, new_value, strlen(new_value), NULL);
    sqlite3_bind_int(results, 2, new_depth);
    sqlite3_bind_int(results, 3, index);

    //execute sql

    return NO_ERROR;
}

static enum ERROR removeEntry(int index){
    assert(single_database_connection != NULL);
    assert(index > 0);

    char sql_statement [100] = "";
    snprintf(sql_statement, 100, "DELETE FROM Roots WHERE root_id = ?");

    sqlite3_stmt* results = NULL;
    int ret_code = sqlite3_prepare_v2(single_database_connection, sql_statement, -1, &results, NULL);

    if(ret_code != SQLITE_OK){
        PRINT_FORMAT_ERROR("Failed to prepare deleting %s", sqlite3_errmsg(single_database_connection));
        return HAD_ERROR;
    }

    sqlite3_bind_int(results, 1, index);

    //execute sql

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

static enum ERROR updateMenu(enum CONFIG config_type){

    return NO_ERROR;

}

static enum ERROR deleteMenu(enum CONFIG config_type){

    return NO_ERROR;
}


static void translateOptionToRow(size_t user_selection){
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
        printf("What action do you want to take?\n%d) Add\n%d) Update\n%d) Delete\n%d) Exit\nEnter the number: ", ADD, UPT, DEL, EXT);
        if(boundedInput(stdin, buffer, 5) != 1){
            puts("Input only needs to be a single digit");
            continue;
        }

        switch(buffer[0]){
            case ADD: addMenu(config); break;

            case UPT: updateMenu(config); break;

            case DEL: deleteMenu(config); break;

            case EXT: editing = 0; break;

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
    assert(config_type == AUDIO_CONFIG || config_type == VIDEO_CONFIG || config_type == COVER_CONFIG);

    char sql_statement [] = "SELECT root_name, root_depth FROM Roots WHERE root_type == ?;";
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

    char sql_statement [] =
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

    char sql_statement [] = "SELECT root_type, root_name, root_depth FROM Roots ORDER BY root_type;";
    sqlite3_stmt* results = NULL;
    int ret_code = sqlite3_prepare_v2(single_database_connection, sql_statement, -1, &results, NULL);

    if(ret_code != SQLITE_OK){
        PRINT_FORMAT_ERROR("Failed to list due to %s", sqlite3_errmsg(single_database_connection));
        return HAD_ERROR;
    }

    int prev_type = -1;
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

    char sql_statement [] =
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

