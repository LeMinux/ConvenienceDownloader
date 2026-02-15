#include "../includes/downloading.h"

/*
 due to how yt-dlp merges it's bit difficult to get exactly the behavior I would like.
 Ideally I would like to have one command where I craft arguments to download
 video, audio, and covers and send them to separate locations.
 Sadly, -o applies to all output and I potentially have completely different locations to send to.
 The file types as specified in the man page aren't file extension types but rather certain kinds of metadata types.
 Of which only the thumbnail type could be used in a combined command.
 It is possible to download audio and video in one command, but the solutions either download twice
 anyway, uses -k to purposely leave itermediate files, or uses the exec option which I find dangerous.

 I also would like to avoid needing to search the current directory for the file by the yt id and extension
 as that would just slow things down and add extra complexity
 With the search current directory strat it would be much slower in a heavily populated directory.
 Ideally I want this program to be ran anywhere as that's why the database holds absolute paths.
 Luckily yt-dlp does have a way to return the filepath if needed.

 There is also the issue of maintanability of needing to change options for preference in the future.
 Having one giant command would make it difficult to change one specific metadata if it applies to all.

 When it comes to the metadata I am aware that --parse-metadata is an option, but it's behavior is heavily
 tied to how yt-dlp parses OUTPUT TEMPLATES.
 below are some flags and their metadata outputs
 --parse-metadata "foobar==:%(meta_genre)s" adds "foobar=="

 --parse-metadata "foobar:%(meta_genre)s" is NA

 --parse-metadata "foobar\::%(meta_genre)s" adds "foobar:"

 --parse-metadata " foobar:%(meta_genre)s" adds " foobar" with a beginning space

 Using the regex version and passing in ".*" "genre" "<replacement>" didn't work

 *WHY THIS HAPPENS*
 As it turns out, the verbose output shows it automatically places the string literal
 of foobar into %(foobar)s.
 This causes yt-dlp to try and serach for a foobar field which doesn't exist and returns NA.
 The reason the other output works is because the ffmpeg argument is in double quotes forcing the use.
 In order to get the result I want I would have to use
 --parse-metadata "%(sigma&l|sigma)s:%(artist)s"
 This uses the replacement formatting to add sigma if sigma doesn't exist (which it won't)
 or
 --parse-metadata "%(sigma|sigma)s:%(artist)s"
 This uses a default value working on the same principle as the last one

 You may then be surprised as to why I am choosing to go this route anyway as it is a hacky solution.
 The alternative would be to send the file into a temporary place, use ffmpeg to add metadata and send to the location,
 and the remove the temp file.
 This would involve three separate tasks while I could let yt-dlp handle that for me.
 yt-dlp is using the same -metadata flag for ffmpeg anyway I just need to set it correctly.
*/

enum META_TYPE {GENRE = 0, ARTIST, ALBUM};

static const char TEMPLATE [] = "%(title)s.%(ext)s";
static const char META_ARG [] = "--parse-metadata";
static const char ADD_COVER [] = "--embed-thumbnail";

static const char GENRE_FORMAT [] =  "%%(zqjjxvly|%s)s:%%(meta_genre)s";
static const char ARTIST_FORMAT [] = "%%(zqqjxvy|%s)s:%%(meta_artist)s";
static const char ALBUM_FORMAT [] =  "%%(zqjjxvvy|%s)s:%%(meta_album)s";
static const int META_SIZE = sizeof(GENRE_FORMAT) - 2;

static const char PRINT_TO_FILE [] = "--print-to-file";
static const char WHAT_TO_PRINT [] = "after_move:filepath";
static const char FILE_NAME [] = "/tmp/add_cover_to.txt";

static enum ERROR createOutputTemplate(int path_id, char* output_template);
static char* createMetaArg(const char* data, enum META_TYPE type);
static char* createPostArg(const char* cover_path);

static enum ERROR createOutputTemplate(int path_id, char* output_template){
    assert(output_template != NULL);
    assert(path_id > 0);

    int length = pathIDToPath(path_id, output_template);
    enum ERROR status = HAD_ERROR;
    if(length != HAD_ERROR){
        memcpy(output_template + length, TEMPLATE, sizeof(TEMPLATE));
        status = NO_ERROR;
    }
    return status;
}

static char* createMetaArg(const char* data, enum META_TYPE type){
    assert(data != NULL);
    assert(type == GENRE || type == ARTIST || type == ALBUM);

    int size = strlen(data) + META_SIZE;
    char* argument = malloc(size);
    if(argument == NULL) return NULL;
    int len = 0;
    switch(type){
        case GENRE:
            len = snprintf(argument, size, GENRE_FORMAT, data);
        break;
        case ARTIST:
            len = snprintf(argument, size, ARTIST_FORMAT, data);
        break;
        case ALBUM:
            len = snprintf(argument, size, ALBUM_FORMAT, data);
        break;
    }

    if(len < 0 || len >= size){
        free(argument);
        return NULL;
    }

    return argument;
}

//want to add album name, artist name, genre, id, perhaps yt url
enum ERROR downloadVideo(const char* yt_url, int v_id, const MetaData_t* meta){
    assert(yt_url != NULL);
    assert(v_id != SKIPPING || v_id == INVALID);
    assert(meta != NULL);

    char output_template [PATH_MAX + sizeof(TEMPLATE) - 1];
    if(createOutputTemplate(v_id, output_template) == HAD_ERROR){
        PRINT_ERROR("Couldn't create output template");
        return HAD_ERROR;
    }

    //To avoid needing to realloc a bunch this will be a set size filled with NULLS at the end
    //the command line arg needs to end with a NULL anyway, so just like how a nul byte
    //would stop strlen early this will do the same.
    //Do need to figure out how to get rid of all those warnings though
    char* command_arguments [] ={
        "yt-dlp",
        "--restrict-filenames",
        "--retries", "4",
        "-o", output_template,
        "-t", "mp4",
        "--embed-metadata",
        NULL, NULL,     //meta argument
        NULL, NULL,     //meta argument
        NULL, NULL,     //meta argument
        NULL,           //URL
        NULL,           //ending NULL
    };
    //the size of the pointer array not all the elements
    size_t append_index = sizeof(command_arguments)/sizeof(command_arguments[0]) - 7 - 1;

    char* genre_meta = NULL;
    char* artist_meta = NULL;
    char* album_meta = NULL;
    enum ERROR error_status = HAD_ERROR;
    if(meta->genre != NULL){
        genre_meta = createMetaArg(meta->genre, GENRE);
        if(genre_meta == NULL) goto failed;
        command_arguments[append_index++] = META_ARG;
        command_arguments[append_index++] = genre_meta;
    }

    if(meta->artist != NULL){
        artist_meta = createMetaArg(meta->artist, ARTIST);
        if(artist_meta == NULL) goto failed;
        command_arguments[append_index++] = META_ARG;
        command_arguments[append_index++] = artist_meta;
    }

    if(meta->album != NULL){
        album_meta = createMetaArg(meta->album, ALBUM);
        if(album_meta == NULL) goto failed;
        command_arguments[append_index++] = META_ARG;
        command_arguments[append_index++] = album_meta;
    }

    command_arguments[append_index] = yt_url;

    error_status = execProgram("/usr/bin/yt-dlp", command_arguments);

    failed:
    free(genre_meta);
    free(artist_meta);
    free(album_meta);
    return error_status;
}


//want to add album name, artist name, genre, id, perhaps yt url
enum ERROR downloadAudio(const char* yt_url, int a_id, const MetaData_t* meta, enum COVERS wants_cover, const char* cover_path){
    assert(yt_url != NULL);
    assert(a_id != SKIPPING || a_id == INVALID);
    assert(meta != NULL);
    assert(wants_cover == THUMB_ART || wants_cover == GIVEN_ART || wants_cover == NO_ART);

    char output_template [PATH_MAX + sizeof(TEMPLATE) - 1];
    if(createOutputTemplate(a_id, output_template) == HAD_ERROR){
        PRINT_ERROR("Couldn't create output template");
        return HAD_ERROR;
    }

    //To avoid needing to realloc a bunch this will be a set size filled with NULLS at the end
    //the command line arg needs to end with a NULL anyway, so just like how a nul byte
    //would stop strlen early this will do the same.
    //Do need to figure out how to get rid of all those warnings though
    char* command_arguments [] ={
        "yt-dlp",
        "--restrict-filenames",
        "--retries", "4",
        "-o", output_template,
        "--extract-audio",
        "--audio-format", "mp3",
        "--audio-quality", "256K",
        "--embed-metadata",
        "--replace-in-metadata", "title", "_-_", "-",
        NULL,           //--embed-thumbnail
        NULL, NULL, NULL, //potential after print
        NULL, NULL,     //meta argument
        NULL, NULL,     //meta argument
        NULL, NULL,     //meta argument
        NULL,           //URL
        NULL            //ending NULL for arg list
    };

    //the size of the pointers in the array not the elements themeselves
    size_t append_index = sizeof(command_arguments)/sizeof(command_arguments[0]) - 11 - 1;

    switch(wants_cover){
        case THUMB_ART:
            command_arguments[append_index++] = ADD_COVER;
        break;
        case GIVEN_ART:
            command_arguments[append_index++] = PRINT_TO_FILE;
            command_arguments[append_index++] = WHAT_TO_PRINT;
            command_arguments[append_index++] = FILE_NAME;
        break;
        default: break;
    }

    char* genre_meta = NULL;
    char* artist_meta = NULL;
    char* album_meta = NULL;
    enum ERROR error_status = HAD_ERROR;
    if(meta->genre != NULL){
        genre_meta = createMetaArg(meta->genre, GENRE);
        if(genre_meta == NULL) goto failed;
        command_arguments[append_index++] = META_ARG;
        command_arguments[append_index++] = genre_meta;
    }

    if(meta->artist != NULL){
        artist_meta = createMetaArg(meta->artist, ARTIST);
        if(artist_meta == NULL) goto failed;
        command_arguments[append_index++] = META_ARG;
        command_arguments[append_index++] = artist_meta;
    }

    if(meta->album != NULL){
        album_meta = createMetaArg(meta->album, ALBUM);
        if(album_meta == NULL) goto failed;
        command_arguments[append_index++] = META_ARG;
        command_arguments[append_index++] = album_meta;
    }

    command_arguments[append_index] = yt_url;
    error_status = execProgram("/usr/bin/yt-dlp", command_arguments);

    if(wants_cover == GIVEN_ART && error_status == NO_ERROR){
        error_status = writeCover(FILE_NAME, cover_path);
    }

    failed:
    free(genre_meta);
    free(artist_meta);
    free(album_meta);
    return error_status;
}

enum ERROR downloadCover(const char* yt_url, int c_id){
    assert(yt_url != NULL);
    assert(c_id != SKIPPING || c_id == INVALID);

    char output_template [PATH_MAX + sizeof(TEMPLATE) - 1];
    if(createOutputTemplate(c_id, output_template) == HAD_ERROR){
        PRINT_ERROR("Couldn't create output template");
        return HAD_ERROR;
    }

    char* command_arguments [] ={
        "yt-dlp",
        "--restrict-filenames",
        "--retries", "4",
        "--skip-download",
        "-o", output_template,
        "--write-thumbnail",
        "--convert-thumbnails", "jpg",
        yt_url,
        NULL
    };

    execProgram("/usr/bin/yt-dlp", command_arguments);
    return NO_ERROR;
}
