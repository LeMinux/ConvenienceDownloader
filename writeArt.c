#include "writeArt.h"
#include "helpers.h"

void writeCovers(FILE* songNames, const char* coverArt){
	char name [LARGER_BUFFER_SIZE];
	while(exactFileInput(songNames, name, LARGER_BUFFER_SIZE) != 0){
		if(!checkIfExists(name)){
			printf(PNT_RED"can't find %s via its path"PNT_RESET, name);
		}
		char* output = malloc(strlen(name) + 2);
		if(output == NULL) printError(FAILED_MALLOC_CODE, FAILED_MALLOC_MSG);
		char* extension = strrchr(name, '.');
		int length = extension - name;

		//add a '_' before the extension to create a temp file
		//This is because FFMPEG can't overwrite in-place
		snprintf(output, strlen(name) + 2, "%.*s_%s",length, name, extension);

		int size = strlen(FFMPEG) + 2 * strlen(DASH_I) + strlen(coverArt) + strlen(name) + strlen(OPTIONS) + strlen(output);
		char* addArtCommand = malloc(size + 1);
		if(addArtCommand == NULL) printError(FAILED_MALLOC_CODE, FAILED_MALLOC_MSG);

		snprintf(addArtCommand, size + 1, "%s%s%s%s%s%s%s", FFMPEG, DASH_I, name, DASH_I, coverArt, OPTIONS, output);
		puts(addArtCommand);
		if(system(addArtCommand) > 0) printError(ART_ERROR_CODE, ART_ERROR_MSG);
		free(addArtCommand);

		size = strlen(MV) + strlen(name) + strlen(output) + 1;
		char* mvCommand = malloc(size + 1);
		if(mvCommand == NULL) printError(FAILED_MALLOC_CODE, FAILED_MALLOC_MSG);
		snprintf(mvCommand, size + 1, "%s%s %s", MV, output, name);
		free(output);
		puts(mvCommand);
		if(system(mvCommand) > 0) printError(MV_TEMP_CODE, MV_TEMP_MSG);
		free(mvCommand);
		memset(name, '\0', LARGER_BUFFER_SIZE);
	}
}

void writeCover(const char* songName, const char* coverArt){
	char* output = malloc(strlen(songName) + 2);
	if(output == NULL) printError(FAILED_MALLOC_CODE, FAILED_MALLOC_MSG);
	snprintf(output, strlen(songName) + 2, "_%s",songName);

	int size = strlen(FFMPEG) + 2 * strlen(DASH_I) + strlen(coverArt) + strlen(songName) + strlen(OPTIONS) + strlen(output);
	char* addArtCommand = malloc(size + 1);
	if(addArtCommand == NULL) printError(FAILED_MALLOC_CODE, FAILED_MALLOC_MSG);

	//the '_' in the front of the last %s is there since ffmpeg can't write in-place
	snprintf(addArtCommand, size + 1, "%s%s%s%s%s%s_%s", FFMPEG, DASH_I, songName, DASH_I, coverArt, OPTIONS, songName);
	puts(addArtCommand);
	if(system(addArtCommand) > 0) printError(ART_ERROR_CODE, ART_ERROR_MSG);
	free(addArtCommand);

	size = strlen(MV) + strlen(songName) + strlen(output) + 1;
	char* mvCommand = malloc(size + 1);
	if(mvCommand == NULL) printError(FAILED_MALLOC_CODE, FAILED_MALLOC_MSG);
	snprintf(mvCommand, size + 1, "%s%s %s", MV, output, songName);
	free(output);
	puts(mvCommand);
	if(system(mvCommand) > 0) printError(MV_TEMP_CODE, MV_TEMP_MSG);
	free(mvCommand);
}
