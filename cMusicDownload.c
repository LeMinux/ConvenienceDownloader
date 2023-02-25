#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//printing defines
#define PNT_GREEN "\x1b[32m"
#define PNT_RED "\x1b[31m"
#define PNT_RESET "\x1b[0m"

//error message defines
#define FAILED_MALLOC_MSG  PNT_RED"\nError MEM: Allocation of memory failed" PNT_RESET
#define INFINITE_MSG  PNT_RED"\nError INF4: infinite loop detected while downloading"PNT_RESET
#define TEMP_FILE_FAIL_MSG  PNT_RED"\nERROR READ: Error in reading temporary file"PNT_RESET
#define FILE_FAIL_MSG  PNT_RED"\nERROR GVFL: Error in reading given file. File does not exist"PNT_RESET
#define DIR_FAIL_MSG  PNT_RED"\nERROR GVDR: Error in reading given directory. Directory does not exist"PNT_RESET
#define CONVERT_FAIL_MSG  PNT_RED"\nERROR CNVT: Error in converting from .mp4 to .mp3"PNT_RESET
#define MP4_FAIL_MSG  PNT_RED"\nERROR MVP4: Error in moving video file to desired directory"PNT_RESET
#define MP3_FAIL_MSG  PNT_RED"\nERROR MVP3: Error in moving .mp3 file to /Unsynced directory"PNT_RESET
#define SKIP_VALID_MSG  PNT_RED"\nERROR INVD: Error in user skipping validation of music directories"PNT_RESET

//error code defines
#define FAILED_MALLOC_CODE 1
#define INFINITE_CODE 2
#define TEMP_FILE_FAIL_CODE 3
#define FILE_FAIL_CODE 4
#define DIR_FAIL_CODE 5
#define CONVERT_FAIL_CODE 6
#define MP4_FAIL_CODE 7
#define MP3_FAIL_CODE 8
#define SKIP_VALID_CODE 9

//some constant defines
#define YT_BASE_LENGTH 44 //this is from the beginning to end of the ID parameter

//NOTE snprintf includes the \0 in the buffer count

//printing an Error is more dynamic and more compact
//this also means there is no table to shift
void printError(int code, const char* message){
	puts(message);
	exit(code);
}

//helper method to get input and clears stdin
//also nul terminates the string
void exactUserInput(char* input, int buffer){
	int index = 0;
	int data = 0;
	switch(buffer){
		case 1:
			*input = getchar();
		break;
		default:
			while(index < buffer - 1 && (data = getchar()) != '\n')
				*(input + index++) = data;

			*(input + (index + 1)) = '\0';
		break;
	}

	//clears stdin
	if(data != '\n')
		while(getchar() != '\n'){}

}

//method for obtaining input from file streams
//returns how many characters read for any error checking needed
int exactFileInput(FILE* stream, char* dest, int buffer){
	int index = 0;
	int data = 0;
	switch(buffer){
		case 1:
			*dest = fgetc(stream);
		break;
		default:
			while(index < buffer - 1 && (data = fgetc(stream)) != EOF && data != '\n')
				*(dest + index++) = data;

			*(dest + (index + 1)) = '\0';
		break;
     }

	//clears to next line
	if(data != '\n' && data != EOF)
		while((data = fgetc(stream)) != '\n' && data != EOF){}

	printf("%d\n", index);
	return index;
}

//asks user for a youtube URL more of a helper method
char* getURL(){
	//strictly gets the portion that only contains the ID
	char buffer [YT_BASE_LENGTH];
	
	do{
		memset(buffer, '\0', YT_BASE_LENGTH);
		printf("Enter the youtube URL that you want to download -> ");
		exactUserInput(buffer, YT_BASE_LENGTH);
	}while(strlen(buffer) == 0);
	//dynamic memory since this will be passed around
	char* youtubeURL = malloc(sizeof(buffer));
	if(youtubeURL == NULL)
		printError(FAILED_MALLOC_CODE, FAILED_MALLOC_MSG);
	
	snprintf(youtubeURL, sizeof(buffer), "%s", buffer);
	return youtubeURL;
}

//downloads a song given the URL for it
void downloadURL(char* youtubeURL){
	//--restrict-filenames makes it so escape characters don't need to be added
	const char* youtubeDL = "youtube-dl --restrict-filenames ";
	int length = strlen(youtubeDL) + strlen(youtubeURL);
	char* downloadCommand = malloc(length + 1);
	if(downloadCommand == NULL)
		printError(FAILED_MALLOC_CODE, FAILED_MALLOC_MSG);

	snprintf(downloadCommand, length + 1, "%s%s", youtubeDL, youtubeURL);

	printf(PNT_GREEN "%s\n" PNT_RESET, downloadCommand);
	int retry = 0;
	while(system(downloadCommand) > 0 && retry <= 4){
		puts("Retrying download");
		++retry;
		if(retry >= 4)
			printError(INFINITE_CODE, INFINITE_MSG);
		
	}
	free(downloadCommand);
}

//helper method for getting the ID
//the Youtube ID is 11 characters encoded by base 64 so there is no need
//to keep it dynamic
//With this system youtube can store 73,786,979,294,838,206,464 videos
char* getID(char* youtubeURL){
	char* id = malloc(12);
	snprintf(id, 12, "%s", strstr(youtubeURL, "?v=") + 3);	
	return id;
}

//obtains the songName with grep and using a temp file
char* getSongName(char* id){
	const char GREP_PT1 [] = "ls | grep \"";
	const char GREP_PT2 [] = "\" > name.txt";
	
	//greps youtube ID so that it can be found
	char* grepCommand = NULL;
	
	grepCommand = malloc(strlen(id) + strlen(GREP_PT1) + strlen(GREP_PT2) + 1);
	if(grepCommand == NULL)
		printError(FAILED_MALLOC_CODE, FAILED_MALLOC_MSG);
	
		
	snprintf(grepCommand, strlen(id) + strlen(GREP_PT1) + strlen(GREP_PT2) + 1, "%s%s%s", GREP_PT1, id, GREP_PT2);
		
	printf(PNT_GREEN "%s\n" PNT_RESET, grepCommand);
	system(grepCommand);
	free(grepCommand);
	
	//getting name
	FILE* nameFile = fopen("name.txt", "r");
	if(nameFile == NULL)
		printError(TEMP_FILE_FAIL_CODE, TEMP_FILE_FAIL_MSG);
	
	//ftell() was not working for text files
	//size will include the iteration to get to EOF
	int size = 0;
	int data = '\0';
	while((data = fgetc(nameFile)) != EOF){
		++size;
	}
	printf("End is the size of %d\n", size);
	fseek(nameFile, SEEK_SET, 0);

	//<POTETNTIAL FIXED>
	char* songName = malloc(size);
	if(songName == NULL)
		printError(FAILED_MALLOC_CODE, FAILED_MALLOC_MSG);

	exactFileInput(nameFile, songName, size);
	
	fclose(nameFile);
	printf(PNT_GREEN "Converting and moving %s\n" PNT_RESET, songName);
	printf("rm name.txt\n");
	system("rm name.txt");
	return songName;
}

//converts and moves the song to the given directory
void convertMove(char* songName, const char* musicDirectory){
	const char FFMPEG [] = "ffmpeg -i ";

	//incase the file extension is not .mp4 or .mkv
	int noExtension = strrchr(songName, '.') - songName;
	char* fileMP3 = malloc(noExtension + 5);
	snprintf(fileMP3, noExtension + 5, "%.*s.mp3", noExtension, songName);

	//converting
	char* convertCommand = malloc(strlen(songName) + strlen(fileMP3) + sizeof(FFMPEG));
	if(convertCommand == NULL)
		printError(FAILED_MALLOC_CODE, FAILED_MALLOC_MSG);

	//+1 for the space in the command +1 since snprintf includes null in buffer
	snprintf(convertCommand, sizeof(FFMPEG) + strlen(songName)+ strlen(fileMP3) + 2, "%s%s %s", FFMPEG, songName, fileMP3);
	printf(PNT_GREEN "%s\n" PNT_RESET, convertCommand);

	//if(system(convertCommand) > 0)
		//printError(CONVERT_FAIL_CODE, CONVERT_FAIL_MSG);
		
	free(convertCommand);

	//moving stuff
	char* moveMP4 = malloc(strlen(songName) + strlen(musicDirectory) + 5);
	if(moveMP4 == NULL)
		printError(FAILED_MALLOC_CODE, FAILED_MALLOC_MSG);

	snprintf(moveMP4, strlen(songName) + strlen(musicDirectory) + 5, "mv %s %s",songName, musicDirectory);
	printf(PNT_GREEN "%s\n" PNT_RESET, moveMP4);
	//if(system(moveMP4) > 0)
		//printError(MP4_FAIL_CODE, MP4_FAIL_MSG);

	char* moveMP3 = malloc(strlen(fileMP3) + 14);
	if(moveMP3 == NULL)
		printError(FAILED_MALLOC_CODE, FAILED_MALLOC_MSG);

	snprintf(moveMP3, strlen(fileMP3) + 14, "mv %s Unsynced/", fileMP3);
	printf(PNT_GREEN "%s\n" PNT_RESET, moveMP3);
	//if(system(moveMP3) > 0)
		//printError(MP3_FAIL_CODE, MP3_FAIL_MSG);

	free(moveMP4);
	free(moveMP3);
	free(fileMP3);
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

//Obtains a list of directories given what root directory
//DownloadTo.txt has provided. This will result in a case sensitive list
//this also keeps <directory>/ to prevent further reading of the DownloadTo.txt
char** getDirectories(int* totalStrings){
	char* rootDir = malloc(101);
	FILE* readFile = fopen("DownloadTo.txt", "r");
	exactFileInput(readFile, rootDir, 101);
	fclose(readFile);

	char* findCommand = malloc(strlen(rootDir) + 19);
	snprintf(findCommand, strlen(rootDir) + 19, "ls -d %s* > dirs.txt", rootDir);
	free(rootDir);
	system(findCommand);
	
	//reads what directories are available
	readFile = fopen("dirs.txt", "r");
	int data = '\0';
	int length = 0;
	char** allDirectories = NULL;
	char* readingLine = malloc(25);
	if(readingLine == NULL)
		printError(FAILED_MALLOC_CODE, FAILED_MALLOC_MSG);
	
	while((data = fgetc(readFile)) != EOF){
		switch(data){
			case '\n':{
				char** temp2D = realloc(allDirectories, sizeof(char*) * (*totalStrings + 1));
				if(temp2D != NULL){
					allDirectories = temp2D;
					allDirectories[*totalStrings] = malloc(strlen(readingLine) + 1);
					if(allDirectories[*totalStrings] == NULL){
						printError(FAILED_MALLOC_CODE, FAILED_MALLOC_MSG);
					}

					snprintf(allDirectories[*totalStrings], strlen(readingLine) + 1, "%s", readingLine);
				}else{
					printError(FAILED_MALLOC_CODE, FAILED_MALLOC_MSG);
				}
				
				//resets readingLine
				free(readingLine);
				readingLine = malloc(50);
				if(readingLine == NULL){
					printError(FAILED_MALLOC_CODE, FAILED_MALLOC_MSG);
				}

				length = 0;
				*totalStrings += 1;
			}break;
			default:
				readingLine[length++] = data;
				if(length % 25 == 0){
					char* temp = NULL;
					if((temp = realloc(readingLine, length + 25)) == NULL){
						printError(FAILED_MALLOC_CODE, FAILED_MALLOC_MSG);
					}else{
						readingLine = temp;
					}
				}
				readingLine[length] = '\0';
			break;
		}
	}

	free(readingLine);
	fclose(readFile);
	system("rm dirs.txt");
	return allDirectories;
}

//gets from the user what directory they want to download into
//with the help of getDirectories
char* userDirectory(void){
	//asks user for desired directory
	int index = -1;
	int length = 0;
	char* input = malloc(101);
	if(input == NULL)
		printError(FAILED_MALLOC_CODE, FAILED_MALLOC_MSG);

	char** listOfDirs = getDirectories(&length);
	while(index == -1){
		do{
			printf("Where do you want to download the song? or type exit: ");
			exactUserInput(input, 101);
		}while(strlen(input) == 0);

		if(strcmp(input, "exit") == 0 || strcmp(input, "Exit") == 0)
			exit(0);

		int s = 0;
		for(;s < length; ++s){
			int lastIndex = strrchr(listOfDirs[s], '/') + 1 - listOfDirs[s];
			//little bit faster to check if the first
			//character matches instead of comparing the entire word
			if(listOfDirs[s][lastIndex] == input[0] && strcmp((*(listOfDirs + s) + lastIndex), input) == 0)
				index = s;

		}

		if(index == -1){
			printf(PNT_RED"\nCould not find the directory %s. Remember case matters.\n"PNT_RESET, input);
			memset(input, '\0', strlen(input));
		}
	}
	free(input);
	//sets return variable
	char* directory = malloc(strlen(listOfDirs[index]) + 1);
	snprintf(directory, strlen(listOfDirs[index]) + 1, "%s", listOfDirs[index]);

	//freeing 2D array
	int f = 0;
	for(;f < length; ++f)
		free(listOfDirs[f]);
	
	free(listOfDirs);
	return directory;
}

/*NEED TO PLACE COMMAND LINE ARGUMENTS LATER
*command line arguments will be
* -l for listing directories
* -f to specify downloading a from a list of URLs in a file
* -p for prompting each time <perhaps>
* -w for writing where to download to 
*/
int main(int argc, char** argv){
	switch(argc){
		case 1:
			//default case if only using command
			do{
				//ask for directory
				char* where = userDirectory();
				if(strcmp(where, "SKIPPED") == 0)
					printError(SKIP_VALID_CODE, SKIP_VALID_MSG);
					
				printf(PNT_GREEN"You have desired to download to %s\n"PNT_RESET, where);

				//get URL from user
				char* url = getURL();
				
				//download song via URL
				downloadURL(url);
				
				//get the ID for later movement
				char* youtubeID = getID(url);
				free(url);
				//get song name via youtube ID
				char* song = getSongName(youtubeID);
				free(youtubeID);

				//convert to mp3 and move mp4 and mp3 to its spots
				convertMove(song, where);

				free(song);
				free(where);
				puts("Download was successful");

			}while(repeat() == 1);
		break;
		case 2:
			//lists only subdirectoires in the given directory in DownloadTo.txt
			if(strcmp(argv[1], "-l") == 0){
				puts("v List of availiable directories v");
				int length = 0;
				char** listOfDirs = getDirectories(&length);
				int p = 0;
				for(;p < length; ++p){
					printf("%s\n", listOfDirs[p]);
					free(listOfDirs[p]);
				}
				FILE* downloadTo = fopen("DownloadTo.txt", "r");
				char* directory = malloc(101);
				exactFileInput(downloadTo, directory, 101);
				free(directory);
				free(listOfDirs);
			}
		break;
		case 3:
			//downloading via a file
			if(strcmp(argv[1], "-f") == 0){
				FILE* inFile = fopen(argv[2], "r");
				if(inFile == NULL)
					printError(FILE_FAIL_CODE, FILE_FAIL_MSG);

				char* where = userDirectory();
				if(strcmp(where, "SKIPPED") == 0)
					printError(SKIP_VALID_CODE, SKIP_VALID_MSG);

				//fileDownload(inFile, where);
				printf(PNT_GREEN"Downloading URLS to %s\n"PNT_RESET, where);

				char urls [YT_BASE_LENGTH] = "";
				//get URLs assumming they are separating by \n
				while(exactFileInput(inFile, urls, YT_BASE_LENGTH) == YT_BASE_LENGTH - 1){
					memset(urls,'\0', YT_BASE_LENGTH);
					//obtain a URL from the file
					exactFileInput(inFile, urls, YT_BASE_LENGTH);

					//download URL
					downloadURL(urls);
					
					//get ID for movement
					char* youtubeID = getID(urls);
					
					//obtain the song name
					char* song = getSongName(youtubeID);
					free(youtubeID);

					//convert and move files
					convertMove(song, where);
					free(song);
					song = NULL;
				}
				free(where);
				fclose(inFile);
				puts("Download was successful");
			}

			if(strcmp(argv[1], "-w") == 0){
				//checks if directory exists
				char* lsCommand = malloc(strlen(argv[2] + 16));
				//suppreses output with 1>/dev/null
				snprintf(lsCommand, strlen(argv[2]) + 16, "ls %s 1>/dev/null", argv[2]);
				if(system(lsCommand) != 0)
					printError(DIR_FAIL_CODE, DIR_FAIL_MSG);

				free(lsCommand);

				FILE* writeToFile = fopen("DownloadTo.txt", "w");
				if(argv[2][strlen(argv[2]) - 1] == '/')
					fputs(argv[2], writeToFile);
				else
					fprintf(writeToFile, "%s/\n", argv[2]);
				
				fclose(writeToFile);
				puts("Successfully written to DownloadTo.txt");
			}
		break;
		default: puts("I dunno haven't gotten this far");
	}
	return 0;
}
