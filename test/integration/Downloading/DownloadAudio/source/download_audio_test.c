#include "../includes/download_audio_test.h"

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
    getRealPath(AUDIO_ROOT, test_path);
    addExtraRootEntry(database, AUDIO_CONFIG, test_path, 0);
    addExtraPathEntry(database, 1, ROOT_ITSELF);
    addExtraPathEntry(database, 1, PATH_1);
    addExtraPathEntry(database, 1, PATH_2);
    addExtraPathEntry(database, 1, PATH_3);
    addExtraPathEntry(database, 1, PATH_4);
    addExtraPathEntry(database, 1, PATH_5);
    addExtraPathEntry(database, 1, PATH_6);
    addExtraPathEntry(database, 1, PATH_7);
    addExtraPathEntry(database, 1, PATH_8);
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
    const char* fail_message = NULL;
    const char expecting_find [] = "Expected to FIND metadata it was NOT FOUND:";
    const char expecting_not_find [] = "Expected to NOT FIND metadta but it was FOUND:";

    if(exp_ret == FOUND) fail_message = expecting_find;
    else if(exp_ret == NOT_FOUND) fail_message = expecting_not_find;
    else fail_msg("Grep returned a different error code");

    if(WEXITSTATUS(ret) != exp_ret) fail_msg("%s '%s'", fail_message, meta_content);

}

static void assertCover(const char* path, enum GREP exp_ret){
    char command [100];
    size_t len = snprintf(command, sizeof(command), "exiftool '%s'* | grep Picture", path);
    if(len >= sizeof(command)){
        fail_msg("command buffer is too short you need to extend it");
    }

    enum GREP ret = system(command);
    if(!WIFEXITED(ret)) fail_msg("Couldn't conduct the system command");

    const char* fail_message = NULL;
    const char expecting_cover [] = "Expected to FIND cover art but it was NOT FOUND";
    const char not_expecting_cover [] = "Expected to NOT FIND cover but it was FOUND";

    if(exp_ret == FOUND) fail_message = expecting_cover;
    else if(exp_ret == NOT_FOUND) fail_message = not_expecting_cover;
    else fail_msg("Grep returned a different error code");

    if(WEXITSTATUS(ret) != exp_ret) fail_msg("%s", fail_message);

}

void testDownloadAudioNoMetaDataUsesOnPageStats(void** state){
    sqlite3* database = *state;
    const char exp_path [] = AUDIO_ROOT PATH_1;
    MetaData_t data = {0};

    addEntry(database);

    downloadAudio("https://www.youtube.com/watch?v=iic-mhXFZic", PATH_1_ID, &data, NO_ART, NULL);

    assertDownloaded(exp_path);
    assertMetaData(exp_path, "Music",  FOUND);
    assertMetaData(exp_path, "Tom Cardy, Brian David Gilbert", FOUND);
    assertMetaData(exp_path, "Beautiful Mind", FOUND);
    assertCover(exp_path, NOT_FOUND);
}

void testDownloadAudioAllMetaData(void** state){
    sqlite3* database = *state;
    const char exp_path [] = AUDIO_ROOT PATH_2;
    MetaData_t data = {.genre="Mine Craft", .artist="C418", .album="Ocean"};

    addEntry(database);

    downloadAudio("https://www.youtube.com/watch?v=HvdP87eDasE", PATH_2_ID, &data, NO_ART, NULL);

    assertDownloaded(exp_path);
    assertMetaData(exp_path, data.genre,  FOUND);
    assertMetaData(exp_path, data.artist, FOUND);
    assertMetaData(exp_path, data.album, FOUND);
    assertCover(exp_path, NOT_FOUND);
}

void testDownloadAudioEmbedsCoverArt(void** state){
    sqlite3* database = *state;
    const char exp_path [] = AUDIO_ROOT PATH_3;
    MetaData_t data = {0};

    addEntry(database);

    downloadAudio("https://www.youtube.com/watch?v=iic-mhXFZic", PATH_3_ID, &data, THUMB_ART, NULL);

    assertDownloaded(exp_path);
    assertCover(exp_path, FOUND);
}

void testDownloadAudioEmbedsNoCoverArt(void** state){
    sqlite3* database = *state;
    const char exp_path [] = AUDIO_ROOT PATH_4;
    MetaData_t data = {0};

    addEntry(database);

    downloadAudio("https://www.youtube.com/watch?v=wMIWqUPlEtE", PATH_4_ID, &data, NO_ART, NULL);

    assertDownloaded(exp_path);
    assertCover(exp_path, NOT_FOUND);
}

void testDownloadAudioWeirdMetaData(void** state){
    sqlite3* database = *state;
    const char exp_path [] = AUDIO_ROOT PATH_5;
    MetaData_t data = {
        .genre="%titles.%exts",
        .artist="?P<meta_synopsis>",
        .album=" ?!@#$%^&*_-~+=.<>|"
    };

    addEntry(database);

    downloadAudio("https://www.youtube.com/watch?v=SaoT_ULWJZk", PATH_5_ID, &data, NO_ART, NULL);

    assertDownloaded(exp_path);
    assertMetaData(exp_path, data.genre,  FOUND);
    assertMetaData(exp_path, data.artist, FOUND);
    assertMetaData(exp_path, data.album, FOUND);
    assertCover(exp_path, NOT_FOUND);
}

void testDownloadAudioEmbedsGivenCoverArt(void** state){
    sqlite3* database = *state;
    const char exp_path [] = AUDIO_ROOT PATH_6;
    const char cover_path [] = "./purple-frog.jpg";
    MetaData_t data = {0};

    addEntry(database);

    downloadAudio("https://www.youtube.com/watch?v=k85mRPqvMbE", PATH_6_ID, &data, GIVEN_ART, cover_path);

    assertDownloaded(exp_path);
    assertCover(exp_path, FOUND);
}

//yt-dlp is a bit weird when it comes to this specific task
//using --emebed-thumbnail will grab the page's thumbnail.
//Using the --postprocesses-args flags to add the custom art does work, but it
//removed metadata due to how the processes is ordered.
//Basically, the meta-data is already written, but when adding the cover art it
//removes it.
//Additonally, with --embed-thumbnail and the postprocesses-args flags 
//it tries to add both the custom cover art and the video's cover art in the same ffmpeg command internally.
void testDownloadAudioEmbedsGivenCoverArtAndAllMetaData(void** state){
    sqlite3* database = *state;
    const char exp_path [] = AUDIO_ROOT PATH_7;
    const char cover_path [] = "./purple-frog.jpg";
    MetaData_t data = {.genre="Frog", .artist="Crazy", .album="Wow"};

    addEntry(database);

    downloadAudio("https://www.youtube.com/watch?v=k85mRPqvMbE", PATH_7_ID, &data, GIVEN_ART, cover_path);

    assertDownloaded(exp_path);
    assertMetaData(exp_path, data.genre,  FOUND);
    assertMetaData(exp_path, data.artist, FOUND);
    assertMetaData(exp_path, data.album, FOUND);
    assertCover(exp_path, FOUND);
}

void testDownloadAudioPartialMetaDataFillsRestFromPage(void** state){
    sqlite3* database = *state;
    const char exp_path [] = AUDIO_ROOT PATH_8;
    MetaData_t data = {.genre="", .artist="", .album="Single"};

    addEntry(database);

    downloadAudio("https://www.youtube.com/watch?v=HvdP87eDasE", PATH_8_ID, &data, NO_ART, NULL);

    assertDownloaded(exp_path);
    assertMetaData(exp_path, "Entertainment",  FOUND); //genre
    assertMetaData(exp_path, "Toothless", FOUND); //artist
    assertMetaData(exp_path, data.album, FOUND);
    assertCover(exp_path, NOT_FOUND);
}
