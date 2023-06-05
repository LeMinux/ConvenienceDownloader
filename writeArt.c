#include "writeArt.h"
#include "helpers.h"

void writeCovers(FILE* songNames, const char* coverArt){
	char originalName [LARGER_BUFFER_SIZE] = "";
	while(exactFileInput(songNames, originalName, LARGER_BUFFER_SIZE) != 0){
			//counts how many escape characters are needed to add
			int i = 0;
			int length = strlen(originalName);
			for(; i < length; ++i){
				switch(originalName[i]){
					case '(': case ')': case '[': case ']': case '{':
					case '}': case '\'': case '\"': case '\\': case ' ':
					case '`':
						++length;
					break;
					default: break;
				}
			}

			//add escape characters to string
			char* name = malloc(length + 1);
			if(name == NULL) printError(FAILED_MALLOC_CODE, FAILED_MALLOC_MSG);
			int a = 0;
			int p = 0;
			for(;a < strlen(originalName) && p < length; ++a, ++p){
				switch(originalName[a]){
						case '(': name[p] = '\\'; name[++p] = '('; break;
						case ')': name[p] = '\\'; name[++p] = ')'; break;
						case '[': name[p] = '\\'; name[++p] = '['; break;
						case ']': name[p] = '\\'; name[++p] = ']'; break;
						case '{': name[p] = '\\'; name[++p] = '{'; break;
						case '}': name[p] = '\\'; name[++p] = '}'; break;
						case '\'': name[p] = '\\'; name[++p] = '\''; break;
						case '`': name[p] = '\\'; name[++p] = '`'; break;
						case '\"': name[p] = '\\'; name[++p] = '\"'; break;
						case '\\': name[p] = '\\'; name[++p] = '\\'; break;
						case ' ': name[p] = '\\'; name[++p] = ' '; break;
						default: name[p] = originalName[a]; break;
				}
			}
			//access the last index and place nul
			name[length] = '\0';
			puts(name);
		if(!checkIfExists(name)){
			printf(PNT_RED"can't find %s via its path\n"PNT_RESET, originalName);
			memset(originalName, '\0', strlen(originalName));
			free(name);
		}else{
			//output is the name but with a '_' right before the .mp3
			char* output = malloc(strlen(name) + 2);
			if(output == NULL) printError(FAILED_MALLOC_CODE, FAILED_MALLOC_MSG);
			char* extension = strrchr(name, '.');
			length = extension - name;

			//add a '_' before the extension to create a temp file
			//This is because FFMPEG can't overwrite in-place
			snprintf(output, strlen(name) + 2, "%.*s_%s",length, name, extension);

			int size = strlen(FFMPEG) + 2 * strlen(DASH_I) + strlen(coverArt) + strlen(name) + strlen(OPTIONS) + strlen(output);
			char* addArtCommand = malloc(size + 1);
			if(addArtCommand == NULL) printError(FAILED_MALLOC_CODE, FAILED_MALLOC_MSG);

			//creates the command to move art to metadata
			snprintf(addArtCommand, size + 1, "%s%s%s%s%s%s%s", FFMPEG, DASH_I, name, DASH_I, coverArt, OPTIONS, output);
			puts(addArtCommand);
			if(system(addArtCommand) > 0) printError(ART_ERROR_CODE, ART_ERROR_MSG);
			free(addArtCommand);

			size = strlen(MV) + strlen(name) + strlen(output) + 1;
			char* mvCommand = malloc(size + 1);
			//move temp file as originalfile
			if(mvCommand == NULL) printError(FAILED_MALLOC_CODE, FAILED_MALLOC_MSG);
			snprintf(mvCommand, size + 1, "%s%s %s", MV, output, name);
			free(output);
			puts(mvCommand);
			if(system(mvCommand) > 0) printError(MV_TEMP_CODE, MV_TEMP_MSG);
			free(mvCommand);
			free(name);
			memset(originalName, '\0', strlen(originalName));
		}
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
