#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/stat.h>

//for making code look nicer
void printError(int code){
	switch(code){
		case 1: perror("Error MEM: Allocation of memory failed"); exit(1);
		break;
		case 2: perror("Error INF4: infinite loop detected while downloading"); exit(1);
		break;
		case 3: perror("Error FILE: Error in opening temporary file to obtain name"); exit(1);
		break;
		case 4: perror("ERROR READ: Error in reading temporary file to obtain name"); exit(1);
		break;
		case 5: perror("ERROR GVFL: Error in reading given file. File does not exist"); exit(1);
		break;
		case 6: perror("ERROR GVDR: Error in reading given directory. Directory does not exist"); exit(1);
		break;
		case 7: perror("ERROR CNVT: Error in converting from .mp4 to .mp3"); exit(1);
		break;
		case 8: perror("ERROR MVP4: Error in moving video file to desired directory"); exit(1);
		break;
		case 9: perror("ERROR MVP3: Error in moving .mp3 file to /Unsynced directory"); exit(1);
		break;
		case 10: perror("ERROR INVD: Error in user skipping validation of music directories"); exit(1);
		default: perror("An error has occured"); exit(1);
	}
}

//asks user for a youtube URL more of a helper method
char * getURL(void){
	//longest URL I've found went up to 98
	char buffer [101] = "";
	do{
		printf("Enter the youtube URL that you want to download -> ");
		fgets(buffer, 100, stdin);
	}while(strlen(buffer) == 0 && strrchr(buffer, '\n') == NULL);
	//dynamic memory since this will be passed around
	char* youtubeURL = malloc(sizeof(buffer));
	snprintf(youtubeURL, sizeof(buffer), "%s", buffer);
	return youtubeURL;
}

//downloads a song given the URL for it
void downloadURL(char* youtubeURL){
	//--restrict-filenames makes it so escape characters don't need to be added
	char downloadCommand [140]= "";
	if(strstr(youtubeURL, "&list") == NULL)
		snprintf(downloadCommand, strlen(youtubeURL) + 33, "youtube-dl --restrict-filenames %s", youtubeURL);
	else
		snprintf(downloadCommand, strlen(youtubeURL) + 33, "youtube-dl --restrict-filenames %s", strstr(youtubeURL, "&list"));
	
	int retry = 0;
	while(system(downloadCommand) > 0 && retry <= 4){
		puts("Retrying download");
		++retry;
		if(retry >= 4)
			printError(2);
		
	}
}

//helper method for getting the ID
//the Youtube ID is 11 characters, but it's dynamic in case that changes
char* getID(char* youtubeURL){
	char* id = NULL;
	int idSize = 0;
	
	//finds video parameter
	char* vPara = strstr(youtubeURL, "?v="); 
	idSize = strrchr(youtubeURL, '\0') - (vPara + 3) - 1;

	id = malloc(idSize + 1);
	if(id == NULL)
		printError(1);

	snprintf(id, idSize + 1, "%s", vPara + 3);
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
		songName = realloc(songName, size + 1);			
		if(songName == NULL)
			printError(1);
	
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
			
	snprintf(moveMP4, strlen(songName) + strlen(musicDirectory) + 9, "mv %s VLC/%s",songName, musicDirectory);
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


int repeat(void){
	char yesNo [4] = "";
	do{
		printf("Do you want to download another song? Y/N: ");
		fgets(yesNo, 3, stdin);
		if(strlen(yesNo) > 1)
			yesNo[0] = tolower(yesNo[0]);
			
	}while(strlen(yesNo) > 0 && yesNo[0] != 'y' && yesNo[0] != 'n');
	
	if(yesNo[0] == 'y')
		return 0;
	else
		return 1;
}

//testing with directories
char* findDirectory(void){
	//TODO 
	return("HE HE HE HA");
}

//forces the input of a valid directory to be returned
char* getDirectory(void){
	struct stat knowWhere;
	if(stat("DownloadTo.txt", &knowWhere)){
		puts("DownloadTo.txt does not exist. Please use the -w flag to specify the directory to download to.");
		exit(1);
	}else if(!knowWhere.st_size){
		puts("DownloadTo.txt is empty. Please use the -w flag to specify the directory to download to.");	
		exit(1);
	}
	DIR* scan;
	//if(scan = opendir(<-w Flag thing>) == NULL)
		printError(6);

	closedir(scan);
	//STOP HERE WORK ON READING FIRST
	int valid = 1;
	while(valid == 1){
		char directory [26] = "";
		do{
			printf("Where do you want to download the song? or type exit: ");
			fgets(directory, 25, stdin);
		}while(strlen(directory) == 0 && strrchr(directory, '\n') == NULL);
		
		//finds the \n at the end and replaces it with \0
		directory[strcspn(directory, "\n")] = '\0';
		//puts to lowercase to make it easier for input and validating
		int i = 0;
		for(;i < strlen(directory); ++i)
			directory[i] = tolower(directory[i]);
	
		//checks the available directories
		if(strcmp(directory, "electro") == 0)
			return "Electro";
				
		if(strcmp(directory, "hardblin") == 0)
			return "HardBlin";
			
		if(strcmp(directory, "mincecraft") == 0)
			return "MinceCraft";
			
		if(strcmp(directory, "mr.intense") == 0)
			return "Mr.Intense";
				
		if(strcmp(directory, "ohyes") == 0)
			return "OhYes";
				
		if(strcmp(directory, "phonkdeeznuts") == 0)
			return "PhonkDeezNuts";
				
		if(strcmp(directory, "teamfortress2") == 0)
			return "TeamFortress2";
				
		if(strcmp(directory, "troll") == 0)
			return "Troll";
			
		if(strcmp(directory, "test") == 0)
			return "Test";
				
		if(strcmp(directory, "exit") == 0)
			exit(0);
			
		printf("Invalid directory of %s retry entry\n", directory);		
	}
	//in case it some how skips the whole loop
	return "SKIPPED";
}

/*NEED TO PLACE COMMAND LINE ARGUMENTS LATER
*command line arguments will be
* -l for listing valid directories
* -f to specify downloading a file
* -p for prompting each time <perhaps>
* -w for writing into directory.txt file
*/
int main(int argc, char** argv){
	switch(argc){
		case 1:
			do{
				char* where = getDirectory();
				if(strcmp(where, "SKIPPED") == 0)
					printError(9);
					
				//downloadSong(where);
				printf("You have desired to download to VLC/%s\n", where);
				char* url = getURL();
				downloadURL(url);
				char* youtubeID = getID(url);
				free(url);
				char* song = getSongName(youtubeID);
				free(youtubeID);
				convertMove(song, where);
				free(song);
				puts("Download was successful");
			}while(repeat() == 0);
		break;
		case 2:
			if(strcmp(argv[1], "-l") == 0){
				char buffer [101] = "";
				FILE* downloadTo = fopen("DownloadTo.txt", "r");
				fgets(buffer, 101, downloadTo);
				DIR* openDir = opendir(buffer);
				if(openDir == NULL)
					printError(6);
			
				fclose(downloadTo);
				closedir(openDir);
			
				printf("%s\n", buffer);
				exit(0);
			}
		break;
		case 3:		
			if(strcmp(argv[1], "-f") == 0){
				FILE* inFile = fopen(argv[2], "r");
				if(inFile == NULL)
					printError(5);
					
				char* where = getDirectory();
				if(strcmp(where, "SKIPPED") == 0)
					printError(9);
					
				//fileDownload(inFile, where);
				printf("Downloading URLS to VLC/%s\n", where);
		
				char urls [101] = "";
				while(fgets(urls, 100, inFile) != NULL){
					urls[strcspn(urls, "\n")] = '\0';
					downloadURL(urls);
					char* youtubeID = getID(urls);
					char* song = getSongName(youtubeID);
					free(youtubeID);
					convertMove(song, where);
					free(song);
				}
				puts("Download was successful");
			}

			if(strcmp(argv[1], "-w") == 0){
				//checks if directory exists
				DIR* exists = opendir(argv[2]);
				if(exists == NULL)
					printError(6);
				
				closedir(exists);
				FILE* writeToFile = fopen("DownloadTo.txt", "w");
				if(argv[2][strlen(argv[2]) - 1] == '/')
					fputs(argv[2], writeToFile);
				else{
					char* withSlash = malloc(strlen(argv[2]) + 2);
					snprintf(withSlash, strlen(argv[2]) + 2, "%s%c", argv[2], '/');
					fputs(withSlash, writeToFile);
					free(withSlash);
				}
				fclose(writeToFile);
				puts("Successfully written to DownloadTo.txt");
			}
		break;
		default: puts("I dunno haven't gotten this far");
	}
	return 0;
}
