#include "cMusicDownload.h"
#include "helpers.h"
#include "linkedList.h"
#include "writeArt.h"
#include <stdio.h>

#define UNKNOWN_ARG PNT_RED"Unknown argument of %s\n"PNT_RESET

/*
* command line arguments will be
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
				char* sendMP4 = getMP4Dest();
				char* sendMP3 = getMP3Dest();

				if(sendMP4 == NULL) printError(SKIP_VALID_CODE, SKIP_VALID_MSG);
				if(sendMP3 == NULL) printError(SKIP_VALID_CODE, SKIP_VALID_MSG);
			

				printf(PNT_GREEN"MP4 with be sent to %s and the MP3 will be send to %s\n"PNT_RESET, sendMP4, sendMP3);

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
				moveFile(song, sendMP4);
				moveFile(mp3, sendMP3);

				free(mp3);
				free(song);
				free(sendMP4);
				free(sendMP3);
				puts("Download was successful");
			}while(repeat() == 1);
		break;
		case 2:
			//lists subdirectoires in the given directory in DownloadTo.txt
			if(strcmp(argv[1], "-l") == 0){
				puts("v List of availiable directories for MP4");
				Node_t* listOfDirs = getDirectories(4);
				printList(listOfDirs);
				deleteList(&listOfDirs);

				puts("v List of availiable directories for MP3");
				listOfDirs = getDirectories(3);
				printList(listOfDirs);
				deleteList(&listOfDirs);
			}else{
				printf(UNKNOWN_ARG, argv[1]);
			}
		break;
		case 3:
			//downloading via a file
			if(strcmp(argv[1], "-f") == 0){
				FILE* inFile = fopen(argv[2], "r");
				if(inFile == NULL) printError(FILE_FAIL_CODE, FILE_FAIL_MSG);

				char* sendMP4 = getMP4Dest();
				char* sendMP3 = getMP3Dest();
				if(sendMP4 == NULL) printError(SKIP_VALID_CODE, SKIP_VALID_MSG);
				if(sendMP3 == NULL) printError(SKIP_VALID_CODE, SKIP_VALID_MSG);

				printf(PNT_GREEN"Downloading MP4s to %s and MP3s to %s\n"PNT_RESET, sendMP4, sendMP3);

				char urls [YT_URL_BUFFER] = "";
				//get URLs assumming they are separating by \n
				while(exactFileInput(inFile, urls, YT_URL_BUFFER) != 0){
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
					moveFile(song, sendMP4);
					moveFile(mp3, sendMP3);

					free(mp3);
					free(song);
					memset(urls,'\0', YT_URL_BUFFER);
				}

				fclose(inFile);
				free(sendMP4);
				free(sendMP3);
				puts("Download was successful");

			//where to send mp4(video) files
			}else if(strcmp(argv[1], "-w4") == 0){
				//check if directory exists
				if(!checkIfExists(argv[2])) printError(DIR_FAIL_CODE, DIR_FAIL_MSG);
				FILE* writeToFile = fopen(WHERE_SEND_FILES, "r");
				//File does not exist must create it
				if(writeToFile == NULL){
					writeToFile = fopen(WHERE_SEND_FILES, "w");
					if(writeToFile == NULL) printError(DOWNLOAD_CREATE_CODE, DOWNLOAD_CREATE_MSG);

					if(argv[2][strlen(argv[2]) - 1] == '/')
						fprintf(writeToFile, "4>%s\n", argv[2]);
					else
						fprintf(writeToFile, "4>%s/\n", argv[2]);
				//file exists and must check if second line exists
				}else{
					int length = 0;
					int data = '\0';
					char* line = NULL;
					if(fgetc(writeToFile) != '3'){
						while((data = fgetc(writeToFile)) != '\n' && data != EOF);

						//there is no second line
						if((data = fgetc(writeToFile)) == EOF){
							writeToFile = freopen(WHERE_SEND_FILES, "w", writeToFile);
							if(writeToFile == NULL) printError(DOWNLOAD_CREATE_CODE, DOWNLOAD_CREATE_MSG);
							if(argv[2][strlen(argv[2]) - 1] == '/')
								fprintf(writeToFile, "4>%s\n", argv[2]);
							else
								fprintf(writeToFile, "4>%s/\n", argv[2]);

						//second line is the mp3 path, copy to newly written
						}else if(data == '3'){
							while((data = fgetc(writeToFile)) != '\n' && data != EOF) ++length;
							line = malloc(length + 1);
							if(line == NULL) printError(FAILED_MALLOC_CODE, FAILED_MALLOC_MSG);
							//+1 since it begins with 3> and > is not needed
							fseek(writeToFile, -1 * length + 1,SEEK_CUR);
							fread(line, sizeof(char), length, writeToFile);
							writeToFile = freopen(WHERE_SEND_FILES, "w", writeToFile);
							if(writeToFile == NULL) printError(DOWNLOAD_CREATE_CODE, DOWNLOAD_CREATE_MSG);
							if(argv[2][strlen(argv[2]) - 1] == '/'){
								fprintf(writeToFile, "4>%s\n", argv[2]);
								fprintf(writeToFile, "3>%s", line);
							}else{
								fprintf(writeToFile, "4>%s/\n", argv[2]);
								fprintf(writeToFile, "3>%s", line);
							}
							//courrupted second line
						}else{
							printf("%c\n",data);
							printError(DOWNLOAD_CREATE_CODE, DOWNLOAD_CREATE_MSG);
						}
					//read MP3 line to copy into written file
					}else{
						while((data = fgetc(writeToFile)) != '\n' && data != EOF) ++length;
						line = malloc(length + 1);
						if(line == NULL) printError(FAILED_MALLOC_CODE, FAILED_MALLOC_MSG);
						fseek(writeToFile, -1 * length, SEEK_CUR);
						fread(line, sizeof(char), length, writeToFile);
						writeToFile = freopen(WHERE_SEND_FILES, "w", writeToFile);
						if(writeToFile == NULL) printError(DOWNLOAD_CREATE_CODE, DOWNLOAD_CREATE_MSG);
						if(argv[2][strlen(argv[2]) - 1] == '/'){
							fprintf(writeToFile, "4>%s\n", argv[2]);
							fprintf(writeToFile, "3>%s", line);
						}else{
							fprintf(writeToFile, "4>%s/\n", argv[2]);
							fprintf(writeToFile, "3>%s", line);
						}
					}
				}
				fclose(writeToFile);
				puts("Successfully written to DownloadTo.txt where to send video files");
			//where to send mp3(audio) files
			}else if(strcmp(argv[1], "-w3") == 0){
				//check if directory exists
				if(!checkIfExists(argv[2])) printError(DIR_FAIL_CODE, DIR_FAIL_MSG);
				FILE* writeToFile = fopen(WHERE_SEND_FILES, "r");
				//File does not exist must create it
				if(writeToFile == NULL){
					writeToFile = fopen(WHERE_SEND_FILES, "w");
					if(writeToFile == NULL) printError(DOWNLOAD_CREATE_CODE, DOWNLOAD_CREATE_MSG);
					if(argv[2][strlen(argv[2]) - 1] == '/')
						fprintf(writeToFile, "3>%s\n", argv[2]);
					else
						fprintf(writeToFile, "3>%s/\n", argv[2]);
				//file exists and must check if second line exists
				}else{
					int length = 0;
					int data = '\0';
					char* line = NULL;
					//find next line if it exists
					if(fgetc(writeToFile) != '4'){
						//there is no second line
						if((data = fgetc(writeToFile)) == EOF){
							writeToFile = freopen(WHERE_SEND_FILES, "w", writeToFile);
							if(writeToFile == NULL) printError(DOWNLOAD_CREATE_CODE, DOWNLOAD_CREATE_MSG);
							if(argv[2][strlen(argv[2]) - 1] == '/')
								fprintf(writeToFile, "3>%s\n", argv[2]);
							else
								fprintf(writeToFile, "3>%s/\n", argv[2]);
						//second line has the mp4 path, copy to newly written
						}else if(data == '4'){
							while((data = fgetc(writeToFile)) != '\n' && data != EOF) ++length;
							line = malloc(length + 1);
							if(line == NULL) printError(FAILED_MALLOC_CODE, FAILED_MALLOC_MSG);
							//+1 since 4> and the > is not needed
							fseek(writeToFile, -1 * length + 1, SEEK_CUR);
							fread(line, sizeof(char), length, writeToFile);
							writeToFile = freopen(WHERE_SEND_FILES, "w", writeToFile);
							if(writeToFile == NULL) printError(DOWNLOAD_CREATE_CODE, DOWNLOAD_CREATE_MSG);
							if(argv[2][strlen(argv[2]) - 1] == '/'){
								fprintf(writeToFile, "4>%s", line);
								fprintf(writeToFile, "3>%s\n", argv[2]);
							}else{
								fprintf(writeToFile, "4>%s", line);
								fprintf(writeToFile, "3>%s/\n", argv[2]);
							}
							//courrupted second line
						}else{
							printError(DOWNLOAD_CREATE_CODE, DOWNLOAD_CREATE_MSG);
						}
					//read MP4 line to copy into written file
					}else{
						while((data = fgetc(writeToFile)) != '\n' && data != EOF) ++length;
						line = malloc(length + 1);
						if(line == NULL) printError(FAILED_MALLOC_CODE, FAILED_MALLOC_MSG);
						fseek(writeToFile, -1 * length, SEEK_CUR);
						fread(line, sizeof(char), length, writeToFile);
						writeToFile = freopen(WHERE_SEND_FILES, "w", writeToFile);
						if(writeToFile == NULL) printError(DOWNLOAD_CREATE_CODE, DOWNLOAD_CREATE_MSG);
						if(argv[2][strlen(argv[2]) - 1] == '/'){
							fprintf(writeToFile, "4>%s", line);
							fprintf(writeToFile, "3>%s", argv[2]);
						}else{
							fprintf(writeToFile, "4>%s", line);
							fprintf(writeToFile, "3>%s/", argv[2]);
						}
					}
				}
				fclose(writeToFile);
				puts("Successfully written to DownloadTo.txt where to send audio files");
			}else{
				printf(UNKNOWN_ARG, argv[1]);
			}
		break;
		case 4:
			//this will write any cover art metadata to a list of mp3 files
			//specified by their path
			//note this will overwrite the original mp3s metadata
			if(strcmp(argv[1], "-ca") == 0){
				FILE* songNameFile = fopen(argv[2], "r");
				FILE* coverArt = fopen(argv[3], "r");
				if(songNameFile == NULL) printError(MP3_LIST_CODE, MP3_LIST_MSG);

				//only the name of the cover art file is needed
				//this is just to check it exists
				if(coverArt == NULL) printError(COVER_ART_CODE, COVER_ART_MSG);
				fclose(coverArt);

				writeCovers(songNameFile, argv[3]);
				fclose(songNameFile);
			}else{
				printf(UNKNOWN_ARG, argv[1]);
			}
		break;
		default: puts(PNT_RED"Too many arguments"PNT_RESET); break;
	}
	return 0;
}
