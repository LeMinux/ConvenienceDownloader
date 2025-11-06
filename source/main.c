#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../includes/fileOps.h"
#include "../includes/userInput.h"

static const char VERSION [] = "2025 9.7";

int main(int argc, char** argv){
    int keepArt = 0;
    enum {THUMB_ART, GIVEN_ART, NO_ART} cover_mode = THUMB_ART;
    FILE* cover_art = NULL;
    FILE* url_list = NULL;

    unsigned int new_audio_amount = 0;
    unsigned int new_video_amount = 0;

    //array of open directory streams
    //DIR** audio_dirs = NULL;
    //DIR** video_dirs = NULL;
    DirInfoArray audio_dir_info = {.dir_entries = NULL, .length = 0};
    DirInfoArray video_dir_info = {.dir_entries = NULL, .length = 0};
    DirInfoArray cover_dir_info = {.dir_entries = NULL, .length = 0};


    int opt = 0;
    int parsing = 1;
    while(parsing){
        int option_index = 0;
        static const char short_options [] = "lhvf:c:a:o:";
        static const struct option long_options [] = {
            {"list", no_argument, NULL, 'l'},
            {"file", required_argument, NULL, 'f'},
            {"cover", required_argument, NULL, 'c'},
            {"write-audio", required_argument, NULL, 'a'},
            {"write-video", required_argument, NULL, 'o'},
            {"help", no_argument, NULL, 'h'},
            {"version", no_argument, NULL, 'v'},
            {0, 0, 0, 0} //last struct must be zeros
        };

        opt = getopt_long(argc, argv, short_options, long_options, &option_index);

        if(opt == -1){
            parsing = 0;
            break;
        }

        switch(opt){
            case 'l':
                (void)puts("v List of availiable directories for MP4");
                (void)puts("\nv List of availiable directories for MP3");
                (void)puts("\nv List of availiable directories for Cover Arts");
            break;

            case 'f':
                url_list = openFile(optarg, "r");
                if(url_list == NULL){
                    fprintf(stderr, RED"Couldn't open URL file\n"RESET);
                    exit(EXIT_FAILURE);
                }
            break;

            case 'c':
                if(strcmp(optarg, "NO-ART")){
                    cover_mode = NO_ART;
                }else{
                    cover_art = openFile(optarg, "r");
                    if(cover_art == NULL){
                        fprintf(stderr, RED"Couldn't open cover file\n"RESET);
                        exit(EXIT_FAILURE);
                    }
                    cover_mode = GIVEN_ART;
                }
            break;

            //getopt doesn't really have a way to accept multiple arguments for one option.
            //There is a way by looping through optind, but it risks missing directories
            //that start with '-' since any amount of arguments can be given for this.
            //There may also be issues if -a and -o are combined together since
            //both arguments would be in optind without a way to distinguish.
            case 'a':
                if(appendEntry(&audio_dir_info, optarg) != 0){
                    fprintf(stderr, "Audio config has not been overwritten.\n");
                    exit(EXIT_FAILURE);
                }
            break;

            case 'o':
                if(appendEntry(&video_dir_info, optarg) != 0){
                    fprintf(stderr, "Video config has not been overwritten.\n");
                    exit(EXIT_FAILURE);
                }
            break;

            //NOT IMPELEMTNED
            case 'r':
                if(appendEntry(&cover_dir_info, optarg) != 0){
                    fprintf(stderr, "Cover config has not been overwritten.\n");
                    exit(EXIT_FAILURE);
                }
            break;

            case 'h':
                printf("Usage: %s [OPTIONS]\n\nOptions\n"
                    "-h, --help\t\t\tPrint this help menu :D\n"
                    "-v, --version\t\t\tPrint the current version\n"
                    "-l, --list\t\t\tPrint all potential destinations for audio and video\n"
                    "-f, --file=FILE\t\t\tSpecify a file with youtube URLs separated by newlines\n"
                    "-c, --cover=COVER\t\tSpecify what cover art to use or pass NO-ART to enforce adding no art\n"
                    "-a, --write-audio=PATH,DEPTH\tSpecify what file paths to save audio to and their depths. Only giving the path without a depth will default to infinite depth.\n"
                    "-o, --write-video=PATH,DEPTH\tSpecify what file paths to save video to and their depths. Only giving the path without a depth will default to infinite depth.\n",
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

    if(audio_dir_info.length > 0){
        if(setConfigDest(AUDIO_CONFIG, &audio_dir_info) != 0){
            fprintf(stderr, RED"Failed to set audio destinations\n"RESET);
            exit(EXIT_FAILURE);
        }
    }else{
        if(readConfig(AUDIO_CONFIG, &audio_dir_info) != 0){
            fprintf(stderr, "Error in reading audio config. Make sure paths are valid");
            exit(EXIT_FAILURE);
        }
    }

    if(video_dir_info.length > 0){
        if(setConfigDest(VIDEO_CONFIG, &video_dir_info) != 0){
            fprintf(stderr, RED"Failed to set video destinations\n"RESET);
            exit(EXIT_FAILURE);
        }
    }else{
        if(readConfig(VIDEO_CONFIG, &audio_dir_info) != 0){
            fprintf(stderr, "Error in reading video config. Make sure paths are valid");
            exit(EXIT_FAILURE);
        }
    }

    if(cover_dir_info.length > 0){
        if(setConfigDest(VIDEO_CONFIG, &video_dir_info) != 0){
            fprintf(stderr, RED"Failed to set video destinations\n"RESET);
            exit(EXIT_FAILURE);
        }
    }else{
        if(readConfig(VIDEO_CONFIG, &audio_dir_info) != 0){
            fprintf(stderr, "Error in reading video config. Make sure paths are valid");
            exit(EXIT_FAILURE);
        }
    }

    buildDirTree(/*audio*/);
    buildDirTree(/*vidoe*/);
    buildDirTree(/*cover*/);
    //obtain directory list
    if(url_list == NULL){
        //execute_no_list();
    }else{
        //execute_with_list();
    }

    return 0;
}
