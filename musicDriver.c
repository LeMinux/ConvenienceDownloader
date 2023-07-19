#include <string.h>
#include <stdio.h>
#include "cMusicDownload.h"
#include "helpers.h"
#include "linkedList.h"
#include "writeArt.h"

#define UNKNOWN_ARG "Unknown argument of %s\n"
#define COVER_ART_NOT_GIVEN "Did not specify a file for Cover art"
#define FILE_NOT_GIVEN "Did not specify a file to use"
#define W4_NOT_GIVEN "Did not provide the path to download video files to"
#define W3_NOT_GIVEN "Did not provide the path to download audio files to"
#define MP3_ERROR "File must contain .MP3 files for writing cover art to"

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
		default: printError(1, "Client passed unknown mode for writing to DownloadTo.txt");
		break;
	}
}

int main(int argc, char** argv){
	//modes for different execution
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
					printError(1, FILE_NOT_GIVEN);
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
					writeFormat(writeToFile, argv[c], 4);

				//file exists and must check if second line exists
				}else{
					int data = '\0';
					writeToFile = fopen(WHERE_SEND_FILES, "r");
					FILE* tempFile = fopen("DownloadToTemp.txt", "w");
					if(tempFile == NULL) printError(TEMP_FILE_FAIL_CODE, TEMP_FILE_FAIL_MSG);

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
							printError(DOWNLOAD_READ_CODE, DOWNLOAD_READ_MSG);
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
					writeFormat(writeToFile, argv[c], 3);

				//file exists and must check if second line exists
				}else{
					int data = '\0';
					writeToFile = fopen(WHERE_SEND_FILES, "r");
					FILE* tempFile = fopen("DownloadToTemp.txt", "w");
					if(tempFile == NULL) printError(TEMP_FILE_FAIL_CODE, TEMP_FILE_FAIL_MSG);

					//find next line if it exists
					if(fgetc(writeToFile) != '4'){
						//skip to second line
						while((data = fgetc(writeToFile)) != '\n' && data != EOF);

						//there is no second line
						if((data = fgetc(writeToFile)) == EOF){
							if(writeToFile == NULL) printError(DOWNLOAD_CREATE_CODE, DOWNLOAD_CREATE_MSG);
							writeFormat(tempFile, argv[c], 3);

						//second line has the mp4 path, copy to newly written
						}else if(data == '4'){
							fputc('4', tempFile);
							while((data = fgetc(writeToFile)) != '\n' && data != EOF)
								fputc(data, tempFile);

							fputc('\n', tempFile);
							writeFormat(tempFile, argv[c], 3);
						}else{
							printError(DOWNLOAD_READ_CODE, DOWNLOAD_READ_MSG);
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
				printError(1, UNKNOWN_ARG);
				//printf(UNKNOWN_ARG, argv[c]);
				exit(1);
			}
		}
	}else{
		//default execution this also uses the default mode for cover arts
		do{
			//ask for directory
			char* sendMP4 = getDests(4, "Where do you want to send the MP4? or type exit: ");
			char* sendMP3 = getDests(3, "Where do you want to send the MP3? or type exit: ");

			if(sendMP4 == NULL) printError(SKIP_VALID_CODE, SKIP_VALID_MSG);
			if(sendMP3 == NULL) printError(SKIP_VALID_CODE, SKIP_VALID_MSG);

			printf("MP4 will be sent to %s and the MP3 will be send to %s\n", sendMP4, sendMP3);

			//get URL from user
			char* url = getURL();

			//download song via URL
			downloadURL(url, 1);

			//get the ID for later movement
			char* youtubeID = getID(url);
			free(url);

			//get song name via youtube ID
			grepIntoFile(youtubeID);
			free(youtubeID);

			//read file to get both .jpg and .mp4
			FILE* greppedFile = fopen("GrepTemp.txt", "r");
			char* coverArt = NULL;
			char* song = NULL;

			//the first line will always be the .jpg
			unkownFileRead(greppedFile, &coverArt);
			unkownFileRead(greppedFile, &song);
			system("rm GrepTemp.txt");

			//convert to mp3 and move mp4 and mp3 to its spots
			char* mp3 = convertToMp3(song);
			//add coverArt
			writeCover(mp3, coverArt);
			char* removeArt = malloc(3 + strlen(coverArt) + 1);
			snprintf(removeArt, 3 + strlen(coverArt) + 1, "%s%s", "rm ", coverArt);
			system(removeArt);
			free(removeArt);
			moveFile(song, sendMP4);
			moveFile(mp3, sendMP3);

			free(mp3);
			free(song);
			free(coverArt);
			free(sendMP4);
			free(sendMP3);
			puts("Download was successful");

			/*
			//download song via URL
			downloadURL(url, 0);

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
			*/
		}while(repeat() == 1);
	}

	//finished parsing arguments

	//check if both dynamic modes are desired
	//this will download youtubeURLS and add .mp3 cover arts
	//This will also accept a file that contains both youtube URLS and .mp3 paths
	//NEED TO FIGURE OUT WHAT TO DO WITH DEFAULT
	if(fileMode && coverArtMode){
		char* buffer = NULL;
		FILE* inFile = fopen(argv[fileFlagIndex], "r");
		if(inFile == NULL) printError(FILE_FAIL_CODE, FILE_FAIL_MSG);

		char* sendMP4 = getDests(4, "Where do you want to send the MP4? or type exit: ");
		char* sendMP3 = getDests(3, "Where do you want to send the MP3? or type exit: ");
		if(sendMP4 == NULL) printError(SKIP_VALID_CODE, SKIP_VALID_MSG);
		if(sendMP3 == NULL) printError(SKIP_VALID_CODE, SKIP_VALID_MSG);
		printf("MP4 will be sent to %s and the MP3 will be send to %s\n", sendMP4, sendMP3);

		while(unkownFileRead(inFile, &buffer) != 0){
			//if line read is a youtube URL
			if(strstr(buffer, YOUTUBE_URL) != NULL){
				//have to shorten URL so yt-dlp does not return
				//early from parsing through a playlist
				char shortenedURL [YT_URL_BUFFER] = "";

				//snprintf was complaining about truncation and
				//I didn't want compliation warnings
				strncpy(shortenedURL, buffer, YT_URL_BUFFER);
				shortenedURL[YT_URL_BUFFER - 1] = '\0';

				char* mp3 = NULL;
				char* song = NULL;
				char* youtubeID = getID(shortenedURL);
				if(coverArtDefaultMode){
					downloadURL(shortenedURL, 1);
					//get song name via youtube ID
					grepIntoFile(youtubeID);

					//read file to get both .jpg and .mp4
					FILE* greppedFile = fopen("GrepTemp.txt", "r");
					char* coverArt = NULL;

					//the first line will always be the .jpg
					unkownFileRead(greppedFile, &coverArt);
					unkownFileRead(greppedFile, &song);

					system("rm GrepTemp.txt");

					//convert to mp3 and move mp4 and mp3 to its spots
					mp3 = convertToMp3(song);
					//add coverArt
					writeCover(mp3, coverArt);
					char* removeArt = malloc(3 + strlen(coverArt) + 1);
					snprintf(removeArt, 3 + strlen(coverArt) + 1, "%s%s", "rm ", coverArt);
					system(removeArt);
					free(coverArt);
					free(removeArt);
					moveFile(song, sendMP4);
					moveFile(mp3, sendMP3);
				}else{
					downloadURL(shortenedURL, 0);
					song = getSongName(youtubeID);
					//convert to mp3 and move mp4 and mp3 to its spots
					mp3 = convertToMp3(song);
					//add coverArt
					writeCover(mp3, argv[coverArtIndex]);
					moveFile(song, sendMP4);
					moveFile(mp3, sendMP3);
				}

				free(youtubeID);
				free(mp3);
				free(song);
			//if line read is a .mp3 file
			}else if(strstr(buffer, ".mp3") != NULL){
				char* quoted = surroundInQuotes(buffer);
				if(checkIfExists(quoted, 'f')){
					if(coverArtDefaultMode){
						moveFile(quoted, sendMP3);
					}else{
						writeCover(buffer, argv[coverArtIndex]);
						moveFile(quoted, sendMP3);
					}
				}else{
					printf(PNT_RED"can't find .mp3 %s via its path\n"PNT_RESET, buffer);
				}
				free(quoted);
			}else{
				printf(PNT_RED"Line obtianed is not a youtube URL or .MP3 %s\n"PNT_RESET, buffer);
			}
			free(buffer);
			buffer = NULL;
		}
		free(sendMP4);
		free(sendMP3);
		fclose(inFile);

	//writes cover art while downloading youtube vidoes
	}else if(coverArtMode){
		//default mode is using the video thumbnail
		if(coverArtDefaultMode){
			do{
				puts("NOTE: this will not ask to reset the cover art if you decide to download more!");
				//ask for directory
				char* sendMP4 = getDests(4, "Where do you want to send the MP4? or type exit: ");
				char* sendMP3 = getDests(3, "Where do you want to send the MP4? or type exit: ");

				if(sendMP4 == NULL) printError(SKIP_VALID_CODE, SKIP_VALID_MSG);
				if(sendMP3 == NULL) printError(SKIP_VALID_CODE, SKIP_VALID_MSG);

				printf("MP4 will be sent to %s and the MP3 will be send to %s\n", sendMP4, sendMP3);

				//get URL from user
				char* url = getURL();

				//download song via URL
				downloadURL(url, 1);

				//get the ID for later movement
				char* youtubeID = getID(url);
				free(url);

				//get song name via youtube ID
				grepIntoFile(youtubeID);
				free(youtubeID);

				//read file to get both .jpg and .mp4
				FILE* greppedFile = fopen("GrepTemp.txt", "r");
				char* coverArt = NULL;
				char* song = NULL;

				//the first line will always be the .jpg
				unkownFileRead(greppedFile, &coverArt);
				unkownFileRead(greppedFile, &song);

				system("rm GrepTemp.txt");

				//convert to mp3 and move mp4 and mp3 to its spots
				char* mp3 = convertToMp3(song);
				//add coverArt
				writeCover(mp3, coverArt);
				char* removeArt = malloc(3 + strlen(coverArt) + 1);
				snprintf(removeArt, 3 + strlen(coverArt) + 1, "%s%s", "rm ", coverArt);
				system(removeArt);
				free(removeArt);
				moveFile(song, sendMP4);
				moveFile(mp3, sendMP3);

				free(mp3);
				free(song);
				free(coverArt);
				free(sendMP4);
				free(sendMP3);
				puts("Download was successful");
			}while(repeat() == 1);

		//when a cover art is specified
		}else{
			do{
				puts("NOTE: this will not ask to reset the cover art if you decide to download more!");
				//ask for directory
				char* sendMP4 = getDests(4, "Where do you want to send the MP4? or type exit: ");
				char* sendMP3 = getDests(3, "Where do you want to send the MP3? or type exit: ");

				if(sendMP4 == NULL) printError(SKIP_VALID_CODE, SKIP_VALID_MSG);
				if(sendMP3 == NULL) printError(SKIP_VALID_CODE, SKIP_VALID_MSG);


				printf("MP4 will be sent to %s and the MP3 will be send to %s\n", sendMP4, sendMP3);

				//get URL from user
				char* url = getURL();

				//download song via URL
				downloadURL(url, 0);

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

		char* sendMP4 = getDests(4, "Where do you want to send the MP4? or type exit: ");
		char* sendMP3 = getDests(3, "Where do you want to send the MP4? or type exit: ");
		if(sendMP4 == NULL) printError(SKIP_VALID_CODE, SKIP_VALID_MSG);
		if(sendMP3 == NULL) printError(SKIP_VALID_CODE, SKIP_VALID_MSG);

		printf(PNT_GREEN"Downloading MP4s to %s and MP3s to %s\n"PNT_RESET, sendMP4, sendMP3);

		char urls [YT_URL_BUFFER] = "";
		//get URLs assumming they are separating by \n
		while(exactFileInput(inFile, urls, YT_URL_BUFFER) != 0){
			//download URL
			downloadURL(urls, 0);

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
