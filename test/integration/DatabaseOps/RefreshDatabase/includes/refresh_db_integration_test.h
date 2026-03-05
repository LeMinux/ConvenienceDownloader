#ifndef REFRESH_DB_INTEGRATION_TESTING
#define REFRESH_DB_INTEGRATION_TESTING

#include <string.h>
#include <stdio.h>

#include "testGlobals.h"
#include "testSetups.h"
#include "databaseOps.h"

#define NOT_FOUND -1

#define DIR_ENV "./dir_env"

typedef struct Root_Init{
    enum CONFIG type;
    const char* dir_name;
}Root_Init_t;

typedef struct Exp_Root_Info{
    Root_Init_t root_entry;
    int num_of_paths;
    const char** paths;
}Exp_Root_Info_t;

void testRefreshDatabase(void** state);

#endif
