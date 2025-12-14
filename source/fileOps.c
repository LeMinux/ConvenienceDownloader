#include "../includes/fileOps.h"
#include "../includes/userInput.h"

/*
int moveFile(const char* fileName, const char* destination){
	const char MOVE [] = "mv ";
	int length = strlen(fileName) + strlen(destination) + strlen(MOVE);
	//+1 for space
	char* moveCommand = malloc(length + 2);
	snprintf(moveCommand, length + 2, "%s%s %s", MOVE, fileName, destination);
	printf("%s\n", moveCommand);
	system(moveCommand);
	free(moveCommand);

	//file is still in the current directory if NO_ERROR occurs
	if(checkIfExists(fileName) == NO_ERROR){
		PRINT_FORMAT_ERROR(PNT_RED "Failed to move file %s to the destination %s\n" PNT_RESET, fileName, destination);
		return HAD_ERROR;
	}else{
		return NO_ERROR;
	}
}

//note this will only search the current directory
//this should be fine though since downloads are set in the currentDirectory
void getFileNameByID(const char* id, const char* extension, char* outString, int length){
	DIR* currentDirectory = NULL;
	struct dirent *entry;

	currentDirectory = opendir(".");
	if (currentDirectory == NULL)printAndExit(EXIT_FAILURE, "Failed to open currentDirectory)");

	int isFound = 0;
	while((entry = readdir(currentDirectory)) != NULL && !isFound){
		struct stat statEntry;
		if(stat(entry->d_name, &statEntry) == 0){
			if(S_ISREG(statEntry.st_mode)){
				if(strstr(entry->d_name, extension) && strstr(entry->d_name, id)){
					memcpy(outString, entry->d_name, length);
					isFound = 1;
				}
			}
		}
	}

	// Close the directory
	closedir(currentDirectory);
}

//converts a file into an mp3 via ffmpeg
//this returns the newly created mp3 file
void convertToMp3(const char* songName){
	//create mp3 file name
	const char FFMPEG [] = "ffmpeg -i ";
	const char MP3_EXTENSTION [] = ".mp3";
	const char BITRATE [] = " -b:a 256K ";
	int extension = strrchr(songName, '.') - songName;
	int length = extension + strlen(MP3_EXTENSTION);
	char* fileMP3 = malloc(length + 1);
	if(fileMP3 == NULL) printAndExit(EXIT_FAILURE, FAILED_MALLOC_MSG);

	//uses format expression to limit size
	snprintf(fileMP3, length + 1, "%.*s.mp3", extension, songName);

	//converting
	length = strlen(songName) + strlen(fileMP3) + strlen(FFMPEG) + strlen(BITRATE);
	char* convertCommand = malloc(length + 3);
	if(convertCommand == NULL) printAndExit(EXIT_FAILURE, FAILED_MALLOC_MSG);

	//+2 for the space in the command +1 since snprintf includes nul in buffer
	snprintf(convertCommand, length + 1, "%s%s%s%s", FFMPEG, songName, BITRATE, fileMP3);
	printf(PNT_GREEN "%s\n" PNT_RESET, convertCommand);

	if(system(convertCommand) > 0) printAndExit(EXIT_FAILURE, CONVERT_FAIL_MSG);

	free(fileMP3);
	free(convertCommand);
}
*/

/*
int checkIfExists(const char* check){
	if(access(check, F_OK) == -1)
		return HAD_ERROR;
	else
		return NO_ERROR;
}
*/

/*
int validateDirPath(const char* path){
	//path doesn't exist
	struct stat checkStat;
	if(stat(path, &checkStat) == -1){
		PRINT_FORMAT_ERROR(RED "Path \"%s\" does not exist\n" RESET, path);
		return HAD_ERROR;
	}

	if(S_ISDIR(checkStat.st_mode) == NOT_A_DIR) {
		PRINT_FORMAT_ERROR(RED "Path \"%s\" is not a directory\n" RESET, path);
		return HAD_ERROR;
	}

	//checks if the effective user id matches the directory
	if(geteuid() != checkStat.st_uid){
		PRINT_FORMAT_ERROR(RED "The user does not own the directory \"%s\"\n" RESET, path);
		return HAD_ERROR;
	}

	//S_I*USR only checks if user bits are set
	//this does not check if the user owns it hence the check above
	if((checkStat.st_mode & (S_IRUSR | S_IWUSR | S_IXUSR)) != (S_IRUSR | S_IWUSR | S_IXUSR)) {
		PRINT_FORMAT_ERROR(RED "Directory \"%s\" needs read, write, and execution privledges" RESET, path);
		return HAD_ERROR;
	}

	return NO_ERROR;
}
*/

/*
dirent openDir(const char* dir_name){
    //Don't care about symlinks, so stat is used to resolve symlinks even if chained.
    //Plus the user may want a symlink to a directory in the file.
    struct stat dir_stat = {0};
    if(stat(dir_name, &dir_stat) != 0){
        PRINT_FORMAT_ERROR("Couldn't get dir information\n");
        return NULL;
    }

    //Directories and other kinds of files though are checked since those aren't regular files.
    FILE* open_file = NULL;
    if(S_ISDIR(dir_stat.st_mode)){
        open_file = fopen(file_name, "r");
        if(open_file == NULL){
            PRINT_FORMAT_ERROR("Failed to open file: %s", file_name);
        }
    }else{
        PRINT_FORMAT_ERROR("File name given is not a regular file: %s", file_name);
    }

    return open_file;
}
*/

/*
//This does create a race condition, but it's a simple check for
//when the user wants to save directories
//better checks are done when it will be used
int checkIsDir(const char* dir_name){
    assert(dir_name != NULL || dir_name[0] != '\0');

    //once again dont' care if it's a link
    struct stat dir_stat = {0};
    if(stat(dir_name, &dir_stat) != 0){
        PRINT_FORMAT_ERROR("Couldn't get directory information\n");
        return -1;
    }

    if(!S_ISDIR(dir_stat.st_mode)){
        PRINT_FORMAT_ERROR("Directory path given is not a directory: %s", dir_name);
        return -1;
    }

    return 0;
}
*/

void openDir(const char* dir_path, DIR** stream_result){
    assert(dir_path != NULL);
    assert(stream_result != NULL);

    DIR* dir_stream = opendir(dir_path);
    if(dir_stream == NULL){
        PRINT_FORMAT_ERROR("Failed to open directory: %s\n", dir_path);
        *stream_result = NULL;
    }else{
        *stream_result = dir_stream;
    }
}

FILE* openFile(const char* file_name, const char* mode){
    assert(file_name != NULL || file_name[0] != '\0');
    assert(mode != NULL || mode[0] != '\0');

    FILE* open_file = fopen(file_name, mode);
    if(open_file == NULL){
        PRINT_FORMAT_ERROR("Failed to open file: %s", file_name);
        goto error_no_open;
    }

    int fd = fileno(open_file);
    if(fd < 0){
        PRINT_FORMAT_ERROR("Failed to get descriptor of file: %s", file_name);
        goto error_did_open;
    }


    struct stat file_stat = {0};
    if(fstat(fd, &file_stat) != 0){
        PRINT_FORMAT_ERROR("Couldn't get file information on %s\n", file_name);
        goto error_did_open;
    }

    //Directories can be opened with fopen without returning NULL
    if(S_ISREG(file_stat.st_mode)){
        goto success;
    }else{
        PRINT_FORMAT_ERROR("File name given is not a regular file: %s", file_name);
        goto error_did_open;
    }

    error_did_open:
        fclose(open_file);

    error_no_open:
        return NULL;

    success:
        return open_file;
}

/*
int setConfigDest(int config, const RootInfoArray* dir_infos){
    assert(config == AUDIO_CONFIG || config == VIDEO_CONFIG || config == COVER_CONFIG);
    assert(dir_infos != NULL);
    assert(dir_infos->dir_entries != NULL);
    assert(dir_infos->length != 0);

    char* config_path = "";
    switch(config){
        case AUDIO_CONFIG: config_path = AUDIO_CONFIG_PATH; break;
        case VIDEO_CONFIG: config_path = VIDEO_CONFIG_PATH; break;
        case COVER_CONFIG: config_path = COVER_CONFIG_PATH; break;
    }

    sqlite3* database = NULL;
    if(sqlite3_open(CONFIG_DATABASE, &database) != SQLITE_OK){
        PRINT_ERROR("Could not open database for configs.\nNo settings have been changed");
        return -1;
    }

    //ensure that string is nul terminated
    sqlite3_stmt* statement = NULL;
    for(int i = 0; i < dir_infos->length; ++i){
        RootInfo* entry = &dir_infos->dir_entries[i];
        if(sqlite3_prepare_v2(database, "INSERT INTO ? VALUES(?, ?, ?)", -1, &statement, NULL) != SQLITE_OK){

        }

        sqlite3_bind_text();
        sqlite3_bind_int();
        sqlite3_bind_int();
        sqlite3_step();
        sqlite3_finalize();

        if(fprintf(config_file, "%.*s,%d\n", entry->name_length, entry->root_name, entry->depth) < 0){
            PRINT_FORMAT_ERROR("Encountered an error while rewriting %s.\nData has been lost.", config_path);
            return -1;
        }
    }

    fclose(config_file);
    return 0;
}
*/

/*
int setConfigDest(int config, const RootInfoArray* dir_infos){
    assert(config == AUDIO_CONFIG || config == VIDEO_CONFIG || config == COVER_CONFIG);
    assert(dir_infos != NULL);
    assert(dir_infos->dir_entries != NULL);
    assert(dir_infos->length != 0);

    char* config_path = "";
    switch(config){
        case AUDIO_CONFIG: config_path = AUDIO_CONFIG_PATH; break;
        case VIDEO_CONFIG: config_path = VIDEO_CONFIG_PATH; break;
        case COVER_CONFIG: config_path = COVER_CONFIG_PATH; break;
    }

    FILE* config_file = openFile(config_path, "w");
    if(config_file == NULL){
        PRINT_FORMAT_ERROR("Failed to open config file: %s.\nNo settings have been overwritten.", config_path);
        return -1;
    }

    //ensure that string is nul terminated
    for(int i = 0; i < dir_infos->length; ++i){
        RootInfo* entry = &dir_infos->dir_entries[i];
        //probably figure out how to make it so it doesn't lose data
        //maybe make a temp file first then later rename to config file
        if(fprintf(config_file, "%.*s,%d\n", entry->name_length, entry->root_name, entry->depth) < 0){
            PRINT_FORMAT_ERROR("Encountered an error while rewriting %s.\nData has been lost.", config_path);
            return -1;
        }
    }

    fclose(config_file);
    return 0;
}
*/

/*
int readConfig(int config, RootInfoArray* dir_array){
    assert(config == AUDIO_CONFIG || config == VIDEO_CONFIG || config == COVER_CONFIG);
    assert(dir_array != NULL);
    assert(dir_array->dir_entries == NULL);
    assert(dir_array->length == 0);

    char* config_path = "";
    switch(config){
        case AUDIO_CONFIG: config_path = AUDIO_CONFIG_PATH; break;
        case VIDEO_CONFIG: config_path = VIDEO_CONFIG_PATH; break;
        case COVER_CONFIG: config_path = COVER_CONFIG_PATH; break;
    }

    FILE* config_file = openFile(config_path, "r");
    if(config_file == NULL){
        PRINT_FORMAT_ERROR("Failed to open config file: %s.", config_path);
        return -1;
    }

    char buffer [CONFIG_BUFFER] = "";
    //length is determined in appendRootEntry
    (void)exactInput(config_file, buffer, CONFIG_BUFFER);
    if(appendRootEntry(dir_array, buffer) != 0){
        PRINT_FORMAT_ERROR("Entry %s in file %s had an issue", buffer, config_path);
        return HAD_ERROR;
    }

    return 0;
}
*/

/*
void listConfig(int config){

    char* config_path = "";
    switch(config){
        case AUDIO_CONFIG: config_path = AUDIO_CONFIG_PATH; break;
        case VIDEO_CONFIG: config_path = VIDEO_CONFIG_PATH; break;
        case COVER_CONFIG: config_path = COVER_CONFIG_PATH; break;
    }

    FILE* config_file = openFile(config_path, "r");
    if(config_file == NULL){
        PRINT_FORMAT_ERROR("Failed to open config file: %s.", config_path);
    }

    char buffer [CONFIG_BUFFER] = "";
    while(exactInput(config_file, buffer, CONFIG_BUFFER)){
        printf("%s\n", buffer);
        //probably some code to print paths too for depth
    }
}
*/
