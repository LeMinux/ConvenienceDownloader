#include "../includes/databaseOps.h"

static sqlite3* single_database_connection = NULL;

static enum ERROR addEntry(enum CONFIG config, const char* entry, int depth){
    assert(single_database_connection != NULL);
    assert(config == AUDIO_CONFIG || config == VIDEO_CONFIG || config == COVER_CONFIG);
    assert(entry != NULL);
    assert(depth > 0 || depth == INF_DEPTH);

    char sql_statement [100] = "";
    snprintf(sql_statement, 100,"INSERT INTO Roots (config_type, root_name, root_length) VALUES (%d, ?, ?)", config_type);

    sqlite3_stmt* results = NULL;
    int ret_code = sqlite3_prepare_v2(single_database_connection, sql_statement, -1, &results, NULL);

    if(ret_code != SQLITE_OK){
        PRINT_FORMAT_ERROR("Failed to prepare adding %s", sqlite3_errmsg(single_database_connection));
        return HAD_ERROR;
    }

    sqlite3_bind_text(results, 1, user_input, input_length, NULL);
    sqlite3_bind_int(results, 2, input_length);


}

static enum ERROR updateEntry(int index, const char* new_value, int new_depth){
    assert(single_database_connection != NULL);
    assert(index > 0);
    assert(new_value != NULL);
    assert(new_depth > 0 || new_depth == INF_DEPTH);

}

static enum ERROR removeEntry(int index){
    assert(single_database_connection != NULL);
    assert(index > 0);

}


static int findPaths(const char* begin_path, int depth){
    assert(single_database_connection != NULL);

}

enum ERROR initDatabase(void){
    if(sqlite3_open(CONFIG_DATABASE, &single_database_connection) != SQLITE_OK){
        PRINT_ERROR("Could not open configuration database. Have you done make init?");
        return HAD_ERROR;
    }

    /* execute init.sql */

    return NO_ERROR;
}

void editMenu(enum CONFIG config){
    assert(single_database_connection != NULL);
    assert(config == AUDIO_CONFIG ||
           config == VIDEO_CONFIG ||
           config == COVER_CONFIG ||
           config == BLACK_CONFIG);

    enum ERROR err = NO_ERROR;
    if(config != BLACK_CONFIG)
        err = listConfigRoots(config);
    else
        err = listBlacklist();

    if(err){
        PRINT_ERROR("Could not list paths nothing has been changed");
        return;
    }

    int editing = 1;
    char buffer [5] = ""; //buffer of 5 just to see if length is greater than 1
    while(editing){
        printf("What action do you want to take?\n1) Add\n2) Update\n3) Delete\n4) Exit\nEnter the number: ");
        if(exactInput(stdin, buffer, 5) != 2){
            puts("Enter an available number");
            continue;
        }

        switch(buffer[0]){
            case '1': addMenu(config); break;

            case '2': updateMenu(config); break;

            case '3': deleteMenu(config); break;

            case '4': editing = 0; break;

            default: puts("Enter an available number"); break;
        }
    }
}

enum ERROR addMenu(enum CONFIG config_type){

    char path_input [4096] = "";
    char depth_input [15] = "";
    int input_length = 0;
    int valid_input = 0;
    long depth = 0;
    do{
        puts("Enter the path you want to add:");
        input_length = exactInput(stdin, path_input, 4096);
        if(opendir(path_input) != NO_ERROR){
            valid_input = 1;
        }
    }while(!valid_input);

    valid_input = 0;
    do{
        puts("What depth do you want for that path?");
        exactInput(stdin, depth_input, 4096);
        errno = 0;

        if(errno == ERANGE || depth > INT_MAX){
            PRINT_ERROR("Depth given is too large");
            continue;
        }

        if(depth < 0){
            PRINT_ERROR("Depth can't be negative");
            continue;
        }

        valid_input = 1;
    }while(!valid_input);

    addEntry(config_type, const char *entry, depth)


    //bind parameters
    //add to database
    return NO_ERROR
}

enum ERROR updateMenu(enum CONFIG){

}

enum ERROR deleteMenu(enum CONFIG){

}

enum ERROR findEntry(const char* entry){
    assert(single_database_connection != NULL);


}

//Figure out how you want to figure out the index once a user inputs
//returning the statement might be easier
enum ERROR listConfigRoots(enum CONFIG config_type){
    assert(single_database_connection != NULL);
    assert(config_type == AUDIO_CONFIG || config_type == VIDEO_CONFIG || config_type == COVER_CONFIG);

    char sql_statement [100] = "";
    snprintf(sql_statement, sizeof(sql_statement), "SELECT root_name, depth FROM Roots WHERE config_type == %d;", config_type);
    sqlite3_stmt* results = NULL;
    int ret_code = sqlite3_prepare_v2(single_database_connection, sql_statement, -1, &results, NULL);

    if(ret_code != SQLITE_OK){
        PRINT_FORMAT_ERROR("Failed to list due to %s", sqlite3_errmsg(single_database_connection));
        return HAD_ERROR;
    }

    int count = 1;
    puts("index) root ");
    //just to print something for now
    printf("%s%-10s%s", "index", "root path", "depth");
    printf("%-25s", "-");
    while((ret_code = sqlite3_step(results)) == SQLITE_ROW){
        char* root = (char*)sqlite3_column_text(results, 0);
        int depth = sqlite3_column_int(results, 1);
        printf("%d) %s | %d", count, root, depth);
        ++count;
    }

    return NO_ERROR;
}

enum ERROR listConfigRootsWithPaths(enum CONFIG config_type){
    assert(single_database_connection != NULL);

    char sql_statement [130] = "";
    snprintf(sql_statement, 130, "SELECT root_id, root_name, path_name FROM Paths INNER JOIN Roots WHERE config_type == %d ORDER BY config_type, root_id;", config_type);

    sqlite3_stmt* results = NULL;
    int ret_code = sqlite3_prepare_v2(single_database_connection, sql_statement, -1, &results, NULL);

    if(ret_code != SQLITE_OK){
        PRINT_FORMAT_ERROR("Failed to list due to %s", sqlite3_errmsg(single_database_connection));
        return HAD_ERROR;
    }

    int prev_root_id = -1;
    while((ret_code = sqlite3_step(results)) == SQLITE_ROW){
        int cur_root_id = sqlite3_column_int(results, 0);

        if(cur_root_id != prev_root_id){
            char* root = (char*)sqlite3_column_text(results, 1);
            printf("\t%s\n", root);
        }

        char* path = (char*)sqlite3_column_text(results, 3);
        printf("\t\t%s\n", path);

        prev_root_id = cur_root_id;
    }

    return NO_ERROR;
}

enum ERROR listAllRoots(){
    assert(single_database_connection != NULL);

    char sql_statement [100] = "";
    snprintf(sql_statement, 100, "SELECT config_type, root_name, depth FROM Roots ORDER BY config_type;");

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
            switch(cur_type){
                case AUDIO_CONFIG:
                    puts("AUDIO ROOTS");
                break;

                case VIDEO_CONFIG:
                    puts("VIDEO ROOTS");
                break;

                case COVER_CONFIG:
                    puts("COVER ROOTS");
                break;
            }
        }
        printf("\t%s     %d", root, depth);
        prev_type = cur_type;
    }

    return NO_ERROR;
}

enum ERROR listAllRootWithPaths(){
    assert(single_database_connection != NULL);

    char sql_statement [120] = "";
    snprintf(sql_statement, 120, "SELECT config_type, root_id, root_name, path_name, depth FROM Paths INNER JOIN Roots ORDER BY config_type, root_id;");

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
            switch(cur_type){
                case AUDIO_CONFIG:
                    puts("AUDIO ROOTS");
                break;

                case VIDEO_CONFIG:
                    puts("VIDEO ROOTS");
                break;

                case COVER_CONFIG:
                    puts("COVER ROOTS");
                break;

                case BLACK_CONFIG:
                    //TODO IF EVEN VIABLE
                break;
            }
        }

        if(cur_root_id != prev_root_id){
            char* root = (char*)sqlite3_column_text(results, 2);
            int depth = sqlite3_column_int(results, 4);
            printf("\t%s\t%d\n", root, depth);
        }

        char* path = (char*)sqlite3_column_text(results, 3);
        printf("\t\t%s\n", path);

        prev_root_id = cur_root_id;
        prev_type = cur_type;
    }

    return NO_ERROR;
}

enum ERROR listBlacklist(){
    assert(single_database_connection != NULL);

    char sql_statement [100] = "";
    snprintf(sql_statement, 100, "SELECT black_path FROM Blacklist ORDER BY black_path;");

    sqlite3_stmt* results = NULL;
    int ret_code = sqlite3_prepare_v2(single_database_connection, sql_statement, -1, &results, NULL);

    if(ret_code != SQLITE_OK){
        PRINT_FORMAT_ERROR("Failed to list due to %s", sqlite3_errmsg(single_database_connection));
        return HAD_ERROR;
    }

    puts("BLACK LIST");
    while((ret_code = sqlite3_step(results)) == SQLITE_ROW){
        char* path = (char*)sqlite3_column_text(results, 0);
        printf("\t%s", path);
    }

    return NO_ERROR;
}
