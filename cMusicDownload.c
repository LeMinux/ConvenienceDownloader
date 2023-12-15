#include "cMusicDownload.h"
#include "helpers.h"
#include "linkedList.h"
#include <bits/posix1_lim.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//asks user for a youtube URL
char* getURL(void){
	//strictly gets the portion that only contains the ID
	char buffer [YT_URL_BUFFER];
	do{
		memset(buffer, '\0', YT_URL_BUFFER);
		printf("Enter the youtube URL that you want to download -> ");
		exactUserInput(buffer, YT_URL_BUFFER);
	}while(strlen(buffer) == 0);
	//dynamic memory since this will be passed around
	char* youtubeURL = malloc(sizeof(buffer));
	if(youtubeURL == NULL) printError(EXIT_FAILURE, FAILED_MALLOC_MSG);

	snprintf(youtubeURL, sizeof(buffer), "%s", buffer);
	return youtubeURL;
}

//downloads a song given the URL for it
void downloadURL(const char* youtubeURL, int mode){
	//--restrict-filenames makes it so escape characters don't need to be added
	//-f bestvideo to force as .mp4
	//--write-thumbnail to get thumnail
	//--convert-thumbnail since the default is webp
	//-R to specify 4 retries
	const char* youtubeDL = NULL;
	switch(mode){
		case 0:
			youtubeDL = "yt-dlp"
				" -f bestvideo[ext=mp4]+bestaudio[ext=m4a]/best[ext=mp4]/best"
				" --restrict-filenames -R 4 ";
		break;
		case 1:
			youtubeDL = "yt-dlp"
				" -f bestvideo[ext=mp4]+bestaudio[ext=m4a]/best[ext=mp4]/best "
				" --restrict-filenames "
				" --write-thumbnail "
				" --convert-thumbnails jpg "
				" -R 4 ";
		break;
		default: puts(PNT_RED"Client passed an unkown mode for downloading URL"PNT_RESET); exit(1); break;
	}

	int length = strlen(youtubeDL) + strlen(youtubeURL);
	char* downloadCommand = malloc(length + 1);
	if(downloadCommand == NULL) printError(EXIT_FAILURE, FAILED_MALLOC_MSG);
	snprintf(downloadCommand, length + 1, "%s%s", youtubeDL, youtubeURL);
	printf(PNT_GREEN "%s\n" PNT_RESET, downloadCommand);
	if(system(downloadCommand) > 0) printError(EXIT_FAILURE, DOWNLOAD_FAIL_MSG);
	free(downloadCommand);
}

/*
 *The Youtube ID is 11 characters encoded by base 64 so there is no need
 *to keep it dynamic
 *With this system youtube can store 73,786,979,294,838,206,464 videos
 *Youtube will also truncate to 11 if the ID is greater than 11
 */
char* getID(const char* youtubeURL){
	char* id = malloc(YT_ID_SIZE + 1);
	if(id == NULL) printError(EXIT_FAILURE, FAILED_MALLOC_MSG);
	snprintf(id, YT_ID_SIZE + 1, "%s", strstr(youtubeURL, "?v=") + 3);
	return id;
}

//obtains the songName with grep and using a temp file
//avoid using this when trying to also get cover art
char* getSongName(const char* id){
	grepIntoFile(id);
	//getting name
	FILE* nameFile = fopen("GrepTemp.txt", "r");
	if(nameFile == NULL) printError(EXIT_FAILURE, TEMP_FILE_FAIL_MSG);
	char* songName = NULL;
	unknownInput(nameFile, &songName);

	fclose(nameFile);
	printf(PNT_GREEN "Converting and moving %s\n" PNT_RESET, songName);
	system("rm GrepTemp.txt");
	return songName;
}


 //converts a file into an mp3 via ffmpeg
 //this returns the newly created mp3 file
 char* convertToMp3(const char* songName){
	//create mp3 file name
	const char FFMPEG [] = "ffmpeg -i ";
	const char MP3_EXTENSTION [] = ".mp3";
	//incase the file extension is not .mp4 or .mkv
	int extension = strrchr(songName, '.') - songName;
	int length = extension + strlen(MP3_EXTENSTION);
	char* fileMP3 = malloc(length + 1);
	if(fileMP3 == NULL) printError(EXIT_FAILURE, FAILED_MALLOC_MSG);

	//uses a regular expression to limit size
	snprintf(fileMP3, length + 1, "%.*s.mp3", extension, songName);

	//converting
	length = strlen(songName) + strlen(fileMP3) + strlen(FFMPEG);
	char* convertCommand = malloc(length + 3);
	if(convertCommand == NULL) printError(EXIT_FAILURE, FAILED_MALLOC_MSG);

	//+1 for the space in the command +1 since snprintf includes nul in buffer
	snprintf(convertCommand, length + 3, "%s%s %s", FFMPEG, songName, fileMP3);
	printf(PNT_GREEN "%s\n" PNT_RESET, convertCommand);

	if(system(convertCommand) > 0) printError(EXIT_FAILURE, CONVERT_FAIL_MSG);
	free(convertCommand);
	return fileMP3;
 }

 void moveFile(const char* file, const char* destination){
	const char MOVE [] = "mv ";
	int length = strlen(file) + strlen(destination) + strlen(MOVE);
	//+1 for space
	char* moveCommand = malloc(length + 2);
	snprintf(moveCommand, length + 2, "%s%s %s", MOVE, file, destination);
	printf("%s\n", moveCommand);
	system(moveCommand);
	free(moveCommand);
 }

//helper method for if the user wants to repeat or not
int repeat(void){
	char yesNo = '\0';
	do{
		printf("Do you want to download another song? Y/N: ");
		exactUserInput(&yesNo, 1);
		switch(yesNo){
			case 'y': case 'Y': return 1; break;
			case 'n': case 'N': return 0; break;
			default: puts("~~Invalid input~~"); break;
		}
	}while(1==1);

	//incase of some wack error
	return 0;
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
			//fprintf(stderr, "concatonating %s and %s is larger than max limit of %d", basePath, direntp->d_name, _POSIX_PATH_MAX);
			//printError(EXIT_FAILURE, "Directory name is too large");
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
			exactUserInput(input, 101);
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
			exactUserInput(input, 101);
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
