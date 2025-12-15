#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../includes/fileOps.h"
#include "../includes/databaseOps.h"
#include "../includes/userInput.h"

int main(int argc, char** argv){
    if(initDatabase() == HAD_ERROR){
        exit(EXIT_FAILURE);
    }

    int keepArt = 0;
    enum {THUMB_ART, GIVEN_ART, NO_ART} cover_mode = THUMB_ART;
    FILE* cover_art = NULL;
    FILE* url_list = NULL;

    unsigned int new_audio_amount = 0;
    unsigned int new_video_amount = 0;

    //array of open directory streams
    //DIR** audio_dirs = NULL;
    //DIR** video_dirs = NULL;
    //RootInfoArray audio_roots = {.dir_entries = NULL, .length = 0};
    //RootInfoArray video_roots = {.dir_entries = NULL, .length = 0};
    //RootInfoArray cover_roots = {.dir_entries = NULL, .length = 0};


    int opt = 0;
    int parsing = 1;
    int edit_choice  = -1;
    while(parsing){
        int option_index = 0;
        static const char short_options [] = "dhlrvc:e:f:";
        static const struct option long_options [] = {
            {"deep-list", no_argument, NULL, 'd'},
            {"help", no_argument, NULL, 'h'},
            {"list", no_argument, NULL, 'l'},
            //{"ll", no_argument, NULL, 'd'},
            //{"write-audio", required_argument, NULL, 'a'},
            //{"write-video", required_argument, NULL, 'o'},
            {"refresh", no_argument, NULL, 'r'},
            {"version", no_argument, NULL, 'v'},
            {"cover", required_argument, NULL, 'c'},
            {"edit", required_argument, NULL, 'e'},
            {"file", required_argument, NULL, 'f'},
            {0, 0, 0, 0} //last struct must be zeros
        };

        opt = getopt_long(argc, argv, short_options, long_options, &option_index);

        if(opt == -1){
            parsing = 0;
            break;
        }

        switch(opt){
            case 'l':
                listAllRoots();
            break;

            case 'd':
                (void)listAllRootWithPaths();
                /*
                (void)puts("v list of all MP4 destinations");
                (void)listAllRootWithPaths(VIDEO_CONFIG);

                (void)puts("v list of all MP3 destinations");
                (void)listAllRootWithPaths(AUDIO_CONFIG);

                (void)puts("v list of all cover art destinations");
                (void)listAllRootWithPaths(COVER_CONFIG);
                */
            break;

            case 'f':
                url_list = openFile(optarg, "r");
                if(url_list == NULL){
                    PRINT_ERROR("Couldn't open URL file\n");
                    exit(EXIT_FAILURE);
                }
            break;

            case 'c':
                if(strcmp(optarg, "NO-ART")){
                    cover_mode = NO_ART;
                }else{
                    cover_art = openFile(optarg, "r");
                    if(cover_art == NULL){
                        PRINT_ERROR("Couldn't open cover file\n");
                        exit(EXIT_FAILURE);
                    }
                    cover_mode = GIVEN_ART;
                }
            break;

            case 'e':
                edit_choice = getConfigToEdit(optarg);
                if(edit_choice != -1){
                    editMenu(edit_choice);
                }else{
                    PRINT_ERROR("Configs to edit are audio (a), video (v), and cover (c)");
                    exit(EXIT_FAILURE);

                }
                puts("Leaving editing now");
                exit(EXIT_SUCCESS);

            break;

            case 'r':
                refreshDatabase();
            break;

            //getopt doesn't really have a way to accept multiple arguments for one option.
            //There is a way by looping through optind, but it risks missing directories
            //that start with '-' since any amount of arguments can be given for this.
            //There may also be issues if -a and -o are combined together since
            //both arguments would be in optind without a way to distinguish.
            /*
            case 'a':
                if(appendRootEntry(&audio_roots, optarg) != 0){
                    fprintf(stderr, "Audio config has not been overwritten.\n");
                    exit(EXIT_FAILURE);
                }
            break;

            case 'o':
                if(appendRootEntry(&video_roots, optarg) != 0){
                    fprintf(stderr, "Video config has not been overwritten.\n");
                    exit(EXIT_FAILURE);
                }
            break;

            //NOT IMPELEMTNED
            case 'r':
                if(appendRootEntry(&cover_roots, optarg) != 0){
                    fprintf(stderr, "Cover config has not been overwritten.\n");
                    exit(EXIT_FAILURE);
                }
            break;
            */

            case 'h':
                printf("Usage: %s [OPTIONS]\n\nOptions\n"
                    "-h, --help\t\t\tPrint this help menu :D\n"
                    "-v, --version\t\t\tPrint the current version\n"
                    "-l, --list\t\t\tPrint all your root paths from your config\n"
                    "-d, --deep-list\t\t\tPrint all potential destinations based on your config\n"
                    "-f, --file [FILE]\t\t\tSpecify a file with youtube URLs separated by newlines\n"
                    "-r, --refresh\t\t\tTell the configuration database to check stored paths"
                    "-e, --edit \t\t\t[(a)udio, (v)ideo, (c)over, (b)lack] edit your destinations for the specified type. Short hand a, v, and c can be used"
                    "-c, --cover=COVER\t\tSpecify what cover art to use or pass NO-ART to enforce adding no art\n",
                    //"-a, --write-audio=PATH,DEPTH\tSpecify what file paths to save audio to and their depths. Only giving the path without a depth will default to infinite depth.\n"
                    //"-o, --write-video=PATH,DEPTH\tSpecify what file paths to save video to and their depths. Only giving the path without a depth will default to infinite depth.\n",
                    argv[0]
                );
                exit(EXIT_SUCCESS);
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
    
    puts("Finished parsing");

    /*
    if(audio_roots.length > 0){
        if(setConfigDest(AUDIO_CONFIG, &audio_roots) != 0){
            PRINT_ERROR("Failed to set audio destinations. Make sure paths are valid");
            exit(EXIT_FAILURE);
        }
    }else{
        if(readConfig(AUDIO_CONFIG, &audio_roots) != 0){
            PRINT_ERROR("Error in reading audio config.");
            exit(EXIT_FAILURE);
        }
    }

    if(video_roots.length > 0){
        if(setConfigDest(VIDEO_CONFIG, &video_roots) != 0){
            PRINT_ERROR("Failed to set video destinations");
            exit(EXIT_FAILURE);
        }
    }else{
        if(readConfig(VIDEO_CONFIG, &audio_roots) != 0){
            PRINT_ERROR("Error in reading video config.");
            exit(EXIT_FAILURE);
        }
    }

    if(cover_roots.length > 0){
        if(setConfigDest(VIDEO_CONFIG, &video_roots) != 0){
            PRINT_ERROR("Failed to set video destinations. Make sure paths are valid");
            exit(EXIT_FAILURE);
        }
    }else{
        if(readConfig(VIDEO_CONFIG, &audio_roots) != 0){
            PRINT_ERROR("Error in reading video config. Make sure paths are valid");
            exit(EXIT_FAILURE);
        }
    }

    DirOptionArray* audio_options = buildDirOptions(&audio_roots);
    DirOptionArray* video_options = buildDirOptions(&video_roots);
    DirOptionArray* cover_options = buildDirOptions(&cover_roots);
    */

    //free rootInfos?

    if(url_list == NULL){
        //execute_no_list();
    }else{
        //execute_with_list();
    }

    return 0;
}
