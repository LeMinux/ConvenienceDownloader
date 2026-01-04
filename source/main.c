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
                if(listAllRoots() == HAD_ERROR){
                    PRINT_ERROR("Sorry couldn't list root paths :(");
                }
            break;

            case 'd':
                if(listAllRootWithPaths() == HAD_ERROR){
                    PRINT_ERROR("Sorry couldn't list all paths :(");
                }
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

            case 'h':
                printf("Usage: %s [OPTIONS]\n\nOptions\n"
                    "-h, --help\t\t\tPrint this help menu :D\n"
                    "-v, --version\t\t\tPrint the current version\n"
                    "-l, --list\t\t\tPrint all your root paths from your config\n"
                    "-d, --deep-list\t\t\tPrint all potential destinations based on your config\n"
                    "-f, --file [FILE]\t\t\tSpecify a file with youtube URLs separated by newlines\n"
                    "-r, --refresh\t\t\tTell the database to scan through its root paths to remove or add entries."
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

    if(url_list == NULL){
        //execute_no_list();
    }else{
        //execute_with_list();
    }

    return 0;
}
