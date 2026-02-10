#ifndef DATABASEOPS_H
#define DATABASEOPS_H

#include <sqlite3.h>
#include <pwd.h>

#include "globals.h"
#include "userInput.h"

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
*
*   return: NO_ERROR if could get database HAD_ERROR otherwise
*/
enum ERROR initDatabase(void);

/*
*   Function to refresh the database by checking what paths still exists
*
*   return:
*   NO_ERROR if all entries have been refreshed.
*   HAD_ERROR if a database error occured. rollback is handled in the method
*
*/
enum ERROR refreshDatabase(void);

/*
*   Brings up the add menu interface. This interface will also handle
*   commits and rollbacks as it's the outer most interface.
*
*   config_type: filter for what should be changed
*
*/
void addMenu(enum CONFIG config_type);

/*
*   Brings up the update menu interface. This interface will also handle
*   commits and rollbacks as it's the outer most interface.
*
*   config_type: filter for what should be changed
*
*/
void updateMenu(enum CONFIG config_type);

/*
*   Brings up the delete menu interface. This interface will also handle
*   commits and rollbacks as it's the outer most interface.
*
*   config_type: filter for what should be changed
*
*/
void deleteMenu(enum CONFIG config_type);

/*
*   Lists every root from every config
*
*   return:
*   NO_ERROR if could list everything
*   HAD_ERROR if a database error occured. No data is written to as this just reads
*
*/
enum ERROR listAllRoots(void);

/*
*   Lists every root from every config, and their associated paths
*
*   return:
*   NO_ERROR if could list everything
*   HAD_ERROR if a database error occured. No data is written to as this just reads
*/
enum ERROR listAllRootWithPaths(void);

/*
*   Lists the roots from one config
*
*   config_type: filter for what should be listed
*
*   return:
*   NO_ERROR if could list everything
*   HAD_ERROR if a database error occured. No data is written to as this just reads
*
*/
enum ERROR listConfigRoots(enum CONFIG config_type);

/*
*   Lists the roots from one config with their associated paths
*
*   config_type: filter for what should be listed
*
*   return:
*   NO_ERROR if could list everything
*   HAD_ERROR if a database error occured. No data is written to as this just reads
*/
enum ERROR listConfigRootsWithPaths(enum CONFIG config_type);


/*
*   Returns how many total path entries there are for a config type
*
*   config_type: filter for what should be counted
*
*   return:
*   Number of path rows
*   HAD_ERROR if database error occured
*
*/
int getNumOfPathRowsForConfig(enum CONFIG config);

/*
*   Returns the path id based on the selected index derived from
*   the ORDER BY statement in the SQL listing.
*
*   user_selection: The index selection a user made based off ORDER BY output
*   config_type: filter for what should be counted
*
*   return:
*   the path id in the database
*   HAD_ERROR if database error occured
*/
int translatePathIndexToRow(int user_selection, enum CONFIG config_type);

/*
*   Takes the path id and sets the absolute path into full_path.
*   The absolute path is a concatenation of the root_name and path_name from
*   the database.
*
*   path_id: id of a path in the database
*   full_path: A string of at least PATH_MAX size that will be written into
*
*   return:
*   NO_ERROR if could create name
*   HAD_ERROR if name is too long, or if database had an error
*/
enum ERROR pathIDToPath(int path_id, char* full_path);

#endif

