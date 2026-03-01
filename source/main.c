#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "../includes/fileOps.h"
#include "../includes/databaseOps.h"
#include "../includes/userInput.h"
#include "../includes/downloading.h"

enum DOWNLOAD_COVERS {NO = 0, YES};

static void editMenu(enum CONFIG config);
static void executeNoList(const MetaData_t* meta_info, enum DOWNLOAD_COVERS wants_cover, enum COVERS cover_mode, const char* cover_path);
static void executeWithList(FILE* list, const MetaData_t* overall_meta_info, enum DOWNLOAD_COVERS wants_cover, enum COVERS cover_mode, const char* cover_path);

static void editMenu(enum CONFIG config){
    enum EDITING {NOT = 0, IS};
    enum EDITING editing = IS;
    char buffer [5] = "";
    while(editing){
        enum ERROR err = NO_ERROR;
        err = listConfigRoots(config);

        if(err){
            PRINT_ERROR("Could not list paths");
            break;
        }

        printf("What action do you want to take?\n%c) Add\n%c) Update\n%c) Delete\n%c) Exit\nEnter the number: ", ADD_OPT, UPT_OPT, DEL_OPT, EXT_OPT);
        if(boundedInput(stdin, buffer, 5, NULL) != 1){
            ADVISE_USER("Enter a single digit");
            continue;
        }

        switch(buffer[0]){
            case ADD_OPT: addMenu(config); break;
            case UPT_OPT: updateMenu(config); break;
            case DEL_OPT: deleteMenu(config); break;
            case EXT_OPT: editing = NOT; break;
            default: ADVISE_USER("Enter an available number"); break;
        }
    }
}

static void executeNoList(const MetaData_t* meta_info, enum DOWNLOAD_COVERS wants_cover, enum COVERS cover_mode, const char* cover_path){
    do{
        int video_path_id = getUserChoiceForDir(VIDEO_CONFIG);
        int audio_path_id = getUserChoiceForDir(AUDIO_CONFIG);
        int cover_path_id = SKIPPING;
        if(wants_cover) cover_path_id = getUserChoiceForDir(COVER_CONFIG);

        if(video_path_id != SKIPPING || audio_path_id != SKIPPING || cover_path_id != SKIPPING){
            char yt_url [YT_URL_INPUT_SIZE] = "";
            enum INPUT user_input = INVALID;
            while(user_input == INVALID){
                user_input = getURLFromUser(yt_url);
                if(yt_url[0] == '\0'){
                    ADVISE_USER("Okay No URL then");
                    video_path_id = SKIPPING;
                    audio_path_id = SKIPPING;
                    cover_path_id = SKIPPING;
                    user_input = VALID;
                }
            }
            if(video_path_id != SKIPPING) (void)downloadVideo(yt_url, video_path_id, meta_info);
            if(audio_path_id != SKIPPING) (void)downloadAudio(yt_url, audio_path_id, meta_info, cover_mode, cover_path);
            if(cover_path_id != SKIPPING) (void)downloadCover(yt_url, cover_path_id);
        }
    }while(askToRepeat());
}

static void executeWithList(FILE* list, const MetaData_t* overall_meta_info, enum DOWNLOAD_COVERS wants_cover, enum COVERS cover_mode, const char* cover_path){
    FILE* error_log = fopen("/tmp/con-downloader-errors.txt", "w");
    if(error_log == NULL){
        PRINT_ERROR("Could not create file for logging errors");
        exit(EXIT_FAILURE);
    }

    int video_path_id = getUserChoiceForDir(VIDEO_CONFIG);
    int audio_path_id = getUserChoiceForDir(AUDIO_CONFIG);
    int cover_path_id = SKIPPING;
    if(wants_cover) cover_path_id = getUserChoiceForDir(COVER_CONFIG);

    if(video_path_id == SKIPPING && audio_path_id == SKIPPING && cover_path_id == SKIPPING){
        ADVISE_USER("Fine download nothing then >:(");
        return;
    }

    enum ERROR were_errors = NO_ERROR;
    enum FILE_INPUT line_details = GOOD_LINE;
    while(line_details != DONE){
        MetaData_t per_line_meta = {NULL};
        char url_buffer [YT_URL_INPUT_SIZE] = "";

        line_details = readFileLine(list, url_buffer, &per_line_meta);
        if(line_details == GOOD_LINE){
            MetaData_t added_meta_data = {
                .genre=overall_meta_info->genre,
                .artist=overall_meta_info->artist,
                .album=overall_meta_info->album
            };

            if(per_line_meta.genre != NULL) added_meta_data.genre = per_line_meta.genre;
            if(per_line_meta.artist != NULL) added_meta_data.artist = per_line_meta.artist;
            if(per_line_meta.album != NULL) added_meta_data.album = per_line_meta.album;

            if(video_path_id != SKIPPING){
                enum ERROR video_error = downloadVideo(url_buffer, video_path_id, &added_meta_data);
                if(video_error == HAD_ERROR){
                    were_errors = HAD_ERROR;
                    fprintf(error_log, "Getting the video for the url %s wasn't error free\n", url_buffer);
                }
            }

            if(audio_path_id != SKIPPING){
                enum ERROR audio_error = downloadAudio(url_buffer, audio_path_id, &added_meta_data, cover_mode, cover_path);
                if(audio_error == HAD_ERROR){
                    were_errors = HAD_ERROR;
                    fprintf(error_log, "Getting the audio for the url %s wasn't error free\n", url_buffer);
                }
            }

            if(cover_path_id != SKIPPING){
                enum ERROR cover_error = downloadCover(url_buffer, cover_path_id);
                if(cover_error == HAD_ERROR){
                    were_errors = HAD_ERROR;
                    fprintf(error_log, "Getting the cover for the url %s wasn't error free\n", url_buffer);
                }
            }

            free((char*)per_line_meta.genre);
            free((char*)per_line_meta.artist);
            free((char*)per_line_meta.album);
        }
    }
    fclose(error_log);
    ADVISE_USER("Finished parsing the file");
    if(were_errors == HAD_ERROR){
        ADVISE_USER("There were some errors while downloading. Check the file tmp/con-downloader-errors.txt");
    }
}

int main(int argc, char** argv){
    if(initDatabase() == HAD_ERROR){
        exit(EXIT_FAILURE);
    }

    FILE* cover_art = NULL;
    FILE* url_list = NULL;

    int opt = 0;
    int parsing = 1;
    int edit_choice  = -1;
    char* genre = NULL;
    char* artist = NULL;
    char* album = NULL;
    char* cover_path = NULL;
    enum COVERS cover_mode = THUMB_ART;
    enum DOWNLOAD_COVERS download_covers = NO;
    while(parsing){
        int option_index = 0;
        static const char short_options [] = "dhklnrsva:b:c:e:f:g:";
        static const struct option long_options [] = {
            {"artist", required_argument, NULL, 'a'},
            {"album", required_argument, NULL, 'b'},
            {"cover", required_argument, NULL, 'c'},
            {"deep-list", no_argument, NULL, 'd'},
            {"edit", required_argument, NULL, 'e'},
            {"file", required_argument, NULL, 'f'},
            {"genre", required_argument, NULL, 'g'},
            {"help", no_argument, NULL, 'h'},
            {"keep-covers", no_argument, NULL, 'k'},
            {"list", no_argument, NULL, 'l'},
            {"no-covers", no_argument, NULL, 'n'},
            {"refresh", no_argument, NULL, 'r'},
            {"strict-meta", no_argument, NULL, 's'},
            {"version", no_argument, NULL, 'v'},
            {0, 0, 0, 0} //last struct must be zeros
        };

        opt = getopt_long(argc, argv, short_options, long_options, &option_index);

        if(opt == -1){
            parsing = 0;
            break;
        }

        switch(opt){
            case 'a':
                artist = optarg;
                (void)sanitizeMetaString(artist);
            break;

            case 'b':
                album = optarg;
                (void)sanitizeMetaString(album);
            break;

            case 'c':
                if(cover_mode != THUMB_ART){
                    PRINT_ERROR("Can not specify both cover modes as they conflict");
                    exit(EXIT_FAILURE);
                }
                cover_art = openFile(optarg, "r");
                if(cover_art == NULL){
                    PRINT_ERROR("Couldn't open provided cover file\n");
                    exit(EXIT_FAILURE);
                }
                //can be a TOCTOU, but this is handed off to another program
                //so not like I can use the fd anyway.
                fclose(cover_art);
                cover_path = optarg;
                cover_mode = GIVEN_ART;
            break;

            case 'd':
                if(listAllRootWithPaths() == HAD_ERROR){
                    PRINT_ERROR("Sorry couldn't list all paths :(");
                    exit(EXIT_FAILURE);
                }
                exit(EXIT_SUCCESS);
            break;

            case 'e':
                edit_choice = getConfigToEdit(optarg);
                if(edit_choice != NOT_A_CONFIG){
                    editMenu(edit_choice);
                }else{
                    PRINT_ERROR("Configs to edit are audio (a), black (b), video (v), and cover (c)");
                    exit(EXIT_FAILURE);
                }
                exit(EXIT_SUCCESS);
            break;

            case 'f':
                url_list = openFile(optarg, "r");
                if(url_list == NULL){
                    PRINT_ERROR("Couldn't open provided URL file\n");
                    exit(EXIT_FAILURE);
                }
            break;

            case 'g':
                genre = optarg;
                (void)sanitizeMetaString(genre);
            break;

            case 'h':
                printf("Usage: %s [OPTIONS]\n\nOPTIONS:\n"
                    "-a, --artist GENRE\t\t\tSpecify the artist to add as meta data for all downloaded content\n"
                    "-b, --album ALBUM\t\t\tSpecify the album to add as meta data for all downloaded content\n"
                    "-c, --cover COVER\t\tSpecify an image file to add as a cover art to all downloaded content\n"
                    "-d, --deep-list\t\t\tPrint all root paths and their children paths\n"
                    "-e, --edit [audio (a),video (v),cover (c)]\t\t\tEnter the short or long form to edit your root paths for the specified config.\n"
                    "-f, --file FILE\t\t\tSpecify a file with youtube URLs separated by newlines. Modifiers can be added at the end of each line in the format of |GENRE:ARTIST:ALBUM\n"
                    "-g, --genre GENRE\t\t\tSpecify the genre to add as meta data to all downloaded content\n"
                    "-h, --help\t\t\tPrint this help menu :D\n"
                    "-k, --keep-covers\t\t\t Enable the prompt asking where to send covers\n"
                    "-l, --list\t\t\tPrint just root paths and their depth\n"
                    "-n, --no-cover\t\t\t Do not add the cover art on the youtube page for all downloaded content\n"
                    "-r, --refresh\t\t\tTell the database to scan through its root paths to remove or add children paths\n"
                    "-v, --version\t\t\tPrint the current version\n"
                    ,argv[0]
                );
                exit(EXIT_SUCCESS);
            break;

            case 'k':
                download_covers = YES;
            break;

            case 'l':
                if(listAllRoots() == HAD_ERROR){
                    PRINT_ERROR("Sorry couldn't list root paths :(");
                    exit(EXIT_FAILURE);
                }
                exit(EXIT_SUCCESS);
            break;

            case 'n':
                if(cover_mode != THUMB_ART){
                    PRINT_ERROR("Can not specify both cover modes as they conflict");
                    exit(EXIT_FAILURE);
                }

                cover_mode = NO_ART;
            break;

            case 'r':
                if(refreshDatabase() == HAD_ERROR){
                    ADVISE_USER("Failed to refresh the database.\nNo changes have been made.");
                }else{
                    ADVISE_USER("Database refreshed!");
                }
                exit(EXIT_SUCCESS);
            break;

            case 's':
                puts("You found the secret feature!\nYour reward is nothing, but you read the source code at least :D.");
            break;

            case 'v':
                puts(VERSION);
                exit(EXIT_SUCCESS);
            break;

            default:
                exit(EXIT_FAILURE);
            break;
        }
    } //end of arg parsing

    if(genre != NULL) ADVISE_USER_FORMAT("GENRE metadata will be %s\n", genre);
    if(artist != NULL) ADVISE_USER_FORMAT("ARTIST metadata will be %s\n", artist);
    if(album != NULL) ADVISE_USER_FORMAT("ALBUM metadata will be %s\n", album);


    if(download_covers == YES){
        if(getNumOfPathRowsForConfig(COVER_CONFIG) == 0){
            ADVISE_USER("You need to add some cover destinations with --edit");
            exit(EXIT_FAILURE);
        }
    }else{
        if(getNumOfPathRowsForConfig(VIDEO_CONFIG) == 0 &&
            getNumOfPathRowsForConfig(AUDIO_CONFIG) == 0){
            ADVISE_USER("You need to add audio and video destinations with --edit");
            exit(EXIT_FAILURE);
        }
    }

    MetaData_t meta_data = {.genre = genre, .artist=artist, .album=album};
    if(url_list == NULL){
        executeNoList(&meta_data, download_covers, cover_mode, cover_path);
    }else{
        executeWithList(url_list, &meta_data, download_covers, cover_mode, cover_path);
        (void)fclose(url_list);
    }

    return EXIT_SUCCESS;
}
