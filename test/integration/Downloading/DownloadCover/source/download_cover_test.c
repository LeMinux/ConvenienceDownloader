#include "../includes/download_cover_test.h"

static void getRealPath(const char* input, char result [PATH_MAX]){
    if(realpath(input, result) == NULL){
        fail_msg("Could not create real path");
    }
}

//only the db really needs the real path
//testing can use a relative path
static void addEntry(sqlite3* database){
    char test_path [PATH_MAX];
    getRealPath(COVER_ROOT, test_path);
    addExtraRootEntry(database, COVER_CONFIG, test_path, 0);
    addExtraPathEntry(database, 1, ROOT_ITSELF);
    addExtraPathEntry(database, 1, PATH_1);
}

static void assertDownloaded(const char* path){
    DIR* dir = opendir(path);
    if(dir == NULL){
        fail_msg("Could not open directory");
    }

    int file_count = 0;
    struct dirent* dir_entry;
    while((dir_entry = readdir(dir)) != NULL){
        ++file_count;
    }
    //3 because there are . and .. special dirs
    if(file_count < 3){
        fail_msg("Did not download to %s\n", path);
    }else if(file_count > 3){
        fail_msg("Download location should only have 1 file");
    }
}


void testDownloadCoverDownloadsCoverArt(void** state){
    sqlite3* database = *state;
    const char* exp_path = COVER_ROOT PATH_1;

    addEntry(database);

    downloadCover("https://www.youtube.com/watch?v=ifyatp9yYUQ", PATH_1_ID);

    assertDownloaded(exp_path);
}
