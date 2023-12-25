#include "fileOps.h"
#include <string.h>

void writeDest(char* string, int mode){
	FILE* writeTo = NULL;
	switch(mode){
		case 3:
			writeTo = fopen(DES_MP3, "w");
			if(writeTo == NULL) printError(EXIT_FAILURE, "Failed to create file saying where to write audios to");
		break;
		case 4:
			writeTo = fopen(DES_MP4, "w");
			if(writeTo == NULL) printError(EXIT_FAILURE, "Failed to create file saying where to write videos to");
		break;
		default: printError(EXIT_FAILURE, "Client passed unknown mode for Destinations");
		break;
	}

	if(string[strlen(string) - 1] == '/')
		fprintf(writeTo, "%s", string);
	else
		fprintf(writeTo, "%s/", string);

	//fprintf(writeTo, "%s", string);
	fclose(writeTo);
}

void moveFile(const char* fileName, const char* destination){
	const char MOVE [] = "mv ";
	int length = strlen(fileName) + strlen(destination) + strlen(MOVE);
	//+1 for space
	char* moveCommand = malloc(length + 2);
	snprintf(moveCommand, length + 2, "%s%s %s", MOVE, fileName, destination);
	printf("%s\n", moveCommand);
	system(moveCommand);
	free(moveCommand);
}

//note this will only search the current directory
//this should be fine though since downloads are set in the currentDirectory
void getFileNameByID(const char* id, const char* extension, char* outString, int length){
	DIR* currentDirectory = NULL;
	struct dirent *entry;

	currentDirectory = opendir(".");
	if (currentDirectory == NULL)printError(EXIT_FAILURE, "Failed to open currentDirectory)");

	while((entry = readdir(currentDirectory)) != NULL){
		struct stat statEntry;
		// Print the name of each file in the directory
		if(stat(entry->d_name, &statEntry) == 0){
			if(S_ISREG(statEntry.st_mode)){
				if(strstr(entry->d_name, extension) && strstr(entry->d_name, id)){
					memcpy(outString, entry->d_name, length);
				}
			}
		}
	}

	// Close the directory
	closedir(currentDirectory);
}

void getSubdirectories(const char* basePath, Node_t** list){
	addToList(list, basePath);
	struct dirent* direntp = NULL;
	DIR* dirp = NULL;
	size_t pathLength;

	/* Check input parameters. */
	if (basePath == NULL) return;

	pathLength = strlen(basePath);

	if (pathLength  == 0 || pathLength > _POSIX_PATH_MAX)
		return;

	/* Open directory */
	dirp = opendir(basePath);
	if (dirp == NULL) return;

	while ((direntp = readdir(dirp)) != NULL){
		/* For every directory entry... */
		struct stat fstat;
		char fullPath [_POSIX_PATH_MAX + 1];

		/* Calculate full name, check we are in file length limts */
		if ((pathLength + strlen(direntp->d_name) + 1) > _POSIX_PATH_MAX){
			continue;
		}else{
			if (*(fullPath + pathLength - 1) != '/')
				snprintf(fullPath, pathLength + strlen(direntp->d_name) + 2, "%s/%s", basePath, direntp->d_name);
			else
				snprintf(fullPath, pathLength + strlen(direntp->d_name) + 1, "%s%s", basePath, direntp->d_name);
		}

		/* Ignore special directories. */
		if ((strcmp(direntp->d_name, ".") == 0) ||
			(strcmp(direntp->d_name, "..") == 0))
			continue;

		/* Print only if it is really directory. */
		if (stat(fullPath, &fstat) < 0)
			continue;

		if (S_ISDIR(fstat.st_mode)){
			getSubdirectories(fullPath, list);
		}
	}

    /* Finalize resources. */
    closedir(dirp);
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
	if(fileMP3 == NULL) printError(EXIT_FAILURE, FAILED_MALLOC_MSG);

	//uses a regular expression to limit size
	snprintf(fileMP3, length + 1, "%.*s.mp3", extension, songName);

	//converting
	length = strlen(songName) + strlen(fileMP3) + strlen(FFMPEG) + strlen(BITRATE);
	char* convertCommand = malloc(length + 3);
	if(convertCommand == NULL) printError(EXIT_FAILURE, FAILED_MALLOC_MSG);

	//+2 for the space in the command +1 since snprintf includes nul in buffer
	//and I dunno where the other +1 is from
	snprintf(convertCommand, length + 1, "%s%s%s%s", FFMPEG, songName, BITRATE, fileMP3);
	printf(PNT_GREEN "%s\n" PNT_RESET, convertCommand);

	if(system(convertCommand) > 0) printError(EXIT_FAILURE, CONVERT_FAIL_MSG);
	free(convertCommand);
}

int checkIfExists(const char* check){
	if(access(check, F_OK) == -1)
		return 0;
	else
		return 1;
}
