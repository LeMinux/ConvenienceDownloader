#include "../includes/download_video_test.h"

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
static void assertMetaData(const char* path, const char* meta_content, int exp_ret){
    char command [100];
    size_t len = snprintf(command, sizeof(command), "exiftool '%s'* | grep --fixed-strings '%s'", path, meta_content);
    if(len >= sizeof(command)){
        fail_msg("command buffer is too short you need to extend it");
    }

    int ret = system(command);
    assert_true(ret == exp_ret || WEXITSTATUS(ret) == exp_ret);
}

void testDownloadVideoNoMetaData(void** state){
    sqlite3* database = *state;
    const char* exp_path = VIDEO_ROOT PATH_1;
    MetaData_t data = {NULL, NULL, NULL};

    addEntry(database);

    downloadVideo("https://www.youtube.com/watch?v=yxmFztbq234", PATH_1_ID, &data);

    assertDownloaded(exp_path);
    assertMetaData(exp_path, data.genre,  GREP_NO_FOUND);
    assertMetaData(exp_path, data.artist, GREP_NO_FOUND);
    assertMetaData(exp_path, data.album, GREP_NO_FOUND);
}

void testDownloadVideoAllMetaData(void** state){
    sqlite3* database = *state;
    const char genre [] = "Meme";
    const char artist [] = "Dont Care";
    const char album [] = "MegaFunniez";
    const char* exp_path = VIDEO_ROOT PATH_2;
    MetaData_t data = {.genre=genre, .artist=artist, .album=album};

    addEntry(database);

    downloadVideo("https://www.youtube.com/watch?v=vVC6HJQEVjo", PATH_2_ID, &data);

    assertDownloaded(exp_path);
    assertMetaData(exp_path, data.genre,  GREP_FOUND);
    assertMetaData(exp_path, data.artist, GREP_FOUND);
    assertMetaData(exp_path, data.album, GREP_FOUND);
}


void testDownloadVideoWeirdMetaData(void** state){
    sqlite3* database = *state;
    const char genre [] = "%titles.%exts";
    const char artist [] = "?P<meta_synopsis>";
    const char album [] = " ?!@#$%^&*_-~+=.<>|";
    const char* exp_path = VIDEO_ROOT PATH_3;
    MetaData_t data = {.genre=genre, .artist=artist, .album=album};

    addEntry(database);

    downloadVideo("https://www.youtube.com/watch?v=h7zdroTvMkE", PATH_3_ID, &data);

    assertDownloaded(exp_path);
    assertMetaData(exp_path, data.genre,  GREP_FOUND);
    assertMetaData(exp_path, data.artist, GREP_FOUND);
    assertMetaData(exp_path, data.album, GREP_FOUND);
}
