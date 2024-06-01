#include "userInput.h"
#include "helpers.h"
#include "linkedList.h"
#include "fileOps.h"
#include <stdio.h>
#include "./includes/globals.h"

int clearLine(FILE* stream){
	//this way is more portable
	int data = '\0';
	while ((data = getc(stream)) != '\n' && data != EOF) { }

	if(ferror(stream) != 0){
		PRINT_ERROR("Encountered a stream error while clearing the buffer");
		return HAD_ERROR;
	}

	return NO_ERROR;
}

int exactInput(FILE* stream, char* dest, int length){
	if(fgets(dest, length, stream) == NULL){
		PRINT_ERROR("Encountered a stream error while reading the buffer");
		return HAD_ERROR;
	}
	if(strchr(dest, '\n') == NULL){
		if(clearLine(stdin) != NO_ERROR) return HAD_ERROR;
	}

	return strnlen(dest, length);
}

/*
//method for obtaining input from file streams
//returns how many characters read for any error checking needed
int exactInput(FILE* stream, char* dest, int length){
	int index = 0;
	int data = 0;
	switch(length){
		case 1:
			*dest = fgetc(stream);
		break;
		default:
			while(index < length - 1 && (data = fgetc(stream)) != EOF && data != '\n')
				*(dest + index++) = data;

			 *(dest + index) = '\0';
		break;
     }

	//clears to next line
	if(data != '\n' && data != EOF)
		while((data = fgetc(stream)) != '\n' && data != EOF){}

	return index;
}
*/

//TODO add a check for if dest in NULL
int unknownInput(FILE* stream, char** dest){
	char buffer[CHUNK_READ] = "";
	size_t inputLength = 0;
	size_t bufferLength = 0;
	int isEndLine = 0;

	while(!isEndLine){
		if(fgets(buffer, CHUNK_READ, stream) == NULL){
			if(ferror(stream)) printError(EXIT_FAILURE, "File stream error has occured");

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
				if(*dest == NULL) printError(EXIT_FAILURE, FAILED_MALLOC_MSG);

				//snprintf would also overwrite all of *dest each time and fgets nul terminates
				strcpy((*dest)+inputLength, buffer);
				inputLength += bufferLength;
			}
		}
	}

	return inputLength;
}

int getURL(char ret [YT_URL_BUFFER]){
	do{
		printf("Enter the youtube URL that you want to download -> ");

		//fgets is nul terminating so, clearing ret is not necessary
		//for each invalid attempt
		if(exactInput(stdin, ret, YT_URL_BUFFER) == HAD_ERROR) return HAD_ERROR;

		//input may be 43 characters but last could be \n
		if(strchr(ret, '\n') != NULL){
			printf(PNT_RED"URL is too short! It should look like %s[11 chars]\n"PNT_RESET, YOUTUBE_URL);
		}else{
			clearLine(stdin);
			if(strstr(ret, YOUTUBE_URL) == NULL)
				puts(PNT_RED"This is not a youtubeURL!"PNT_RESET);
			else
				return NO_ERROR;
		}
	}while(1);
}

void downloadFromURL(const char* youtubeURL, int mode, int downloadCoverArt){
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
		default: puts(PNT_RED"Client passed an unknown mode for downloading URL"PNT_RESET); exit(1); break;
	}

	int length = strlen(youtubeDL) + strlen(youtubeURL);
	char* downloadCommand = malloc(length + 1);
	if(downloadCommand == NULL) printError(EXIT_FAILURE, FAILED_MALLOC_MSG);
	snprintf(downloadCommand, length + 1, "%s%s", youtubeDL, youtubeURL);
	printf(PNT_GREEN "%s\n" PNT_RESET, downloadCommand);
	if(system(downloadCommand) > 0) printError(EXIT_FAILURE, DOWNLOAD_FAIL_MSG);
	free(downloadCommand);
}


//helper method for if the user wants to repeat or not
int askToRepeat(void){
	char yesNo = '\0';
	do{
		printf("Do you want to download again? Y/N: ");
		yesNo = fgetc(stdin);
		if(yesNo != '\n') clearLine(stdin);
		switch(yesNo){
			case 'y': case 'Y': return 1; break;
			case 'n': case 'N': return 0; break;
			default: puts("~~Invalid input~~"); break;
		}
	}while(1);

	//incase of some wack error
	return 0;
}

//gets from the user what directory they want to download into
//with the help of getDirectories
char* getUserChoiceForDir(const char* baseDir, const char* prompt){
	//asks user for desired directory
	int found = -1;
	char* returnDir = NULL;
	Node_t* listOfDirs = NULL;
	getSubdirectories(baseDir, &listOfDirs);

	char* input = malloc(101);
	if(input == NULL) printError(EXIT_FAILURE, FAILED_MALLOC_MSG);

	while(found == -1){
		do{
			printList(listOfDirs);
			printf("%s", prompt);
			exactInput(stdin, input, 101);
		}while(strlen(input) == 0);

		if(strcmp(input, "exit") == 0 || strcmp(input, "Exit") == 0){
			exit(0);
		}else if(strcmp(input, "skip") == 0 || strcmp(input, "Skip") == 0){
			returnDir = malloc(5);
			snprintf(returnDir, 5, "%s","SKIP");
			found = 1;
		}else if((found = containsElement(listOfDirs, input)) != -1){
			returnDir = getElement(listOfDirs, found);
			found = 1;
		}else{
			printf(PNT_RED"\nCould not find the directory %s. Remember case matters.\n"PNT_RESET, input);
			memset(input, '\0', strlen(input));
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
	if(input == NULL) printError(EXIT_FAILURE, FAILED_MALLOC_MSG);

	while(found == -1){
		do{
			printList(listOfDirs);
			printf("%s", prompt);
			exactInput(stdin, input, 101);
			//fgets(input, 101, stdin);
			//clearLine(stdin);

		}while(strlen(input) == 0);

		if(strcmp(input, "exit") == 0 || strcmp(input, "Exit") == 0){
			exit(0);
		}else if((found = containsElement(listOfDirs, input)) != -1){
			returnDir = getElement(listOfDirs, found);
			found = 1;
		}else{
			printf(PNT_RED"\nCould not find the directory %s. Remember case matters.\n"PNT_RESET, input);
			memset(input, '\0', strlen(input));
		}
	}

	free(input);
	deleteList(&listOfDirs);
	return returnDir;
}
