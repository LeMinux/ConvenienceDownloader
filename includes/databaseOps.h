#ifndef DATABASEOPS_H
#define DATABASEOPS_H

#include <sqlite3.h>
#include <pwd.h>

#include "globals.h"
#include "userInput.h"
#include "databaseUserInput.h"

enum OPTIONS {ADD_OPT = '1', UPT_OPT, DEL_OPT, EXT_OPT};
enum FIND {FIND_ERROR = -1, NOT_FOUND, FOUND};

//Home path is found at run time
#define CONFIG_DATABASE ".config/con-downloader/con-downloader.db"

//since the db connection is static to this file inorder to use different
//connections calls within the file have to be done
//Now yes to make testing easier I could return a sqlite*.
//However, I want to experiment with file bound variables, and what that means for testing.
//I also want to see what advantages/disadvantages there are.
#ifdef TESTING

#define TESTING_CONFIG_DB ":memory:"

void __testingSetDB(sqlite3* testing_db);

#endif

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
*   Brings up the add menu interface
*/
void addMenu(enum CONFIG config_type);

/*
*   Brings up the update menu interface
*/
void updateMenu(enum CONFIG config_type);

/*
*   Brings up the delete menu interface
*/
void deleteMenu(enum CONFIG config_type);

enum FIND findEntry(enum CONFIG config_type, const char* entry);

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

#endif

