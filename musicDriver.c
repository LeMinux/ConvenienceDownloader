#include <stdio.h>

#include "cMusicDownload.h"
#include "helpers.h"
#include "linkedList.h"
#include "writeArt.h"

#define UNKNOWN_ARG PNT_RED"Unknown argument of %s\n"PNT_RESET
#define COVER_ART_NOT_GIVEN PNT_RED"Did not specify a file for Cover art"PNT_RESET
#define FILE_NOT_GIVEN PNT_RED"Did not specify a file to use"PNT_RESET
#define W4_NOT_GIVEN PNT_RED"Did not provide the path to download video files to"PNT_RESET
#define W3_NOT_GIVEN PNT_RED"Did not provide the path to download audio files to"PNT_RESET
#define MP3_ERROR PNT_RED"File must contain .MP3 files for writing cover art to"PNT_RESET

int main(int argc, char** argv){
	int fileMode = 0;
	int fileFlagIndex = 0;
	int coverArtMode = 0;
	int coverArtDefaultMode = 0;
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
				if(argc - 1 == c){
					printf(FILE_NOT_GIVEN);
					exit(1);
				}

				if(!checkIfExists(argv[c + 1], 'f')) printError(FILE_FAIL_CODE, FILE_FAIL_MSG);

				fileMode = 1;
				fileFlagIndex = ++c;

			//NEED TO DO MORE USER VALIDATION
			}else if(strcmp("-w4", argv[c]) == 0){
				if(argc - 1 == c){
					printf(W4_NOT_GIVEN);
					exit(1);
				}

				//check if directory exists
				if(!checkIfExists(argv[++c], 'd')) printError(DIR_FAIL_CODE, DIR_FAIL_MSG);

				FILE* writeToFile = NULL;
				//DownloadTo.txt does not exist must create it
				if(!checkIfExists(WHERE_SEND_FILES, 'f')){
					writeToFile = fopen(WHERE_SEND_FILES, "w");
					if(writeToFile == NULL) printError(DOWNLOAD_CREATE_CODE, DOWNLOAD_CREATE_MSG);

					if(argv[c][strlen(argv[c]) - 1] == '/')
						fprintf(writeToFile, "4>%s\n", argv[c]);
					else
						fprintf(writeToFile, "4>%s/\n", argv[c]);
				//file exists and must check if second line exists
				}else{
					//int length = 0;
					int data = '\0';
					//char* line = NULL;
					writeToFile = fopen(WHERE_SEND_FILES, "r");
					//trying to overwrite mp4 files path want to find mp3 path
					if(fgetc(writeToFile) != '3'){
						//skip to second line
						while((data = fgetc(writeToFile)) != '\n' && data != EOF);

						//there is no second line
						if((data = fgetc(writeToFile)) == EOF){
							writeToFile = freopen(WHERE_SEND_FILES, "w", writeToFile);
							if(writeToFile == NULL) printError(DOWNLOAD_CREATE_CODE, DOWNLOAD_CREATE_MSG);
							if(argv[c][strlen(argv[c]) - 1] == '/')
								fprintf(writeToFile, "4>%s\n", argv[c]);
							else
								fprintf(writeToFile, "4>%s/\n", argv[c]);

							//second line is the mp3 path, copy to newly written
						}else if(data == '3'){
							FILE* tempFile = fopen("DownloadToTemp.txt", "w");
							if(tempFile == NULL) printError(TEMP_FILE_FAIL_CODE, TEMP_FILE_FAIL_MSG);

							fputc('3', tempFile);
							while((data = fgetc(writeToFile)) != '\n' && data != EOF)
								fputc(data, tempFile);

							fputc('\n', tempFile);
							if(argv[c][strlen(argv[c]) - 1] == '/')
								fprintf(tempFile, "4>%s", argv[c]);
							else
								fprintf(tempFile, "4>%s/", argv[c]);

							moveFile("DownloadToTemp.txt", WHERE_SEND_FILES);
							
							//courrupted second line
						}else{
							printError(DOWNLOAD_READ_CODE, DOWNLOAD_READ_MSG);
						}
						//read MP3 line to copy into written file
					}else{
						FILE* tempFile = fopen("DownloadToTemp.txt", "w");
						if(tempFile == NULL) printError(TEMP_FILE_FAIL_CODE, TEMP_FILE_FAIL_MSG);

						fputc('3', tempFile);
						while((data = fgetc(writeToFile)) != '\n' && data != EOF)
							fputc(data, tempFile);

						fputc('\n', tempFile);
						if(argv[c][strlen(argv[c]) - 1] == '/')
							fprintf(tempFile, "4>%s\n", argv[c]);
						else
							fprintf(tempFile, "4>%s/\n", argv[c]);

						moveFile("DownloadToTemp.txt", WHERE_SEND_FILES);
					}
				}
				fclose(writeToFile);
				puts("Successfully written to DownloadTo.txt where to send video files");
			}

			//where to send mp3(audio) files
			//NEED TO DO MORE USER VALIDATION
			else if(strcmp("-w3", argv[c]) == 0){
				if(argc - 1 == c){
					printf(W3_NOT_GIVEN);
					exit(1);
				}

				//check if directory exists
				if(!checkIfExists(argv[++c], 'd')) printError(DIR_FAIL_CODE, DIR_FAIL_MSG);

				FILE* writeToFile = NULL;
				//File does not exist must create it
				if(!checkIfExists(WHERE_SEND_FILES, 'f')){
					writeToFile = fopen(WHERE_SEND_FILES, "w");
					if(writeToFile == NULL) printError(DOWNLOAD_CREATE_CODE, DOWNLOAD_CREATE_MSG);

					if(argv[c][strlen(argv[c]) - 1] == '/')
						fprintf(writeToFile, "3>%s\n", argv[c]);
					else
						fprintf(writeToFile, "3>%s/\n", argv[c]);
					//file exists and must check if second line exists
				}else{
					//int length = 0;
					int data = '\0';
					//char* line = NULL;
					FILE* tempFile = fopen("DownloadToTemp.txt", "w");
					if(tempFile == NULL) printError(TEMP_FILE_FAIL_CODE, TEMP_FILE_FAIL_MSG);

					writeToFile = fopen(WHERE_SEND_FILES, "r");
					//find next line if it exists
					if(fgetc(writeToFile) != '4'){
						//skip to second line
						while((data = fgetc(writeToFile)) != '\n' && data != EOF);

						//there is no second line
						if((data = fgetc(writeToFile)) == EOF){
							if(writeToFile == NULL) printError(DOWNLOAD_CREATE_CODE, DOWNLOAD_CREATE_MSG);
							if(argv[c][strlen(argv[c]) - 1] == '/')
								fprintf(writeToFile, "3>%s\n", argv[c]);
							else
								fprintf(writeToFile, "3>%s/\n", argv[c]);
						//second line has the mp4 path, copy to newly written
						}else if(data == '4'){
							FILE* tempFile = fopen("DownloadToTemp.txt", "w");
							if(tempFile == NULL) printError(TEMP_FILE_FAIL_CODE, TEMP_FILE_FAIL_MSG);

							fputc('4', tempFile);
							while((data = fgetc(writeToFile)) != '\n' && data != EOF)
								fputc(data, tempFile);

							fputc('\n', tempFile);
							if(argv[c][strlen(argv[c]) - 1] == '/')
								fprintf(tempFile, "3>%s", argv[c]);
							else
								fprintf(tempFile, "3>%s/", argv[c]);

							moveFile("DownloadToTemp.txt", WHERE_SEND_FILES);
						}else{
							printError(DOWNLOAD_READ_CODE, DOWNLOAD_READ_MSG);
						}
						//read MP4 line to copy into written file
					}else{
						FILE* tempFile = fopen("DownloadToTemp.txt", "w");
						if(tempFile == NULL) printError(TEMP_FILE_FAIL_CODE, TEMP_FILE_FAIL_MSG);

						fputc('4', tempFile);
						while((data = fgetc(writeToFile)) != '\n' && data != EOF)
							fputc(data, tempFile);

						fputc('\n', tempFile);
						if(argv[c][strlen(argv[c]) - 1] == '/')
							fprintf(tempFile, "3>%s", argv[c]);
						else
							fprintf(tempFile, "3>%s/", argv[c]);

						moveFile("DownloadToTemp.txt", WHERE_SEND_FILES);
					}
				}
				fclose(writeToFile);
				puts("Successfully written to DownloadTo.txt where to send audio files");
			}else if(strcmp("-ca", argv[c]) == 0){
				if(argc - 1 == c){
					printf(COVER_ART_NOT_GIVEN);
					exit(1);
				}

				if(strcmp("default", argv[c + 1]) == 0){
					coverArtDefaultMode = 1;
					coverArtMode = 1;
				}else{
					if(!checkIfExists(argv[c + 1], 'f')) printError(FILE_FAIL_CODE, FILE_FAIL_MSG);
					coverArtMode = 1;
				}

				coverArtIndex = ++c;
			}else{
				printf(UNKNOWN_ARG, argv[c]);
				exit(1);
			}
		}
	}else{
		//default execution
		do{
			//ask for directory
			char* sendMP4 = getMP4Dest();
			char* sendMP3 = getMP3Dest();

			if(sendMP4 == NULL) printError(SKIP_VALID_CODE, SKIP_VALID_MSG);
			if(sendMP3 == NULL) printError(SKIP_VALID_CODE, SKIP_VALID_MSG);

			printf("MP4 will be sent to %s and the MP3 will be send to %s\n", sendMP4, sendMP3);

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
	}

	//finished parsing arguments

	//check if both dynamic modes are desired
	//this will download youtubeURLS and add .mp3 cover arts
	//This will also accept a file that contains both youtube URLS and .mp3 paths
	//NEED TO FIGURE OUT WHAT TO DO WITH DEFAULT
	if(fileMode && coverArtMode){
		char buffer [LARGER_BUFFER_SIZE] = "";
		FILE* inFile = fopen(argv[fileFlagIndex], "r");
		if(inFile == NULL) printError(FILE_FAIL_CODE, FILE_FAIL_MSG);

		char* sendMP4 = getMP4Dest();
		char* sendMP3 = getMP3Dest();
		if(sendMP4 == NULL) printError(SKIP_VALID_CODE, SKIP_VALID_MSG);
		if(sendMP3 == NULL) printError(SKIP_VALID_CODE, SKIP_VALID_MSG);
		printf("MP4 will be sent to %s and the MP3 will be send to %s\n", sendMP4, sendMP3);

		while(exactFileInput(inFile, buffer, LARGER_BUFFER_SIZE) != 0){
			if(strstr(buffer, YOUTUBE_URL) != NULL){
				//have to shorten URL so yt-dlp does not return
				//early from parsing through a playlist
				char shortenedURL [YT_URL_BUFFER] = "";

				//snprintf was complaining about truncation and
				//I didn't want compliation warnings
				strncpy(shortenedURL, buffer, YT_URL_BUFFER);
				shortenedURL[YT_URL_BUFFER - 1] = '\0';
				downloadURL(shortenedURL);
				char* youtubeID = getID(shortenedURL);
				char* song = getSongName(youtubeID);
				free(youtubeID);

				//convert to mp3 and move mp4 and mp3 to its spots
				char* mp3 = convertToMp3(song);
				//add coverArt
				writeCover(mp3, argv[coverArtIndex]);
				moveFile(song, sendMP4);
				moveFile(mp3, sendMP3);

				free(mp3);
				free(song);
			}else if(strstr(buffer, ".mp3") != NULL){
				char* quoted = surroundInQuotes(buffer);
				if(checkIfExists(quoted, 'f')){
					writeCover(buffer, argv[coverArtIndex]);
					moveFile(quoted, sendMP3);
				}else{
					printf(PNT_RED"can't find .mp3 %s via its path\n"PNT_RESET, buffer);
				}
				free(quoted);
			}else{
				printf(PNT_RED"Line obtianed is not a youtube URL or .MP3 %s\n"PNT_RESET, buffer);
			}
			memset(buffer, '\0', strlen(buffer));
		}
		free(sendMP4);
		free(sendMP3);

		fclose(inFile);

	//writes cover art while downloading youtube vidoes
	}else if(coverArtMode){
		//default mode is using the video thumbnail
		if(coverArtDefaultMode){
			//<TODO CODE>

		//when a cover art is specified
		}else{
			do{
				puts("NOTE: this will not ask to reset the cover art if you decide to download more!");
				//ask for directory
				char* sendMP4 = getMP4Dest();
				char* sendMP3 = getMP3Dest();

				if(sendMP4 == NULL) printError(SKIP_VALID_CODE, SKIP_VALID_MSG);
				if(sendMP3 == NULL) printError(SKIP_VALID_CODE, SKIP_VALID_MSG);


				printf("MP4 will be sent to %s and the MP3 will be send to %s\n", sendMP4, sendMP3);

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
				//add coverArt
				writeCover(mp3, argv[coverArtIndex]);
				moveFile(song, sendMP4);
				moveFile(mp3, sendMP3);

				free(mp3);
				free(song);
				free(sendMP4);
				free(sendMP3);
				puts("Download was successful");
			}while(repeat() == 1);
		}

	//basically default execution with the addition of a file
	}else if(fileMode){
		FILE* inFile = fopen(argv[fileFlagIndex], "r");
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
			char* mp3 = convertToMp3(song);
			moveFile(song, sendMP4);
			moveFile(mp3, sendMP3);

			free(mp3);
			free(song);
			memset(urls,'\0', YT_URL_BUFFER);
		}

		free(sendMP4);
		free(sendMP3);
		puts("Download was successful");

		fclose(inFile);
	}

	return 0;
}
