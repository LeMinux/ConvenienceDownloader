#include "../includes/download_video_test.h"

enum GREP {FOUND = 0, NOT_FOUND};

static void getRealPath(const char* input, char result [PATH_MAX]){
    if(realpath(input, result) == NULL){
        fail_msg("Could not create real path");
    }
}

//only the db really needs the real path
//testing can use a relative path
static void addEntry(sqlite3* database){
    char test_path [PATH_MAX];
    getRealPath(VIDEO_ROOT, test_path);
    addExtraRootEntry(database, VIDEO_CONFIG, test_path, 0);
    addExtraPathEntry(database, 1, ROOT_ITSELF);
    addExtraPathEntry(database, 1, PATH_1);
    addExtraPathEntry(database, 1, PATH_2);
    addExtraPathEntry(database, 1, PATH_3);
    addExtraPathEntry(database, 1, PATH_4);
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

    //3 because there is . and .. special dirs
    if(file_count < 3){
        fail_msg("Did not download to %s\n", path);
    }else if(file_count > 3){
        fail_msg("Download location should only have 1 file");
    }
}

//Don't like the usage of system, but I am in control of what is executed
//Assuming that I don't change PATH, IFS, or what ever werid shell stuff
static void assertMetaData(const char* path, const char* meta_content, enum GREP exp_ret){
    char command [100];
    int len = snprintf(command, sizeof(command), "exiftool '%s'* | grep --fixed-strings '%s'", path, meta_content);
    if(len < 0 || len >= (int)sizeof(command)){
        fail_msg("command buffer is too short you need to extend it");
    }

    enum GREP ret = system(command);
    if(!WIFEXITED(ret)) fail_msg("Couldn't conduct the system command");

    const char* fail_message = NULL;
    const char expecting_find [] = "Expected to FIND metadata it was NOT FOUND:";
    const char expecting_not_find [] = "Expected to NOT FIND metadta but it was FOUND:";

    if(exp_ret == FOUND) fail_message = expecting_find;
    else if(exp_ret == NOT_FOUND) fail_message = expecting_not_find;
    else fail_msg("Grep returned a different error code");

    if(WEXITSTATUS(ret) != exp_ret) fail_msg("%s '%s'", fail_message, meta_content);

}

void testDownloadVideoNoMetaDataUsesOnPageStats(void** state){
    sqlite3* database = *state;
    const char* exp_path = VIDEO_ROOT PATH_1;
    MetaData_t data = {0};

    addEntry(database);

    downloadVideo("https://www.youtube.com/watch?v=yxmFztbq234", PATH_1_ID, &data);

    assertDownloaded(exp_path);
    assertMetaData(exp_path, "Entertainment",  FOUND);        //genre on page
    assertMetaData(exp_path, "The Microwave Network", FOUND); //artist on page
    assertMetaData(exp_path, "Album", NOT_FOUND);             //album on page
}

void testDownloadVideoAllMetaData(void** state){
    sqlite3* database = *state;
    const char* exp_path = VIDEO_ROOT PATH_2;
    MetaData_t data = {.genre="Meme", .artist="Dont Care", .album="MegaFunniez"};

    addEntry(database);

    downloadVideo("https://www.youtube.com/watch?v=vVC6HJQEVjo", PATH_2_ID, &data);

    assertDownloaded(exp_path);
    assertMetaData(exp_path, data.genre,  FOUND);
    assertMetaData(exp_path, data.artist, FOUND);
    assertMetaData(exp_path, data.album, FOUND);
}


void testDownloadVideoWeirdMetaData(void** state){
    sqlite3* database = *state;
    const char* exp_path = VIDEO_ROOT PATH_3;
    MetaData_t data = {
        .genre="%titles.%exts",
        .artist="?P<meta_synopsis>",
        .album=" ?!@#$%^&*_-~+=.<>|"
    };

    addEntry(database);

    downloadVideo("https://www.youtube.com/watch?v=h7zdroTvMkE", PATH_3_ID, &data);

    assertDownloaded(exp_path);
    assertMetaData(exp_path, data.genre,  FOUND);
    assertMetaData(exp_path, data.artist, FOUND);
    assertMetaData(exp_path, data.album, FOUND);
}

void testDownloadVideoPartialMetaDataFillsRestWithPageContent(void** state){
    sqlite3* database = *state;
    const char* exp_path = VIDEO_ROOT PATH_4;
    MetaData_t data = {.genre="Meme", .artist="", .album="MegaFunniez"};

    addEntry(database);

    downloadVideo("https://www.youtube.com/watch?v=yxmFztbq234", PATH_4_ID, &data);

    assertDownloaded(exp_path);
    assertMetaData(exp_path, data.genre,  FOUND);
    assertMetaData(exp_path, "The Microwave Network", FOUND);
    assertMetaData(exp_path, data.album, FOUND);
}
