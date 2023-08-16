#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "cMusicDownload.h"
#include "helpers.h"
#include "linkedList.h"
#include "writeArt.h"

#define UNKNOWN_ARG "Unknown argument of %s\n"
#define COVER_ART_NOT_GIVEN "Did not specify a file for cover art"
#define FILE_NOT_GIVEN "Did not specify a file to use"
#define W4_NOT_GIVEN "Did not provide the path to download video files to"
#define W3_NOT_GIVEN "Did not provide the path to download audio files to"
#define NO_PERMISSION "Writting privleges for the given path is not availiable"
#define NO_NAME "Could not obtain the video name from grepped file"
#define NO_ART "Could not obtain thumbnail from grepped file"

#define USER_MP4_PROMPT "Where do you want to send the MP4? Type exit to leave or type skip to avoid sending files: "
#define USER_MP3_PROMPT "Where do you want to send the MP3? Type exit to leave or type skip to avoid sending files: "
#define USER_MP3_PROMPT_NO_SKIP "Where do you want to send the MP3? Type exit to leave: "
#define USER_MP4_PROMPT_NO_SKIP "Where do you want to send the MP4? Type exit to leave: "


//helper method to avoid double skips
void askUserForPaths(char** sendMP4, char** sendMP3){
	//the value can go directly into the passed pointers but
	//to be a little safer local variables are used
	char* mp4Dest = getDests(4, USER_MP4_PROMPT);
	char* mp3Dest = getDests(3, USER_MP3_PROMPT);
	if(mp4Dest == NULL) printError(EXIT_FAILURE, SKIP_VALID_MSG);
	if(mp3Dest == NULL) printError(EXIT_FAILURE, SKIP_VALID_MSG);

	//ensure both are not SKIP
	while(!strcmp(mp4Dest, "SKIP") && !strcmp(mp3Dest, "SKIP")){
		puts(PNT_RED"Both paths can not be skip!"PNT_RESET);
		free(mp4Dest);
		mp4Dest = NULL;
		free(mp3Dest);
		mp3Dest = NULL;
		mp4Dest = getDests(4, USER_MP4_PROMPT);
		mp3Dest = getDests(3, USER_MP3_PROMPT);
		if(mp4Dest == NULL) printError(EXIT_FAILURE, SKIP_VALID_MSG);
		if(mp3Dest == NULL) printError(EXIT_FAILURE, SKIP_VALID_MSG);
	}

	*sendMP4 = malloc(strlen(mp4Dest) + 1);
	*sendMP3 = malloc(strlen(mp3Dest) + 1);
	snprintf(*sendMP4, strlen(mp4Dest) + 1, "%s", mp4Dest);
	snprintf(*sendMP3, strlen(mp3Dest) + 1, "%s", mp3Dest);
	free(mp4Dest);
	free(mp3Dest);
}

//helper method to download both MP3 and MP4 from a url
//if mode 0 is passed coverArtName is not needed
void downloadMp3AndMp4(char* url, int mode, char** mp4Name, char** mp3Name, char** coverArtName){
	//download song via URL
	downloadURL(url, mode);

	//get the ID for later movement
	char* youtubeID = getID(url);

	//get ID to obtain the name
	grepIntoFile(youtubeID);
	free(youtubeID);
	youtubeID = NULL;

	//read file
	FILE* greppedFile = fopen("GrepTemp.txt", "r");
	if(mode == 0){
		if(unkownFileRead(greppedFile, mp4Name) == 0)
			printError(EXIT_FAILURE, NO_NAME);

	}else{
		if(unkownFileRead(greppedFile, coverArtName) == 0)
			printError(EXIT_FAILURE, NO_ART);
		if(unkownFileRead(greppedFile, mp4Name) == 0)
			printError(EXIT_FAILURE, NO_NAME);
	}

	fclose(greppedFile);
	system("rm GrepTemp.txt");

	//get mp3
	*mp3Name = convertToMp3(*mp4Name);
}

//helper method to just download MP3
//if mode 0 is passed coverArtName is not needed
void downloadMP3(char* url, int mode, char** mp3Song, char** coverArtName){
	//download song via URL
	downloadURL(url, mode);

	//get the ID for later movement
	char* youtubeID = getID(url);

	//get song name via youtube ID
	grepIntoFile(youtubeID);
	free(youtubeID);
	youtubeID = NULL;

	FILE* greppedFile = fopen("GrepTemp.txt", "r");
	char* mp4 = NULL;

	if(mode == 0){
		if(unkownFileRead(greppedFile, &mp4) == 0)
			printError(EXIT_FAILURE, NO_NAME);

	}else{
		if(unkownFileRead(greppedFile, coverArtName) == 0)
			printError(EXIT_FAILURE, NO_ART);
		if(unkownFileRead(greppedFile, &mp4) == 0)
			printError(EXIT_FAILURE, NO_NAME);
	}
	fclose(greppedFile);
	system("rm GrepTemp.txt");

	*mp3Song = convertToMp3(mp4);

	//remove mp4 file
	char* removeCommand = malloc(3 + strlen(mp4) + 1);
	snprintf(removeCommand, 3 + strlen(mp4) + 1, "%s%s", "rm ", mp4);
	free(removeCommand);
	removeCommand = NULL;
	free(mp4);
	mp4 = NULL;
}

//helper method to just download a MP4
char* downloadMP4(char* url){
	//download URL
	//0 is passed since no cover art is needed
	downloadURL(url, 0);

	//get ID for movement
	char* youtubeID = getID(url);
	//free(url);
	//url = NULL;

	//obtain the song name
	char* youtubeTitle = getSongName(youtubeID);
	free(youtubeID);
	youtubeID = NULL;

	return youtubeTitle;
}

//helper method to remove cover arts
void removeCoverArt(char* artName){
	char* removeArt = malloc(3 + strlen(artName) + 1);
	snprintf(removeArt, 3 + strlen(artName) + 1, "%s%s", "rm ", artName);
	system(removeArt);
	free(removeArt);
	removeArt = NULL;
}

//helper method to write the proper format for DownloadTo.txt
void writeFormat(FILE* stream, char* string, int mode){
	switch(mode){
		case 3:
			if(string[strlen(string) - 1] == '/')
				fprintf(stream, "3>%s\n", string);
			else
				fprintf(stream, "3>%s/\n", string);
		break;
		case 4:
			printf("placing %s\n",string);
			if(string[strlen(string) - 1] == '/')
				fprintf(stream, "4>%s\n", string);
			else
				fprintf(stream, "4>%s/\n", string);
		break;
		default: printError(EXIT_FAILURE, "Client passed unknown mode for writing to DownloadTo.txt");
		break;
	}
}

int main(int argc, char** argv){
	//modes for different execution
	int fileMode = 0;
	int fileFlagIndex = 0;
	int coverArtMode = 0;
	int noArtMode = 0;
	int coverArtIndex = 0;

	if(argc > 1){
		int c = 1;
		for(; c < argc; ++c){
			if(strcmp("-l", argv[c]) == 0){
				puts("v List of availiable directories for MP4");
				Node_t* listOfDirs = getDirectories(4);
				printList(listOfDirs);
				deleteList(&listOfDirs);

				puts("v List of availiable directories for MP3");
				listOfDirs = getDirectories(3);
				printList(listOfDirs);
				deleteList(&listOfDirs);
			}else if(strcmp("-f", argv[c]) == 0){
				if(argc - 1 == c)
					printError(EXIT_FAILURE, FILE_NOT_GIVEN);

				if(!checkIfExists(argv[c + 1])) printError(EXIT_FAILURE, FILE_FAIL_MSG);

				fileMode = 1;
				fileFlagIndex = ++c;

			//NEED TO DO MORE USER VALIDATION
			}else if(strcmp("-w4", argv[c]) == 0){
				if(argc - 1 == c)
					printError(EXIT_FAILURE, W4_NOT_GIVEN);

				//check if directory exists
				if(!checkIfExists(argv[++c])) printError(EXIT_FAILURE, DIR_FAIL_MSG);

				//check if given writting privleges
				if(access(argv[c], W_OK) == -1)
					printError(EXIT_FAILURE, NO_PERMISSION);

				FILE* writeToFile = NULL;
				//DownloadTo.txt does not exist must create it
				if(!checkIfExists(WHERE_SEND_FILES)){
					writeToFile = fopen(WHERE_SEND_FILES, "w");
					if(writeToFile == NULL) printError(EXIT_FAILURE, DOWNLOAD_CREATE_MSG);
					writeFormat(writeToFile, argv[c], 4);

				//file exists and must check if second line exists
				}else{
					int data = '\0';
					writeToFile = fopen(WHERE_SEND_FILES, "r");
					FILE* tempFile = fopen("DownloadToTemp.txt", "w");
					if(tempFile == NULL) printError(EXIT_FAILURE, TEMP_FILE_FAIL_MSG);

					//trying to overwrite mp4 files path want to find mp3 path
					if(fgetc(writeToFile) != '3'){
						//skip to second line
						while((data = fgetc(writeToFile)) != '\n' && data != EOF);

						//there is no second line
						if((data = fgetc(writeToFile)) == EOF){
							writeFormat(tempFile, argv[c], 4);

						//second line is the mp3 path, copy to temp
						}else if(data == '3'){
							fputc('3', tempFile);
							while((data = fgetc(writeToFile)) != '\n' && data != EOF)
								fputc(data, tempFile);

							fputc('\n', tempFile);
							writeFormat(tempFile, argv[c], 4);

							//courrupted second line
						}else{
							printError(EXIT_FAILURE, DOWNLOAD_READ_MSG);
						}

						//read MP3 line to copy into written file
					}else{
						fputc('3', tempFile);
						while((data = fgetc(writeToFile)) != '\n' && data != EOF)
							fputc(data, tempFile);

						fputc('\n', tempFile);
						writeFormat(tempFile, argv[c], 4);
					}
					fclose(tempFile);
					moveFile("DownloadToTemp.txt", WHERE_SEND_FILES);
				}
				fclose(writeToFile);
				puts("Successfully written to DownloadTo.txt where to send video files");
			}

			//where to send mp3(audio) files
			//NEED TO DO MORE USER VALIDATION
			else if(strcmp("-w3", argv[c]) == 0){
				if(argc - 1 == c) printError(EXIT_FAILURE, W3_NOT_GIVEN);

				//check if directory exists
				if(!checkIfExists(argv[++c])) printError(EXIT_FAILURE, DIR_FAIL_MSG);

				//check if given writting privleges
				if(access(argv[c], W_OK) == -1)
					printError(EXIT_FAILURE, NO_PERMISSION);


				FILE* writeToFile = NULL;

				//File does not exist must create it
				if(!checkIfExists(WHERE_SEND_FILES)){
					writeToFile = fopen(WHERE_SEND_FILES, "w");
					if(writeToFile == NULL) printError(EXIT_FAILURE, DOWNLOAD_CREATE_MSG);
					writeFormat(writeToFile, argv[c], 3);

				//file exists and must check if second line exists
				}else{
					int data = '\0';
					writeToFile = fopen(WHERE_SEND_FILES, "r");
					FILE* tempFile = fopen("DownloadToTemp.txt", "w");
					if(tempFile == NULL) printError(EXIT_FAILURE, TEMP_FILE_FAIL_MSG);

					//find next line if it exists
					if(fgetc(writeToFile) != '4'){
						//skip to second line
						while((data = fgetc(writeToFile)) != '\n' && data != EOF);

						//there is no second line
						if((data = fgetc(writeToFile)) == EOF){
							if(writeToFile == NULL) printError(EXIT_FAILURE, DOWNLOAD_CREATE_MSG);
							writeFormat(tempFile, argv[c], 3);

						//second line has the mp4 path, copy to newly written
						}else if(data == '4'){
							fputc('4', tempFile);
							while((data = fgetc(writeToFile)) != '\n' && data != EOF)
								fputc(data, tempFile);

							fputc('\n', tempFile);
							writeFormat(tempFile, argv[c], 3);
						}else{
							printError(EXIT_FAILURE, DOWNLOAD_READ_MSG);
						}

						//read MP4 line to copy into written file
					}else{
						fputc('4', tempFile);
						while((data = fgetc(writeToFile)) != '\n' && data != EOF)
							fputc(data, tempFile);

						fputc('\n', tempFile);
						writeFormat(tempFile, argv[c], 3);
					}
					fclose(tempFile);
					moveFile("DownloadToTemp.txt", WHERE_SEND_FILES);
				}
				fclose(writeToFile);
				puts("Successfully written to DownloadTo.txt where to send audio files");

			}else if(strcmp("-ca", argv[c]) == 0){
				if(argc - 1 == c) printError(EXIT_FAILURE, COVER_ART_NOT_GIVEN);

				if(strcmp("NO-ART", argv[c + 1]) == 0){
					noArtMode = 1;
					coverArtMode = 1;
				}else{
					if(!checkIfExists(argv[c + 1]))
						printError(EXIT_FAILURE, FILE_FAIL_MSG);

					coverArtMode = 1;
				}

				coverArtIndex = ++c;
			}else{
				printError(EXIT_FAILURE, UNKNOWN_ARG);
			}
		}
	}else{
		//default execution
		do{
			//ask for directory
			char* sendMP4 = NULL;
			char* sendMP3 = NULL;
			askUserForPaths(&sendMP4, &sendMP3);

			char* url = NULL;
			char* mp3 = NULL;
			char* mp4 = NULL;
			char* coverArt = NULL;
			//if wanting to send to MP4 path
			if(strcmp(sendMP4, "SKIP")){
				//if wanting to send to MP3 path
				if(strcmp(sendMP3, "SKIP")){
					printf(PNT_GREEN"MP4 will be sent to %s and MP3 will be sent to %s\n"PNT_RESET, sendMP4, sendMP3);

					url = getURL();
					downloadMp3AndMp4(url, 1, &mp4, &mp3, &coverArt);
					writeCover(mp3, coverArt);
					removeCoverArt(coverArt);
					moveFile(mp4, sendMP4);
					moveFile(mp3, sendMP3);

					free(mp4);
					mp4 = NULL;
					free(mp3);
					mp3 = NULL;
					free(coverArt);
					coverArt = NULL;
				}else{
					printf(PNT_GREEN"MP4 will be sent to %s\n"PNT_RESET, sendMP4);

					url = getURL();
					mp4 = downloadMP4(url);
					moveFile(mp4, sendMP4);
					free(mp4);
					mp4 = NULL;
				}
			}else{
				printf(PNT_GREEN"MP3 will be sent to %s\n"PNT_RESET, sendMP3);

				url = getURL();
				downloadMP3(url, 1, &mp3, &coverArt);
				writeCover(mp3, coverArt);
				removeCoverArt(coverArt);
				moveFile(mp3, sendMP3);

				free(mp3);
				mp3 = NULL;
				free(coverArt);
				coverArt = NULL;
			}

			free(url);
			url = NULL;
			free(sendMP4);
			sendMP4 = NULL;
			free(sendMP3);
			sendMP3 = NULL;
		}while(repeat() == 1);
	}
	//finished parsing arguments

	//check if both dynamic modes are desired
	//this will download youtubeURLS and add .mp3 cover arts if wanted
	//This will also accept a file that contains both youtube URLS and .mp3 paths
	if(fileMode && coverArtMode){
		char* buffer = NULL;
		FILE* inFile = fopen(argv[fileFlagIndex], "r");
		if(inFile == NULL) printError(EXIT_FAILURE, FILE_FAIL_MSG);

		//ask for directory
		puts(PNT_RED"NOTE: for this combination of options you can only skip MP4 paths"PNT_RESET);
		char* sendMP4 = getDests(4, USER_MP4_PROMPT);
		puts(PNT_RED"NOTE: Can't skip MP3 path since cover art option was passed"PNT_RESET);
		char* sendMP3 = getDestsNoSkip(3, USER_MP3_PROMPT_NO_SKIP);

		if(sendMP4 == NULL) printError(EXIT_FAILURE, SKIP_VALID_MSG);
		if(sendMP3 == NULL) printError(EXIT_FAILURE, SKIP_VALID_MSG);

		char* mp4 = NULL;
		char* mp3 = NULL;
		//user does not want art
		if(noArtMode){
			//user wants to move MP4s
			if(strcmp(sendMP4, "SKIP")){
				printf(PNT_GREEN"MP4 will be sent to %s and MP3 with no art will be sent to %s\n"PNT_RESET, sendMP4, sendMP3);
				while(unkownFileRead(inFile, &buffer) != 0){
					if(strstr(buffer, YOUTUBE_URL) != NULL){
						//have to shorten URL so yt-dlp does not return
						//early from parsing through a playlist
						char shortenedURL [YT_URL_BUFFER] = "";

						//snprintf was complaining about truncation and
						//I didn't want compliation warnings
						strncpy(shortenedURL, buffer, YT_URL_BUFFER);
						shortenedURL[YT_URL_BUFFER - 1] = '\0';

						sleep(1);
						downloadMp3AndMp4(shortenedURL, 0, &mp4, &mp3, NULL);
						moveFile(mp4, sendMP4);
						moveFile(mp3, sendMP3);

						free(mp4);
						mp4 = NULL;
						free(mp3);
						mp3 = NULL;

					}else if(strstr(buffer, ".mp3") != NULL){
						char* quoted = surroundInQuotes(buffer);
						if(checkIfExists(quoted))
							moveFile(quoted, sendMP3);
						else
							printf(PNT_RED"can't find .mp3 %s via its path\n"PNT_RESET, buffer);

						free(quoted);
						quoted = NULL;
					}else{
						printf(PNT_RED"Line obtianed is not a youtube URL or .MP3-> %s\n"PNT_RESET, buffer);
					}
				}//end of while

			//user just wants MP3s
			}else{
				printf(PNT_GREEN"MP3 with no art will be sent to %s\n"PNT_RESET, sendMP3);
				while(unkownFileRead(inFile, &buffer) != 0){
					//if line read is a mp3 path
					if(strstr(buffer, ".mp3") != NULL){
						char* quoted = surroundInQuotes(buffer);
						if(checkIfExists(quoted))
							moveFile(quoted, sendMP3);
						else
							printf(PNT_RED"can't find .mp3 %s via its path\n"PNT_RESET, buffer);
						
						free(quoted);
						quoted = NULL;
					}else{
						printf(PNT_RED"Line obtianed is not a .MP3-> %s\n"PNT_RESET, buffer);
					}
				}//end of while
			}

		//user provided their art
		}else{
			//user wants to move MP4s
			if(strcmp(sendMP4, "SKIP")){
				printf(PNT_GREEN"MP4 will be sent to %s and MP3 with specified art will be sent to %s\n"PNT_RESET, sendMP4, sendMP3);

				while(unkownFileRead(inFile, &buffer) != 0){
					if(strstr(buffer, YOUTUBE_URL) != NULL){
						//have to shorten URL so yt-dlp does not return
						//early from parsing through a playlist
						char shortenedURL [YT_URL_BUFFER] = "";

						//snprintf was complaining about truncation and
						//I didn't want compliation warnings
						strncpy(shortenedURL, buffer, YT_URL_BUFFER);
						shortenedURL[YT_URL_BUFFER - 1] = '\0';

						sleep(1);
						downloadMp3AndMp4(shortenedURL, 0, &mp4, &mp3, NULL);
						writeCover(mp3, argv[coverArtIndex]);
						moveFile(mp4, sendMP4);
						moveFile(mp3, sendMP3);

						free(mp4);
						mp4 = NULL;
						free(mp3);
						mp3 = NULL;

					}else if(strstr(buffer, ".mp3") != NULL){
						char* quoted = surroundInQuotes(buffer);
						if(checkIfExists(quoted)){
							writeCover(buffer, argv[coverArtIndex]);
							moveFile(quoted, sendMP3);
						}else{
							printf(PNT_RED"can't find .mp3 %s via its path\n"PNT_RESET, buffer);
						}

						free(quoted);
						quoted = NULL;
					}else{
						printf(PNT_RED"Line obtianed is not a youtube URL or .MP3-> %s\n"PNT_RESET, buffer);
					}
				}//end of while

			//user just wants MP3s
			}else{
				printf(PNT_GREEN"MP3 with specified art will be sent to %s\n"PNT_RESET, sendMP3);

				while(unkownFileRead(inFile, &buffer) != 0){
					if(strstr(buffer, ".mp3") != NULL){
						char* quoted = surroundInQuotes(buffer);
						if(checkIfExists(quoted)){
							writeCover(buffer, argv[coverArtIndex]);
							moveFile(quoted, sendMP3);
						}else
							printf(PNT_RED"can't find .mp3 %s via its path\n"PNT_RESET, buffer);

						free(quoted);
						quoted = NULL;
					}else{
						printf(PNT_RED"Line obtianed is not a .MP3-> %s\n"PNT_RESET, buffer);
					}
				}//end of while
			}
		}

		free(sendMP4);
		sendMP4 = NULL;
		free(sendMP3);
		sendMP3 = NULL;

	//specified art or no art wants to be added
	}else if(coverArtMode){

		//user wants no cover art
		if(noArtMode){
			do{
				//ask for directory
				char* sendMP4 = getDests(4, USER_MP4_PROMPT);
				puts(PNT_RED"Note for this option there is no skipping. You are wanting to download a .mp3 file."PNT_RESET);
				char* sendMP3 = getDestsNoSkip(3, USER_MP3_PROMPT_NO_SKIP);

				if(sendMP4 == NULL) printError(EXIT_FAILURE, SKIP_VALID_MSG);
				if(sendMP3 == NULL) printError(EXIT_FAILURE, SKIP_VALID_MSG);

				char* url = NULL;
				char* mp3 = NULL;
				char* mp4 = NULL;
				//if wanting to send to MP4 path
				if(strcmp(sendMP4, "SKIP")){
					printf(PNT_GREEN"MP4 will be sent to %s and MP3 with no art will be sent to %s\n"PNT_RESET, sendMP4, sendMP3);
					url = getURL();
					mp4 = downloadMP4(url);
					mp3 = convertToMp3(mp4);
					moveFile(mp4, sendMP4);
					moveFile(mp3, sendMP3);

					free(mp4);
					mp4 = NULL;
					free(mp3);
					mp3 = NULL;
				}else{
					printf(PNT_GREEN"MP3 with no art will be sent to %s\n"PNT_RESET, sendMP3);

					url = getURL();
					downloadMP3(url, 0, &mp3, NULL);
					moveFile(mp3, sendMP3);

					free(mp3);
					mp3 = NULL;
				}

				free(url);
				url = NULL;
				free(sendMP4);
				sendMP4 = NULL;
				free(sendMP3);
				sendMP3 = NULL;
			}while(repeat() == 1);

		//when a cover art is specified
		}else{
			puts(PNT_RED"NOTE: this will not ask to reset the cover art if you decide to download more!"PNT_RESET);
			do{
				//ask for directory
				char* sendMP4 = getDests(4, USER_MP4_PROMPT);
				puts(PNT_RED"NOTE: for this option there is no skipping MP3 paths since cover art option is selected."PNT_RESET);
				char* sendMP3 = getDestsNoSkip(3, USER_MP3_PROMPT_NO_SKIP);

				if(sendMP4 == NULL) printError(EXIT_FAILURE, SKIP_VALID_MSG);
				if(sendMP3 == NULL) printError(EXIT_FAILURE, SKIP_VALID_MSG);

				char* url = NULL;
				char* mp3 = NULL;
				char* mp4 = NULL;
				char* coverArt = NULL;
				//if wanting to send to MP4 path
				if(strcmp(sendMP4, "SKIP")){
					//if wanting to send to MP3 path
					printf(PNT_GREEN"MP4 will be sent to %s and MP3 with specified art will be sent to %s\n"PNT_RESET, sendMP4, sendMP3);

					url = getURL();
					downloadMp3AndMp4(url, 0, &mp4, &mp3, NULL);
					writeCover(mp3, argv[coverArtIndex]);
					moveFile(mp4, sendMP4);
					moveFile(mp3, sendMP3);

					free(mp4);
					mp4 = NULL;
					free(mp3);
					mp3 = NULL;

				}else{
					printf(PNT_GREEN"MP3 will be sent to %s\n"PNT_RESET, sendMP3);

					url = getURL();
					downloadMP3(url, 0, &mp3, NULL);
					writeCover(mp3, argv[coverArtIndex]);
					moveFile(mp3, sendMP3);

					free(mp3);
					mp3 = NULL;
					free(coverArt);
					coverArt = NULL;
				}

				free(url);
				url = NULL;
				free(sendMP4);
				sendMP4 = NULL;
				free(sendMP3);
				sendMP3 = NULL;

			}while(repeat() == 1);
		}

	//basically default execution with the addition of a file
	}else if(fileMode){
		FILE* inFile = fopen(argv[fileFlagIndex], "r");
		if(inFile == NULL) printError(EXIT_FAILURE, FILE_FAIL_MSG);

		//ask for directory
		char* sendMP4 = NULL;
		char* sendMP3 = NULL;
		askUserForPaths(&sendMP4, &sendMP3);

		char urlBuffer [YT_URL_BUFFER] = "";
		char* mp3 = NULL;
		char* mp4 = NULL;
		char* coverArt = NULL;

		//if wanting to send to MP4 path
		if(strcmp(sendMP4, "SKIP")){
			//if wanting to send to MP3 path
			if(strcmp(sendMP3, "SKIP")){
				printf(PNT_GREEN"MP4 will be sent to %s and MP3 will be sent to %s\n"PNT_RESET, sendMP4, sendMP3);
				while(exactFileInput(inFile, urlBuffer, YT_URL_BUFFER) != 0){
					downloadMp3AndMp4(urlBuffer, 1, &mp4, &mp3, &coverArt);
					writeCover(mp3, coverArt);
					removeCoverArt(coverArt);
					moveFile(mp4, sendMP4);
					moveFile(mp3, sendMP3);

					free(mp4);
					mp4 = NULL;
					free(mp3);
					mp3 = NULL;
					free(coverArt);
					coverArt = NULL;
				}

			//just wanting MP4 download
			}else{
				printf(PNT_GREEN"MP4 will be sent to %s\n"PNT_RESET, sendMP4);
				while(exactFileInput(inFile, urlBuffer, YT_URL_BUFFER) != 0){
					mp4 = downloadMP4(urlBuffer);
					moveFile(mp4, sendMP4);
					free(mp4);
					mp4 = NULL;
				}
			}

		//just wanting MP3 download
		}else{
			printf(PNT_GREEN"MP3 will be sent to %s\n"PNT_RESET, sendMP3);
			while(exactFileInput(inFile, urlBuffer, YT_URL_BUFFER) != 0){
				downloadMP3(urlBuffer, 1, &mp3, &coverArt);
				writeCover(mp3, coverArt);
				removeCoverArt(coverArt);
				moveFile(mp3, sendMP3);

				free(mp3);
				mp3 = NULL;
				free(coverArt);
				coverArt = NULL;
			}
		}

		memset(urlBuffer,'\0', YT_URL_BUFFER);
		free(sendMP4);
		sendMP4 = NULL;
		free(sendMP3);
		sendMP3 = NULL;
	}

	return 0;
}
