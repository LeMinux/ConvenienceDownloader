#include "../includes/fileOps.h"

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
		fprintf(stderr, PNT_RED "Failed to move file %s to the destination %s\n" PNT_RESET, fileName, destination);
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

int checkIfExists(const char* check){
	if(access(check, F_OK) == -1)
		return HAD_ERROR;
	else
		return NO_ERROR;
}

int validateDirPath(const char* path){
	//path doesn't exist
	struct stat checkStat;
	if(stat(path, &checkStat) == -1){
		fprintf(stderr, RED "Path \"%s\" does not exist\n" RESET, path);
		return HAD_ERROR;
	}

	if(S_ISDIR(checkStat.st_mode) == NOT_A_DIR) {
		fprintf(stderr, RED "Path \"%s\" is not a directory\n" RESET, path);
		return HAD_ERROR;
	}

	//checks if the effective user id matches the directory
	if(geteuid() != checkStat.st_uid){
		fprintf(stderr, RED "The user does not own the directory \"%s\"\n" RESET, path);
		return HAD_ERROR;
	}

	//S_I*USR only checks if user bits are set
	//this does not check if the user owns it hence the check above
	if((checkStat.st_mode & (S_IRUSR | S_IWUSR | S_IXUSR)) != (S_IRUSR | S_IWUSR | S_IXUSR)) {
		fprintf(stderr, RED "Directory \"%s\" needs read, write, and execution privledges" RESET, path);
		return HAD_ERROR;
	}

	return NO_ERROR;
}
