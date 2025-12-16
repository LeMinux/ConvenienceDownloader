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

int exactInput(FILE* stream, char* dest, size_t length){
    assert(stream != NULL);
    assert(dest != NULL);

	if(fgets(dest, length, stream) == NULL && ferror(stream)){
		PRINT_ERROR("Encountered a stream error while reading the file/stream");
        return HAD_ERROR;
	}

    //fgets is nul terminating
    if(feof(stream)){
        return strlen(dest);
    }

    //apparently memrchr isn't standard?
    //It would search in reverse where \n would likely be first.
    //Not the biggest deal just really annoying
    char* newLinePos = memchr(dest, '\n', length);
	if(newLinePos == NULL){
		clearLine(stream);
		return length - 1;
	}else{
		*newLinePos = '\0';
		return (int)(newLinePos - dest);
	}
}

int unknownInput(FILE* stream, char** dest){
	char buffer[CHUNK_READ] = "";
	size_t inputLength = 0;
	size_t bufferLength = 0;
	int isEndLine = 0;

	while(!isEndLine){
		if(fgets(buffer, CHUNK_READ, stream) == NULL){
			if(ferror(stream)){
				PRINT_ERROR("File stream error has occured while reading unknown input");
				exit(EXIT_FAILURE);
			}
			isEndLine = 1;
		}else{
			bufferLength = strlen(buffer);
			if(bufferLength == 0){
				isEndLine = 1;
			}else{
				char* temp = strrchr(buffer, '\n');
				if(temp != NULL){
					*temp = '\0';
					bufferLength -= 1;
					isEndLine = 1;
				}

				*dest = realloc(*dest, inputLength + bufferLength + 1);
				if(*dest == NULL){
					PRINT_ERROR(FAILED_MALLOC_MSG);
					exit(EXIT_FAILURE);
				}

				//snprintf would also overwrite all of *dest each time and fgets nul terminates
				(void)strcpy((*dest)+inputLength, buffer);
				inputLength += bufferLength;
			}
		}
	}

	return inputLength;
}

void getURL(char ret [YT_URL_INPUT_SIZE]){
	do{
		(void)printf("Enter the youtube URL that you want to download -> ");

		//fgets is nul terminating so, clearing ret is not necessary
		//for each invalid attempt
		if(exactInput(stdin, ret, YT_URL_INPUT_SIZE) != YT_URL_INPUT_SIZE - 1){
            PRINT_ERROR("URL is too short! It should look like %s[11 chars]\n");
		}else if(strstr(ret, YOUTUBE_URL) == NULL){
            PRINT_ERROR("This is not a youtubeURL!");
		}else{
			return;
		}
	}while(1);
}

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

//helper method for if the user wants to repeat or not
int askToRepeat(void){
	char yesNo = '\0';
	do{
		(void)printf("Do you want to download again? Y/N: ");
		yesNo = fgetc(stdin);
		if(yesNo != '\n') clearLine(stdin);

		switch(yesNo){
			case 'y': case 'Y': return 1; break;
			case 'n': case 'N': return 0; break;
			default: puts("~~Invalid input~~"); break;
		}
	}while(1);

	//incase of some wack error plus compiler was complaining
	return HAD_ERROR;
}

/*
int appendRootEntry(RootInfoArray* entry_array, const char* new_entry){
    assert(entry_array != NULL);
    assert(new_entry != NULL);
    assert(entry_array->length >= 0);

    //REMEMBER NAME IS MALLOCED
    long depth = -1;
    int path_length = 0;
    char* name = NULL;
    char* comma = strrchr(new_entry, ','); //last comma found
    if(comma != NULL){
        path_length = comma - new_entry;

        errno = 0; //strtol man page says to set errno to zero before handling
        depth = strtol(comma + 1, NULL, 10);
        if(errno == ERANGE || depth > INT_MAX){
            fprintf(stderr, "Depth given is too large\n");
            return HAD_ERROR;
        }

        if(depth < 0){
            fprintf(stderr, "Depth can't be negative\n");
            return HAD_ERROR;
        }
    }else{
        path_length = strlen(new_entry);
    }

    if(path_length >= MY_MAX_PATH_SIZE){
        fprintf(stderr, "File path is too long\n");
        return HAD_ERROR;
    }

    if((name = strndup(new_entry, path_length)) == NULL){
        fprintf(stderr, "Not enough memory\n");
        return HAD_ERROR;
    }

    entry_array->dir_entries = realloc(entry_array->dir_entries, sizeof(RootInfo) * (entry_array->length + 1));
    if(entry_array->dir_entries == NULL){
        fprintf(stderr, "Not enough memory\n");
        return HAD_ERROR;
    }

    openDir(name, &(entry_array->dir_entries[entry_array->length].open_dir));

    if(entry_array->dir_entries[entry_array->length].open_dir == NULL){
        fprintf(stderr, "Failed to open directory %s.\n", name);
        return HAD_ERROR;
    }

    entry_array->dir_entries[entry_array->length].root_name = name;
    entry_array->dir_entries[entry_array->length].name_length = path_length;
    entry_array->dir_entries[entry_array->length].depth = depth;
    ++entry_array->length;

    return NO_ERROR;
}
*/

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
		}while(exactInput(stdin, input, 101) == 0);

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
		}while(exactInput(stdin, input, 101) == 0);

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
