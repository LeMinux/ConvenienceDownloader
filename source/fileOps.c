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

enum INPUT checkDirPath(const char* dir_path){
    assert(dir_path != NULL);

    struct stat file_stat = {0};
    if(lstat(dir_path, &file_stat) < 0){
        int error = errno;
        switch(error){
            case ENOENT: ADVISE_USER("Path given doesn't exist"); break;
            case ENOTDIR: ADVISE_USER("Path given has broken components"); break;
            case EACCES: ADVISE_USER("Can't access path given"); break;
            case ENAMETOOLONG: ADVISE_USER_FORMAT("Path is too long. The limit is %d", PATH_MAX); break;
            default: ADVISE_USER("System limits prevented opening the directory"); break;
        }
        return INVALID;
    }

    if(S_ISLNK(file_stat.st_mode)){
        ADVISE_USER("The end point can't be a link");
        return INVALID;
    }

    if(!(S_ISDIR(file_stat.st_mode))){
        ADVISE_USER("Path given is not a directory");
        return INVALID;
    }

    //yes access can have TOCTOU issues, but this program really doesn't directly
    //handle paths and instead hands it off
    if(access(dir_path, R_OK | W_OK | X_OK)){
        ADVISE_USER("You don't have read, write, and execute permissions for the path");
        return INVALID;
    }

    return VALID;
}

FILE* openFile(const char* file_name, const char* mode){
    assert(file_name != NULL && file_name[0] != '\0');
    assert(mode != NULL && mode[0] != '\0');

    FILE* open_file = fopen(file_name, mode);
    if(open_file == NULL){
        PRINT_FORMAT_ERROR("Failed to open file: %s", file_name);
        return NULL;
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
        return NULL;

    success:
        return open_file;
}
