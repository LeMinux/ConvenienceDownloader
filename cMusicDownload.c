#include "cMusicDownload.h"
#include "helpers.h"
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
	if(youtubeURL == NULL) printError(FAILED_MALLOC_CODE, FAILED_MALLOC_MSG);

	snprintf(youtubeURL, sizeof(buffer), "%s", buffer);
	return youtubeURL;
}

//downloads a song given the URL for it
void downloadURL(char* youtubeURL){
	//--restrict-filenames makes it so escape characters don't need to be added
	//-R to specify 4 retries
	const char* youtubeDL = "yt-dlp --restrict-filenames -R 4 ";
	int length = strlen(youtubeDL) + strlen(youtubeURL);
	char* downloadCommand = malloc(length + 1);
	if(downloadCommand == NULL) printError(FAILED_MALLOC_CODE, FAILED_MALLOC_MSG);

	snprintf(downloadCommand, length + 1, "%s%s", youtubeDL, youtubeURL);
	printf(PNT_GREEN "%s\n" PNT_RESET, downloadCommand);
	if(system(downloadCommand) > 0) printError(DOWNLOAD_FAIL_CODE, DOWNLOAD_FAIL_MSG);
	free(downloadCommand);
}

/*
 *The Youtube ID is 11 characters encoded by base 64 so there is no need
 *to keep it dynamic
 *With this system youtube can store 73,786,979,294,838,206,464 videos
 *Youtube will also truncate to 11 if the ID is greater than 11
 */
char* getID(char* youtubeURL){
	char* id = malloc(YT_ID_SIZE + 1);
	if(id == NULL) printError(FAILED_MALLOC_CODE, FAILED_MALLOC_MSG);
	snprintf(id, YT_ID_SIZE + 1, "%s", strstr(youtubeURL, "?v=") + 3);
	return id;
}

//obtains the songName with grep and using a temp file
char* getSongName(char* id){
	const char GREP_PT1 [] = "ls | grep \"";
	const char GREP_PT2 [] = "\" > name.txt";
	
	//greps youtube ID so that it can be found
	char* grepCommand = NULL;

	//checks if the id needs escape characters for the grep command
	//the '-' character is the culprit if it's at the beginning
	if(id[0] == '-'){
		char newId [YT_ID_SIZE + 2];
		int n = 1;
		int s = 0;
		for(;s < strlen(id);++n, ++s)
			newId[n] = id[s];
		
		newId[YT_ID_SIZE + 1] = '\0';
		newId[0] = '\\';
		grepCommand = malloc(YT_ID_SIZE + 1 + strlen(GREP_PT1) + strlen(GREP_PT2) + 1);
		if(grepCommand == NULL) printError(FAILED_MALLOC_CODE, FAILED_MALLOC_MSG);
		snprintf(grepCommand,YT_ID_SIZE + 1 + strlen(GREP_PT1) + strlen(GREP_PT2) + 1, "%s%s%s", GREP_PT1, newId, GREP_PT2);
	}else{
		grepCommand = malloc(strlen(id) + strlen(GREP_PT1) + strlen(GREP_PT2) + 1);
		if(grepCommand == NULL) printError(FAILED_MALLOC_CODE, FAILED_MALLOC_MSG);
		snprintf(grepCommand, strlen(id) + strlen(GREP_PT1) + strlen(GREP_PT2) + 1, "%s%s%s", GREP_PT1, id, GREP_PT2);
	}

	printf(PNT_GREEN "%s\n" PNT_RESET, grepCommand);
	if(system(grepCommand) != 0) printError(SONG_NOT_CODE, SONG_NOT_MSG);
	free(grepCommand);

	//getting name
	FILE* nameFile = fopen("name.txt", "r");
	if(nameFile == NULL) printError(TEMP_FILE_FAIL_CODE, TEMP_FILE_FAIL_MSG);

	//ftell() was not working for text files
	//size will include the iteration to get to EOF
	int size = 0;
	int data = '\0';
	while((data = fgetc(nameFile)) != EOF){
		++size;
	}
	fseek(nameFile, SEEK_SET, 0);

	char* songName = malloc(size);
	if(songName == NULL) printError(FAILED_MALLOC_CODE, FAILED_MALLOC_MSG);

	exactFileInput(nameFile, songName, size);
	
	fclose(nameFile);
	printf(PNT_GREEN "Converting and moving %s\n" PNT_RESET, songName);
	printf("rm name.txt\n");
	system("rm name.txt");
	return songName;
}


 //converts a file into an mp3 via ffmpeg
 //this returns the newly created mp3 file
 char* convertToMp3(char* songName){
	//create mp3 file name
	const char FFMPEG [] = "ffmpeg -i ";
	const char MP3_EXTENSTION [] = ".mp3";
	//incase the file extension is not .mp4 or .mkv
	int extension = strrchr(songName, '.') - songName;
	int length = extension + strlen(MP3_EXTENSTION);
	char* fileMP3 = malloc(length + 1);
	if(fileMP3 == NULL) printError(FAILED_MALLOC_CODE, FAILED_MALLOC_MSG);

	//uses a regular expression to limit size
	snprintf(fileMP3, length + 1, "%.*s.mp3", extension, songName);

	//converting
	length = strlen(songName) + strlen(fileMP3) + strlen(FFMPEG);
	char* convertCommand = malloc(length + 3);
	if(convertCommand == NULL) printError(FAILED_MALLOC_CODE, FAILED_MALLOC_MSG);

	//+1 for the space in the command +1 since snprintf includes nul in buffer
	snprintf(convertCommand, length + 3, "%s%s %s", FFMPEG, songName, fileMP3);
	printf(PNT_GREEN "%s\n" PNT_RESET, convertCommand);

	if(system(convertCommand) > 0) printError(CONVERT_FAIL_CODE, CONVERT_FAIL_MSG);
	free(convertCommand);
	return fileMP3;
 }

 void moveFile(char* file, const char* destination){
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

Node_t* getDirectories(void){
	const char LS [] = "ls -d ";
	const char TO_DIRS [] = "* > dirs.txt";
	int data = '\0';
	int length = 0;
	FILE* fileWithDir = fopen(WHERE_SEND_FILES, "r");
	if(fileWithDir == NULL) printError(DOWNLOAD_TO_CODE, DOWNLOAD_TO_FAIL);

	//reads the line in the file
	//reallocs until it has read the full line for more efficiency
	char* downloadDir = malloc(length + 1);
	while((data = fgetc(fileWithDir)) != EOF && data != '\n'){
		downloadDir[length++] = data;
		char* temp = NULL;
		if((temp = realloc(downloadDir, length + 1)) == NULL)
			printError(FAILED_MALLOC_CODE, FAILED_MALLOC_MSG);
		else
			downloadDir = temp;
	}
	fclose(fileWithDir);
	downloadDir[length] = '\0';

	length += strlen(LS) + strlen(TO_DIRS);
	char* findCommand = malloc(length + 1);
	snprintf(findCommand, length + 1, "%s%s%s", LS, downloadDir, TO_DIRS);
	if(system(findCommand) != 0){
		printf(PNT_RED"Could not find directory file %s\n"PNT_RESET, downloadDir);
		printError(DIR_FAIL_CODE, DIR_FAIL_MSG);
	}
	free(findCommand);
	free(downloadDir);
	
	FILE* tempFile = fopen("dirs.txt", "r");
	if(tempFile == NULL) printError(TEMP_FILE_FAIL_CODE, TEMP_FILE_FAIL_MSG);

	//variables for creating list
	data = '\0';
	length = 0;
	Node_t* list = NULL;
	char* readingLine = malloc(25);
	if(readingLine == NULL) printError(FAILED_MALLOC_CODE, FAILED_MALLOC_MSG);

	//reads what directories are available
	while((data = fgetc(tempFile)) != EOF){
		switch(data){
			case '\n':
				addToList(&list, readingLine);
				//resets readingLine
				free(readingLine);
				readingLine = malloc(25);
				if(readingLine == NULL) printError(FAILED_MALLOC_CODE, FAILED_MALLOC_MSG);
				length = 0;
			break;
			default:
				//adjust buffer size if needed
				readingLine[length++] = data;
				char* temp = NULL;
				if((temp = realloc(readingLine, length + 1)) == NULL)
					printError(FAILED_MALLOC_CODE, FAILED_MALLOC_MSG);
				else
					readingLine = temp;

				readingLine[length] = '\0';
			break;
		}
	}

	free(readingLine);
	fclose(tempFile);
	system("rm dirs.txt");
	return list;
}

//gets from the user what directory they want to download into
//with the help of getDirectories
char* userDirectory(void){
	//asks user for desired directory
	int found = -1;
	char* returnDir = NULL;
	Node_t* listOfDirs = getDirectories();

	char* input = malloc(101);
	if(input == NULL) printError(FAILED_MALLOC_CODE, FAILED_MALLOC_MSG);

	while(found == -1){
		do{
			printList(listOfDirs);
			printf("Where do you want to download the song? or type exit: ");
			exactUserInput(input, 101);
		}while(strlen(input) == 0);

		if(strcmp(input, "exit") == 0 || strcmp(input, "Exit") == 0)
			exit(0);

		if((found = containsElement(listOfDirs, input)) != -1){
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
				if(where == NULL) printError(SKIP_VALID_CODE, SKIP_VALID_MSG);

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
				char* mp3 = convertToMp3(song);
				moveFile(song, where);
				moveFile(mp3, "../Unsynced/");
				//convertMove(song, where);

				free(mp3);
				free(song);
				free(where);
				puts("Download was successful");
			}while(repeat() == 1);
		break;
		case 2:
			//lists only subdirectoires in the given directory in DownloadTo.txt
			if(strcmp(argv[1], "-l") == 0){
				puts("v List of availiable directories v");
				Node_t* listOfDirs = getDirectories();
				printList(listOfDirs);
				deleteList(&listOfDirs);
			}
		break;
		case 3:
			//downloading via a file
			if(strcmp(argv[1], "-f") == 0){
				FILE* inFile = fopen(argv[2], "r");
				if(inFile == NULL) printError(FILE_FAIL_CODE, FILE_FAIL_MSG);

				char* where = userDirectory();
				if(where == NULL) printError(SKIP_VALID_CODE, SKIP_VALID_MSG);

				//fileDownload(inFile, where);
				printf(PNT_GREEN"Downloading URLS to %s\n"PNT_RESET, where);

				char urls [YT_URL_BUFFER] = "";
				//get URLs assumming they are separating by \n
				while(exactFileInput(inFile, urls, YT_URL_BUFFER) == YT_URL_BUFFER){
					//download URL
					downloadURL(urls);
					
					//get ID for movement
					char* youtubeID = getID(urls);
					
					//obtain the song name
					char* song = getSongName(youtubeID);
					free(youtubeID);

					//convert and move files
					//convertMove(song, where);
					char* mp3 = convertToMp3(song);
					moveFile(song, where);
					moveFile(mp3, "../Unsynced/");

					free(mp3);
					free(song);
					memset(urls,'\0', YT_URL_BUFFER);
				}
				free(where);
				fclose(inFile);
				puts("Download was successful");
			}

			//where to send mp4(video) files
			if(strcmp(argv[1], "-w4") == 0){
				const char ls [] = "ls ";
				//suppreses output with 1>/dev/null
				const char suppress [] = " 1>/dev/null";
				//checks if directory exists
				int length = strlen(argv[2]) + strlen(ls) + strlen(suppress);
				char* lsCommand = malloc(length + 1);
				snprintf(lsCommand, length + 1, "ls %s 1>/dev/null", argv[2]);
				if(system(lsCommand) != 0) printError(DIR_FAIL_CODE, DIR_FAIL_MSG);
				free(lsCommand);

				FILE* writeToFile = fopen("DownloadTo.txt", "w");
				if(argv[2][strlen(argv[2]) - 1] == '/')
					fputs(argv[2], writeToFile);
				else
					fprintf(writeToFile, "%s/\n", argv[2]);
				
				fclose(writeToFile);
				puts("Successfully written to DownloadTo.txt where to send video files");
			}

			//FINISH IMPLEMETNING THIS this is for setting up where to send mp3 files
			if(strcmp(argv[1], "-w3") == 0){
				const char ls [] = "ls ";
				//suppreses output with 1>/dev/null
				const char suppress [] = " 1>/dev/null";
				//checks if directory exists
				int length = strlen(argv[2]) + strlen(ls) + strlen(suppress);
				char* lsCommand = malloc(length + 1);
				snprintf(lsCommand, length + 1, "ls %s 1>/dev/null", argv[2]);
				if(system(lsCommand) != 0) printError(DIR_FAIL_CODE, DIR_FAIL_MSG);
				free(lsCommand);

				//figure out how to separate it in the same file
				puts("Successfully written to DownloadTo.txt where to send audio files");

			}
		break;
		default: puts("Too many arguments");
	}
	return 0;
}
