#include "../includes/userInput.h"

/*
*	 helper method to flush the stream to the next line.
*	 This does not use fflush because it is meant for output streams not input,
*	 this should be used after user input is taken
*	 To avoid having to press enter twice consider where the new line character
*	 is when parsing the input
*
*	stream: file stream to take input from
*/
static void clearLine(FILE* stream){
	//this way is more portable
	int data = '\0';
	while ((data = getc(stream)) != '\n' && data != EOF) { }

	if(ferror(stream)){
		PRINT_ERROR("Encountered a stream error while clearing the file/stream");
		exit(EXIT_FAILURE);
	}
}

int boundedInput(FILE* stream, char* dest, size_t dest_size){
    assert(stream != NULL);
    assert(dest != NULL);
    assert(dest_size > 0);

    //fgets is normally the better choice for taking user input,
    //However, On input where it's just EOF length can't be determined if previous input wasn't overwritten

    size_t amount_written = 0;
    int data = '\0';
    int found_end = 0;
    while(amount_written < dest_size - 1){
        data = getc(stream);
        if(data == '\n' || data == EOF){
            if(ferror(stream)){
                PRINT_ERROR("Encountered an error reading stream for input");
                return HAD_ERROR;
            }
            found_end = 1;
            break;
        }
        dest[amount_written++] = data;
    }

    if(!found_end) clearLine(stream);

    dest[amount_written] = '\0';
    return amount_written;
}

static enum INPUT validIDPortion(const char* id_segment){
    assert(id_segment != NULL);
    assert(strlen(id_segment) == YT_ID_LEN);

    const char id_white_list [] =
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789"
        "_-";

    enum INPUT is_good;
    if(strspn(id_segment, id_white_list) != YT_ID_LEN){
        is_good = INVALID;
    }else{
        is_good = VALID;
    }
    return is_good;
}

//YT urls that have stuff like the radio or playlist in the URL is fine
//since the only important part is the URL and the ID.
enum INPUT getIDFromURL(char ret_id [YT_ID_SIZE]){
    assert(ret_id != NULL);

    char input [YT_URL_INPUT_SIZE];
    (void)printf("Enter the youtube URL that you want to download -> ");

    enum INPUT is_good_input = INVALID;
    #ifdef WRAPPED_INPUT
    if(__wrap_boundedInput(stdin, input, sizeof(input)) != YT_URL_INPUT_SIZE - 1){
    #else
    if(boundedInput(stdin, input, sizeof(input)) != YT_URL_INPUT_SIZE - 1){
    #endif
        ADVISE_USER_FORMAT("URL is too short! It should look like %sXXXXXXXXXXX", YOUTUBE_URL);
    }else if(strstr(input, YOUTUBE_URL) == NULL){
        ADVISE_USER("This is not a youtubeURL!");
    }else{
        char id [YT_ID_SIZE];
        memcpy(id, input + LEN_BEFORE_ID, YT_ID_LEN);
        id[YT_ID_LEN] = '\0';
        puts(id);
        if(validIDPortion(id) == INVALID){
            ADVISE_USER("URL doesn't point to a valid youtube video");
        }else{
            is_good_input = VALID;
        }

        memcpy(ret_id, id, YT_ID_SIZE);
    }

    return is_good_input;
}

/*
int downloadFromURL(const char* youtubeURL, int mode, int downloadCoverArt){
	//--restrict-filenames makes it so escape characters don't need to be added
	//-f bestvideo to force as .mp4
	//--write-thumbnail to get thumnail
	//--convert-thumbnail since the default is webp
	//--audio-quality 256K for a 256K bitrate and better quality audio and save a little on space
	//--audio-format to specify as mp3
	//-R to specify 4 retries
	const char* youtubeDL = NULL;
	switch(mode){
		case 3:
			if(downloadCoverArt){
				youtubeDL = "yt-dlp"
					" --restrict-filenames"
					" --write-thumbnail "
					" --convert-thumbnails jpg "
					" --extract-audio"
					" --audio-format mp3"
					" --audio-quality 256K"
					" -R 4 ";
			}else{
				youtubeDL = "yt-dlp"
					" --restrict-filenames"
					" --extract-audio"
					" --audio-format mp3"
					" --audio-quality 256K"
					" -R 4 ";
			}
		break;
		case 4:
			if(downloadCoverArt){
				youtubeDL = "yt-dlp"
					" -f bestvideo[ext=mp4]+bestaudio[ext=m4a]/best[ext=mp4]/best"
					" --restrict-filenames"
					" --write-thumbnail"
					" --convert-thumbnails jpg"
					" -R 4 ";
			}else{
				youtubeDL = "yt-dlp"
					" -f bestvideo[ext=mp4]+bestaudio[ext=m4a]/best[ext=mp4]/best"
					" --restrict-filenames"
					" -R 4 ";
			}
		break;
		default: PRINT_ERROR("Client passed an unknown mode for downloading URL"); return HAD_ERROR; break;
	}

	//nul byte count is included in YT_URL_INPUT_SIZE
	int length = strlen(youtubeDL) + YT_URL_INPUT_SIZE;
	char* downloadCommand = malloc(length);
	if(downloadCommand == NULL){
		PRINT_ERROR(FAILED_MALLOC_MSG);
		exit(EXIT_FAILURE);
	}
	(void)snprintf(downloadCommand, length + 1, "%s%s", youtubeDL, youtubeURL);
    puts(downloadCommand);
	//(void)printf(PNT_GREEN "%s\n" PNT_RESET, downloadCommand);
	int retVal = NO_ERROR;
	if(system(downloadCommand) > 0){
		PRINT_ERROR(DOWNLOAD_FAIL_MSG);
		retVal = HAD_ERROR;
	}
	free(downloadCommand);
	downloadCommand = NULL;
	return retVal;
}
*/

enum REPEAT askToRepeat(void){
    (void)printf("Do you want to download more? Y/N: ");
    char input_buf [4] = "";

    #ifdef WRAPPED_INPUT
        int user_input_len = __wrap_boundedInput(stdin, input_buf, sizeof(input_buf));
    #else
        int user_input_len = boundedInput(stdin, input_buf, sizeof(input_buf));
    #endif

    enum REPEAT wants_to = ASK_AGAIN;
    switch(user_input_len){
        case 1:
            switch(input_buf[0]){
                case 'y': case 'Y': wants_to = REPEAT; break;
                case 'n': case 'N': wants_to = NO_REPEAT; break;
                default: break;
            }
        break;
        case 2:
            if(strcasecmp(input_buf, "no") == 0){
                wants_to = NO_REPEAT;
            }
        break;
        case 3:
            if(strcasecmp(input_buf, "yes") == 0){
                wants_to = REPEAT;
            }
        break;
        default: break;
    }

    return wants_to;
}

enum CONFIG getConfigToEdit(const char* input){
    assert(input != NULL);

    size_t input_length = strlen(input);
    if(input_length == 1){
        switch(*input){
            case 'A': case 'a': return AUDIO_CONFIG; break;
            case 'V': case 'v': return VIDEO_CONFIG; break;
            case 'C': case 'c': return COVER_CONFIG; break;
            case 'B': case 'b': return BLACK_CONFIG; break;
            default: return -1; break;
        }
    }else if(input_length == OPTION_LEN){
        if(strcasecmp(input, AUDIO_STRING) == 0){
            return AUDIO_CONFIG;
        }else if(strcasecmp(input, VIDEO_STRING) == 0){
            return VIDEO_CONFIG;
        }else if(strcasecmp(input, COVER_STRING) == 0){
            return COVER_CONFIG;
        }else if(strcasecmp(input, BLACK_STRING) == 0){
            return BLACK_CONFIG;
        }else{
            return -1;
        }
    }else{
        return -1;
    }
}

/*
//gets from the user what directory they want to download into
//with the help of getDirectories
char* getUserChoiceForDir(const char* baseDir, const char* prompt){
	//asks user for desired directory
	int found = -1;
	char* returnDir = NULL;
	Node_t* listOfDirs = NULL;
	getSubdirectories(baseDir, &listOfDirs);

	char* input = malloc(101);
	if(input == NULL){
		PRINT_ERROR(FAILED_MALLOC_MSG);
		exit(EXIT_FAILURE);
	}

	while(found == -1){
		do{
			printList(listOfDirs);
			(void)printf("%s", prompt);
		}while(boundedInput(stdin, input, 101) == 0);

		if(strcmp(input, "exit") == 0 || strcmp(input, "Exit") == 0){
			exit(0);
		}else if(strcmp(input, "skip") == 0 || strcmp(input, "Skip") == 0){
			returnDir = malloc(5);
			(void)snprintf(returnDir, 5, "%s","SKIP");
			found = 1;
		}else if((found = containsElement(listOfDirs, input)) != -1){
			returnDir = getElement(listOfDirs, found);
			found = 1;
		}else{
			(void)printf(PNT_RED"\nCould not find the directory %s. Remember case matters.\n"PNT_RESET, input);
			memset(input, '\0', 101);
		}
	}

	free(input);
	deleteList(&listOfDirs);
	return returnDir;
}

//gets from the user what directory they want to download into
//overloaded method that doesn't have the skip option to avoid hidden bugs
char* getUserChoiceForDirNoSkip(const char* baseDir, const char* prompt){
	//asks user for desired directory
	int found = -1;
	char* returnDir = NULL;
	Node_t* listOfDirs = NULL;
	getSubdirectories(baseDir, &listOfDirs);

	char* input = malloc(101);
	if(input == NULL){
		PRINT_ERROR(FAILED_MALLOC_MSG);
		exit(EXIT_FAILURE);
	}

	while(found == -1){
		do{
			printList(listOfDirs);
			(void)printf("%s", prompt);
		}while(boundedInput(stdin, input, 101) == 0);

		if(strcmp(input, "exit") == 0 || strcmp(input, "Exit") == 0){
			exit(0);
		}else if((found = containsElement(listOfDirs, input)) != -1){
			returnDir = getElement(listOfDirs, found);
			found = 1;
		}else{
			(void)printf(PNT_RED"\nCould not find the directory %s. Remember case matters.\n"PNT_RESET, input);
			(void)memset(input, '\0', 101);
		}
	}

	free(input);
	deleteList(&listOfDirs);
	return returnDir;
}
*/
