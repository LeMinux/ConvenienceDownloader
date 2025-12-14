#ifndef DATABASEOPS_H
#define DATABASEOPS_H

#include <sqlite3.h>

#include "helpers.h"
#include "globals.h"
#include "userInput.h"

#define INF_DEPTH -1

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

enum ERROR addMenu(enum CONFIG);
enum ERROR updateMenu(enum CONFIG);
enum ERROR deleteMenu(enum CONFIG);

enum ERROR findEntry(const char* entry);
enum ERROR inBlacklist(const char* dir_path);
enum ERROR addToBlacklist(const char* dir_path);

/*
*   Lists every root from every config
*
*/
enum ERROR listAllRoots();

/*
*   Lists every root from every config, and their associated paths
*
*/
enum ERROR listAllRootWithPaths();

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

enum ERROR listBlacklist();

void editMenu(enum CONFIG config_file);

#endif

