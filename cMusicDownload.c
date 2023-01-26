#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//for making code look nicer
void printError(int code){
	switch(code){
		case 1: puts("Error MEM: Allocation of memory failed"); exit(1);
		break;
		case 2: puts("Error INF4: infinite loop detected while downloading"); exit(2);
		break;
		case 3: puts("Error FILE: Error in creating temporary file"); exit(3);
		break;
		case 4: puts("ERROR READ: Error in reading temporary file"); exit(4);
		break;
		case 5: puts("ERROR GVFL: Error in reading given file. File does not exist"); exit(5);
		break;
		case 6: puts("ERROR GVDR: Error in reading given directory. Directory does not exist"); exit(6);
		break;
		case 7: puts("ERROR CNVT: Error in converting from .mp4 to .mp3"); exit(7);
		break;
		case 8: puts("ERROR MVP4: Error in moving video file to desired directory"); exit(8);
		break;
		case 9: puts("ERROR MVP3: Error in moving .mp3 file to /Unsynced directory"); exit(9);
		break;
		case 10: puts("ERROR INVD: Error in user skipping validation of music directories"); exit(10);
		default: puts("An error has occured"); exit(-1);
	}
}

//helper method to get input and clears stdin
//also nul terminates the string
void exactUserInput(char* input, int buffer){
	int index = 0;
	int data = 0;
	if(buffer != 1){
		while(index < buffer - 1 && (data = getchar()) != '\n')
			*(input + index++) = data;
		
		*(input + (buffer - 1)) = '\0';
	}else{
		*input = getchar();
	}	

	//clears stdin
	if(data != '\n')
		while(getchar() != '\n'){}

}

//method for obtaining input from file streams
void exactFileInput(FILE* stream, char* dest, int buffer){
	int index = 0;
	int data = 0;
	while(index < buffer - 1 && (data = fgetc(stream)) != EOF && data != '\n')
		*(dest + index++) = data;
	
	*(dest + (buffer - 1)) = '\0';
}

//asks user for a youtube URL more of a helper method
char* getURL(){
	//strictly gets the portion that only contains the ID
	char buffer [44];
	
	do{
		memset(buffer, '\0', 44);	
		printf("Enter the youtube URL that you want to download -> ");
		exactUserInput(buffer, 44);
	}while(strlen(buffer) == 0);
	//dynamic memory since this will be passed around
	char* youtubeURL = malloc(sizeof(buffer));
	if(youtubeURL == NULL)
		printError(1);
	
	snprintf(youtubeURL, sizeof(buffer), "%s", buffer);
	return youtubeURL;
}

//downloads a song given the URL for it
void downloadURL(char* youtubeURL){
	//--restrict-filenames makes it so escape characters don't need to be added
	char downloadCommand [78]= "";
	snprintf(downloadCommand, strlen(youtubeURL) + 33, "youtube-dl --restrict-filenames %s", youtubeURL);

	printf("%s\n", downloadCommand);
	int retry = 0;
	while(system(downloadCommand) > 0 && retry <= 4){
		puts("Retrying download");
		++retry;
		if(retry >= 4)
			printError(2);
		
	}
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
		printError(1);
	
		
	snprintf(grepCommand, strlen(id) + strlen(GREP_PT1) + strlen(GREP_PT2) + 1, "%s%s%s", GREP_PT1, id, GREP_PT2);
		
	printf("%s\n", grepCommand);
	system(grepCommand);
	free(grepCommand);
	
	//getting name
	FILE* nameFile = fopen("name.txt", "r");
	if(nameFile == NULL)
		printError(3);
	
	
	int chr = 0;
	//size follows where nul is
	int size = 1;
	char* songName = NULL;
	chr = fgetc(nameFile);
	while(chr != EOF && chr != '\n'){
		char* temp = realloc(songName, size + 1);			
		if(temp == NULL)
			printError(1);
		else
			songName = temp;
		
		songName[size - 1] = chr;
		chr = fgetc(nameFile);
		songName[size] = '\0';
		++size;
	}
	//checks if it actually did read the file
	if(size == 0)
		printError(4);
	
	
	fclose(nameFile);
	printf("Converting and moving %s\n", songName);
	printf("rm name.txt\n");
	system("rm name.txt");
	return songName;
}

//converts and moves the song to the given directory
void convertMove(char* songName, const char* musicDirectory){
	const char FFMPEG [] = "ffmpeg -i ";
	
	//converting
	char* convertCommand = malloc(strlen(songName)*2 + sizeof(FFMPEG));
	if(convertCommand == NULL)
		printError(1);
	
	
	int noExtension = strrchr(songName, '.') - songName;
	//not sure why I have to add +2
	snprintf(convertCommand, strlen(FFMPEG) + strlen(songName) * 2 + 2, "%s%s %.*s.mp3", FFMPEG, songName, noExtension,songName);
	printf("%s", convertCommand);
	if(system(convertCommand) > 0)
		printError(6);
		
	free(convertCommand);

	//moving stuff
	char* moveMP4 = malloc(strlen(songName) + strlen(musicDirectory) + 9);
	if(moveMP4 == NULL)
		printError(1);
		
	char* moveMP3 = malloc(strlen(songName) + 14);
	if(moveMP3 == NULL)
		printError(1);
			
	snprintf(moveMP4, strlen(songName) + strlen(musicDirectory) + 9, "mv %s %s",songName, musicDirectory);
	snprintf(moveMP3, strlen(songName)+ 18, "mv %.*s.mp3 Unsynced/", noExtension, songName);
	
	printf("%s\n", moveMP4);
	if(system(moveMP4) > 0)
		printError(7);

	printf("%s\n", moveMP3);
	if(system(moveMP3) > 0)
		printError(8);
	
	free(moveMP4);
	free(moveMP3);
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
		printError(3);
	
	while((data = fgetc(readFile)) != EOF){
		switch(data){
			case '\n':{
				char** temp2D = realloc(allDirectories, sizeof(char*) * (*totalStrings + 1));
				if(temp2D != NULL){
					allDirectories = temp2D;
					allDirectories[*totalStrings] = malloc(strlen(readingLine) + 1);
					if(allDirectories[*totalStrings] == NULL){
						printError(1);
					}

					snprintf(allDirectories[*totalStrings], strlen(readingLine) + 1, "%s", readingLine);
				}else{
					printError(1);
				}
				
				//resets readingLine
				free(readingLine);
				readingLine = malloc(50);
				if(readingLine == NULL){
					printError(1);
				}

				length = 0;
				*totalStrings += 1;
			}break;
			default:
				readingLine[length++] = data;
				if(length % 25 == 0){
					char* temp = NULL;
					if((temp = realloc(readingLine, length + 25)) == NULL){
						printError(1);
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
			printf("\nCould not find the directory %s. Remember case matters.\n", input);
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
					printError(9);
				
					
				printf("You have desired to download to %s\n", where);

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
					printError(5);
					
				char* where = userDirectory();
				if(strcmp(where, "SKIPPED") == 0)
					printError(9);
					
				//fileDownload(inFile, where);
				printf("Downloading URLS to VLC/%s\n", where);
		
				char urls [101] = "";
				//get URLs assumming they are separating by \n
				while(fgets(urls, 101, inFile) != NULL){
					//get rid of \n
					urls[strcspn(urls, "\n")] = '\0';

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
					free(where);
				}
				fclose(inFile);
				puts("Download was successful");
			}

			if(strcmp(argv[1], "-w") == 0){
				//checks if directory exists
				char* lsCommand = malloc(strlen(argv[2] + 16));
				//suppreses output with 1>/dev/null
				snprintf(lsCommand, strlen(argv[2]) + 16, "ls %s 1>/dev/null", argv[2]);
				if(system(lsCommand) != 0)
					printError(6);

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
