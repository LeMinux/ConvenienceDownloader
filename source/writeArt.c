#include "../includes/writeArt.h"
#include "../includes/userInput.h"
#include "../includes/fileOps.h"

void writeCovers(FILE* songNames, const char* coverArt){
	if(songNames == NULL || coverArt == NULL) printAndExit(EXIT_FAILURE, "Can't add cover art because one argument is NULL");

	char originalName [LARGER_BUFFER_SIZE] = "";
	while(exactInput(songNames, originalName, LARGER_BUFFER_SIZE) != 0){
		char* name = surroundInQuotes(originalName);
		if(!checkIfExists(name)){
			printf(PNT_RED"can't find %s via its path\n"PNT_RESET, originalName);
			memset(originalName, '\0', strlen(originalName));
			free(name);
		}else{
			//output is the name but with a '_' right before the .mp3
			char* output = malloc(strlen(name) + 2);
			if(output == NULL) printAndExit(EXIT_FAILURE, FAILED_MALLOC_MSG);
			char* extension = strrchr(name, '.');
			int length = extension - name;

			//add a '_' before the extension to create a temp file
			//This is because FFMPEG can't overwrite in-place
			snprintf(output, strlen(name) + 2, "%.*s_%s",length, name, extension);

			int size = strlen(FFMPEG) + 2 * strlen(DASH_I) + strlen(coverArt) + strlen(name) + strlen(OPTIONS) + strlen(output);
			char* addArtCommand = malloc(size + 1);
			if(addArtCommand == NULL) printAndExit(EXIT_FAILURE, FAILED_MALLOC_MSG);

			//creates the command to move art to metadata
			snprintf(addArtCommand, size + 1, "%s%s%s%s%s%s%s", FFMPEG, DASH_I, name, DASH_I, coverArt, OPTIONS, output);
			puts(addArtCommand);
			if(system(addArtCommand) > 0) printAndExit(EXIT_FAILURE, ART_ERROR_MSG);
			free(addArtCommand);

			//exit since the temp name is not expect for use
			if(moveFile(output, originalName) == HAD_ERROR){
				PRINT_ERROR("Failed to rename file from its temp name");
				exit(EXIT_FAILURE);
			}
			free(name);
			memset(originalName, '\0', strlen(originalName));
		}
	}
}

void writeCover(const char* songName, const char* coverArt){
	if(songName == NULL || coverArt == NULL) printAndExit(EXIT_FAILURE, "Can't add cover art because one argument is NULL");

	char* name = surroundInQuotes(songName);

	//output is the name but with a '_' right before the .mp3
	char* output = malloc(strlen(name) + 2);
	if(output == NULL) printAndExit(EXIT_FAILURE, FAILED_MALLOC_MSG);
	char* extension = strrchr(name, '.');
	int length = extension - name;

	//add a '_' before the extension to create a temp file
	//This is because FFMPEG can't overwrite in-place
	snprintf(output, strlen(name) + 2, "%.*s_%s",length, name, extension);

	int size = strlen(FFMPEG) + 2 * strlen(DASH_I) + strlen(coverArt) + strlen(name) + strlen(OPTIONS) + strlen(output);
	char* addArtCommand = malloc(size + 1);
	if(addArtCommand == NULL) printAndExit(EXIT_FAILURE, FAILED_MALLOC_MSG);

	//creates the command to move art to metadata
	snprintf(addArtCommand, size + 1, "%s%s%s%s%s%s%s", FFMPEG, DASH_I, name, DASH_I, coverArt, OPTIONS, output);
	puts(addArtCommand);
	if(system(addArtCommand) > 0) printAndExit(EXIT_FAILURE, ART_ERROR_MSG);
	free(addArtCommand);

	//exit since the temp name is not expect for use
	if(moveFile(output, name) == HAD_ERROR){
		PRINT_ERROR("Failed to rename file from its temp name");
		exit(EXIT_FAILURE);
	}
	free(output);
	free(name);
}

void removeCoverArt(const char* artName){
	char* removeArt = malloc(3 + strlen(artName) + 1);
	snprintf(removeArt, 3 + strlen(artName) + 1, "%s%s", "rm ", artName);
	system(removeArt);
	free(removeArt);
	removeArt = NULL;
}
