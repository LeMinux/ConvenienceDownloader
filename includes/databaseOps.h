#ifndef DATABASEOPS_H
#define DATABASEOPS_H

#include <sqlite3.h>

#include "helpers.h"
#include "globals.h"
#include "userInput.h"

#define ADD_OPTION 1
#define UPT_OPTION 2
#define DEL_OPTION 3
#define EXT_OPTION 4

enum OPTIONS {ADD = '1', UPT, DEL, EXT};

#define CONFIG_DATABASE "~/.config/con-downloader/con-downloader.db"

/*
*   Function to initialize the static member in databaseOps.c, so that
*   only one connection needs to be worried about in a contained file
*/
enum ERROR initDatabase(void);

/*
*   Function to refresh the database by checking what paths still exists
*/
enum ERROR refreshDatabase(void);

/*
enum ERROR addMenu(enum CONFIG config_type);
enum ERROR updateMenu(enum CONFIG config_type);
enum ERROR deleteMenu(enum CONFIG config_type);
*/

enum ERROR findEntry(const char* entry);

/*
*   Lists every root from every config
*
*/
enum ERROR listAllRoots(void);

/*
*   Lists every root from every config, and their associated paths
*
*/
enum ERROR listAllRootWithPaths(void);

/*
*   Lists the roots from one config
*
*/
enum ERROR listConfigRoots(enum CONFIG config_type);

/*
*   Lists the roots from one config with their associated paths
*
*/
enum ERROR listConfigRootsWithPaths(enum CONFIG config_type);

void editMenu(enum CONFIG config_file);

//since the db connection is static to this file inorder to use different
//connections calls within the file have to be done
//Now yes to make testing easier I could add a sqlite* parameter to the functions.
//However, I want to experiment with file bound variables, and what that means for testing.
//I also want to see what advantages/disadvantages there are.
#ifdef TESTING

#define TESTING_CONFIG_DB ":memory:"

void __testingSetDB(sqlite3* testing_db);

#endif

#endif

