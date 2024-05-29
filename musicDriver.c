/*COMPLETED
 * When listing the directories do you want it to exit or no?
 * Redo how you get destination info. Separate it out into mp4, mp3, and maybe cover arts
 * Think about using an init to place dests into a global var and also verify everything is in place
 * Verify files before code runs. This includes before commands, but not prompt errors when trying to make the files
 * Do you really need so many input functions or can you just simplify unknown getting and just make that safe and generic
 * Change the modes. It would be better to pass the string since each one wants it. Plus you already know it
 * Simplify the branching of main.
 * See if you can bring down the many free and malloc calls.
 * Maybe separate functions out into more specific header files
 * When converting to mp3 set bitrate to 256K
 * check if the define statements are in their correct header file
 * Add error checking for if finding the file fails
 * Destinations for cover arts
 * Replace the commands with stat (things like ls and find)
 * Change how downloadMode is set in the getModeFromSelection function
 * Change the wantArt and coverArtMode before actual execution
*/

/*TODO
 * Fix bug that creates two // in getting sub dirs
 * For file execution think about adding a way to change destinations mid-way.
 *	(!3>../Bangers/Extreme)
 *	(!4>../Bangers/Extreme)
 *	(!c>../Bangers/Extreme)
 * For file execution if errors occur try to log what URLS or paths were ignored
 *	instead of just crashing the program which results in manual cleansing of the file
 * Add cover art related things such as keeping or discarding them
 * Fix how typing the dests is. Is it really necessary to type the entire path?
 * Adding NULL protection in writeCover()?
 * mv command or rename function?
 * perhaps have a way to add more metadata to mp3 files?
*/

/*CONTINUOUS
 * Use program :P
*/

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "userInput.h"
#include "helpers.h"
#include "linkedList.h"
#include "writeArt.h"
#include "fileOps.h"

#define W4_NOT_GIVEN "Destination to send video does not exist. Please specify where to send them with the -w4 flag"
#define W3_NOT_GIVEN "Destination to send audio does not exist. Please specify where to send them with the -w3 flag\n"
#define NO_PERMISSION "Writting privleges for the given path is not availiable\n"

#define USER_MP4_PROMPT "Where do you want to send the MP4? Type exit to leave or type skip to avoid sending files: "
#define USER_MP3_PROMPT "Where do you want to send the MP3? Type exit to leave or type skip to avoid sending files: "
#define USER_MP3_PROMPT_NO_SKIP "Where do you want to send the MP3? Type exit to leave: "
#define USER_MP4_PROMPT_NO_SKIP "Where do you want to send the MP4? Type exit to leave: "

#define ALLOW_SKIPS 1
#define DISALLOW_SKIPS 0


#define DEFAULT_MODE -1
#define MP3_MODE 3
#define MP4_MODE 4
#define DWNLD_COVER_ART 1
#define NO_DWNLD_COVER_ART 0

#define MP3_EXT ".mp3"
#define MP4_EXT ".mp4"
#define JPG_EXT ".jpg"

#define ID_BUFFER 12

//this struct is here so that function pointers can be used for moving files
//this helps generalize the code a little, which does make it less efficient,
//but it makes changes much easier
typedef struct MovePackage{
	char* id;
	char* audioDest;
	char* videoDest;
	int hasCoverArt;
	char* artName;
} MovePackage;

//package for holding modes
//this also makes it so gettingModeFromSelection doesn't need to worry about NULL
typedef struct ModePackage{
	int downloadMode;
	int coverMode;
} ModePackage;

//global dynamic variables
//these act as the base path. This way opening the destination files only happens once
static char* MP4_BASE_DIR;
static char* MP3_BASE_DIR;
static char* COVER_BASE_DIR;
static char* COVER_BASE_DIR;

void __attribute__((constructor)) initPaths (){
	if(!checkIfExists(DES)){
		printError(EXIT_FAILURE, "Destinations directory is not present. Please run the makefile with \"make\"");
	}

	if(!checkIfExists(DES_MP4)){
	//fprintf(writeTo, "%s", string);
		printf(PNT_RED"Destinations for videos have not been initalized.\n"PNT_RESET);
		int valid = 0;
		char* buffer = NULL;
		while(!valid){
			printf("Please type the path to where you want to send video files-> ");
			unknownInput(stdin, &buffer);
			if(strlen(buffer) == 0){
				puts(PNT_RED"Enter something."PNT_RESET);
			}else if(!checkIfExists(buffer)){
				puts(PNT_RED"The path specified does not exist."PNT_RESET);
			}else if(access(buffer, W_OK) == -1){
				printf(PNT_RED"The path specified does not have writting permissions\n"PNT_RESET);
			}else{
				valid = 1;
				writeDest(buffer, 4);
			}
			free(buffer);
			buffer = NULL;
		}
	}

	if(!checkIfExists(DES_MP3)){
		printf(PNT_RED"Destinations for audios have not been initalized.\n"PNT_RESET);
		int valid = 0;
		char* buffer = NULL;
		while(!valid){
			printf("Please type the path to where you want to send audio files-> ");
			unknownInput(stdin, &buffer);
			if(strlen(buffer) == 0){
				puts(PNT_RED"Enter something."PNT_RESET);
			}else if(!checkIfExists(buffer)){
				puts(PNT_RED"The path specified does not exist"PNT_RESET);
			}else if(access(buffer, W_OK) == -1){
				puts(PNT_RED"The path specified does not have writting permissions"PNT_RESET);
			}else{
				valid = 1;
				writeDest(buffer, 3);
			}
			free(buffer);
			buffer = NULL;
		}
	}

	if(!checkIfExists(DES_COVER)){
		printf(PNT_RED"Where to get covers has not been initalized.\n"PNT_RESET);
		int valid = 0;
		char* buffer = NULL;
		while(!valid){
			printf("Please type the path to where you extract your cover art files-> ");
			unknownInput(stdin, &buffer);
			if(strlen(buffer) == 0){
				puts(PNT_RED"Enter something."PNT_RESET);
			}else if(!checkIfExists(buffer)){
				puts(PNT_RED"The path specified does not exist"PNT_RESET);
			}else if(access(buffer, W_OK) == -1){
				puts(PNT_RED"The path specified does not have writting permissions"PNT_RESET);
			}else{
				valid = 1;
				writeDest(buffer, 5);
			}
			free(buffer);
			buffer = NULL;
		}
	}

	FILE* mp4Dest = NULL;
	FILE* mp3Dest = NULL;
	FILE* coverDest = NULL;

	mp4Dest = fopen(DES_MP4, "r");
	if(mp4Dest == NULL) printError(EXIT_FAILURE, "Failed to open video destination file");

	mp3Dest = fopen(DES_MP3, "r");
	if(mp3Dest == NULL) printError(EXIT_FAILURE, "Failed to open audio destination file");

	coverDest = fopen(DES_COVER, "r");
	if(coverDest == NULL) printError(EXIT_FAILURE, "Failed to open cover destination file");


	unknownInput(mp4Dest, &MP4_BASE_DIR);
	unknownInput(mp3Dest, &MP3_BASE_DIR);
	unknownInput(coverDest, &COVER_BASE_DIR);

	fclose(mp4Dest);
	fclose(mp3Dest);
	fclose(coverDest);
}

//mode specifies audio or video
//for skipping, 0 is false and 1 is true
static char* askUserForPath(int mode, int allowSkipping){
	const char* baseDir = NULL;
	const char* prompt = NULL;
	char* (*askFunction)(const char*, const char*) = NULL;

	switch(mode){
		case MP3_MODE:
			baseDir = MP3_BASE_DIR;
			if(allowSkipping){
				prompt = USER_MP3_PROMPT;
				askFunction = getUserChoiceForDir;
			}else{
				prompt = USER_MP3_PROMPT_NO_SKIP;
				askFunction = getUserChoiceForDirNoSkip;
			}
		break;

		case MP4_MODE:
			baseDir = MP4_BASE_DIR;
			if(allowSkipping){
				prompt = USER_MP4_PROMPT;
				askFunction = getUserChoiceForDir;
			}else{
				prompt = USER_MP4_PROMPT_NO_SKIP;
				askFunction = getUserChoiceForDirNoSkip;
			}
		break;

		default:
			printError(EXIT_FAILURE, "Client passed invalid mode for getting path");
		break;
    }

	return (*askFunction)(baseDir, prompt);
}

//helps keep the priniciple of downloading once
static void convertFirstMoveBoth(MovePackage* movingInfo){
	char videoFileName [MAX_FILE_NAME + 1] = "";
	char audioFileName [MAX_FILE_NAME + 1] = "";

	getFileNameByID(movingInfo->id, MP4_EXT, videoFileName, MAX_FILE_NAME);
	if(strlen(videoFileName) == 0)
		printError(EXIT_FAILURE, "Could not find video file after downloading");

	convertToMp3(videoFileName);

	getFileNameByID(movingInfo->id, MP3_EXT, audioFileName, MAX_FILE_NAME);
	if(strlen(audioFileName) == 0)
		printError(EXIT_FAILURE, "Could not find audio file after downloading");



	if(movingInfo->hasCoverArt){
		if(movingInfo->artName == NULL){
			char jpgFileName [MAX_FILE_NAME + 1] = "";
			getFileNameByID(movingInfo->id, JPG_EXT, jpgFileName, MAX_FILE_NAME);
			if(strlen(jpgFileName) == 0)
				printError(EXIT_FAILURE, "Could not find cover file after downloading");

			writeCover(audioFileName, jpgFileName);
			removeCoverArt(jpgFileName);
			moveFile(videoFileName, movingInfo->videoDest);
			moveFile(audioFileName, movingInfo->audioDest);
		}else{
			writeCover(audioFileName, movingInfo->artName);
			moveFile(videoFileName, movingInfo->videoDest);
			moveFile(audioFileName, movingInfo->audioDest);
		}
	}else{
		moveFile(videoFileName, movingInfo->videoDest);
		moveFile(audioFileName, movingInfo->audioDest);
	}
}

void moveVideo (MovePackage* movingInfo){
	char videoFileName [MAX_FILE_NAME + 1] = "";
	getFileNameByID(movingInfo->id, MP4_EXT, videoFileName, MAX_FILE_NAME);
	if(strlen(videoFileName) == 0)
		printError(EXIT_FAILURE, "Could not find video file after downloading");

	moveFile(videoFileName, movingInfo->videoDest);
}

static void moveAudio(MovePackage* movingInfo){
	char audioFileName [MAX_FILE_NAME + 1] = "";

	getFileNameByID(movingInfo->id, MP3_EXT, audioFileName, MAX_FILE_NAME);
	if(strlen(audioFileName) == 0)
		printError(EXIT_FAILURE, "Could not find audio file after downloading");

	if(movingInfo->hasCoverArt){
		if(movingInfo->artName == NULL){
			char jpgFileName [MAX_FILE_NAME + 1] = "";
			getFileNameByID(movingInfo->id, JPG_EXT, jpgFileName, MAX_FILE_NAME);
			if(strcmp(jpgFileName, "") == 0)
				printError(EXIT_FAILURE, "Could not find cover file after downloading");

			writeCover(audioFileName, jpgFileName);
			removeCoverArt(jpgFileName);
			moveFile(audioFileName, movingInfo->audioDest);
		}else{
			writeCover(audioFileName, movingInfo->artName);
			moveFile(audioFileName, movingInfo->audioDest);
		}
	}else{
		moveFile(audioFileName, movingInfo->audioDest);
	}
}

//The truth table for default execution is
/*
 *	Skip Audio	|	Skip Video	|	Mode	|	Cover Art
 *		0		|		0		|	MP4	|	1
 *		0		|		1		|	MP3	|	1
 *		1		|		0		|	MP4	|	0
 *		1		|		1		|	X	|	X
 *
*/
//helper method to set modes
//passing in negative one for the mode selection lets this method decide what it
//would be according to default execution, but some cases require an override
//so it is allowed to pass in NULL for the modes if that is already known
static void getModeFromSelection(const char* videoDest, const char* audioDest, ModePackage* modes, void (**moveFunction)(MovePackage*)){

	if(modes->coverMode == DEFAULT_MODE){
		//if skipping audio
		if(strcmp(audioDest, "SKIP") == 0)
			modes->coverMode = NO_DWNLD_COVER_ART;
		else
			modes->coverMode = DWNLD_COVER_ART;
	}

	//this is basically going to always be default mode
	//just here for future implementations
	if(modes->downloadMode == DEFAULT_MODE){
		if(strcmp(videoDest, "SKIP") == 0)
			modes->downloadMode = MP3_MODE;
		else
			modes->downloadMode = MP4_MODE;
	}

	if(strcmp(videoDest, "SKIP") != 0 && strcmp(audioDest, "SKIP") != 0)
		*moveFunction = convertFirstMoveBoth;
	else if(strcmp(videoDest, "SKIP") == 0)
		*moveFunction = moveAudio;
	else
		*moveFunction = moveVideo;

}

int main(int argc, char** argv){
	//modes for different execution
	int fileMode = 0;
	int fileFlagIndex = 0;
	int coverArtMode = DEFAULT_MODE;
	int coverArtIndex = 0;
	int keepArt = 0;

	int c = 1;
	for(; c < argc; ++c){
		if(strcmp("-l", argv[c]) == 0){
			puts("v List of availiable directories for MP4");
			Node_t* listOfDirs = NULL;
			getSubdirectories(MP4_BASE_DIR, &listOfDirs);

			printList(listOfDirs);
			deleteList(&listOfDirs);

			puts("\nv List of availiable directories for MP3");

			getSubdirectories(MP3_BASE_DIR, &listOfDirs);
			printList(listOfDirs);
			deleteList(&listOfDirs);
			exit(EXIT_SUCCESS);
		}else if(strcmp("-f", argv[c]) == 0){
			if(!checkIfExists(argv[c + 1])) printError(EXIT_FAILURE, FILE_FAIL_MSG);

			fileMode = 1;
			fileFlagIndex = ++c;

		//where to send mp4(video) files
		}else if(strcmp("-w4", argv[c]) == 0){

			if(!checkIfExists(argv[++c])) printError(EXIT_FAILURE, DIR_FAIL_MSG);

			//check if given writting privleges
			if(access(argv[c], W_OK) == -1) printError(EXIT_FAILURE, NO_PERMISSION);

			writeDest(argv[c], 4);
			puts("Successfully written where to send video files");
			exit(EXIT_SUCCESS);
		}

		//where to send mp3(audio) files
		else if(strcmp("-w3", argv[c]) == 0){

			if(!checkIfExists(argv[++c])) printError(EXIT_FAILURE, DIR_FAIL_MSG);

			//check if given writting privleges
			if(access(argv[c], W_OK) == -1) printError(EXIT_FAILURE, NO_PERMISSION);

			writeDest(argv[c], 3);
			puts("Successfully written where to send audio files");
			exit(EXIT_SUCCESS);

		}else if(strcmp("-ca", argv[c]) == 0){
			if(strcmp("NO-ART", argv[c + 1]) != 0){
				if(!checkIfExists(argv[c + 1])) printError(EXIT_FAILURE, FILE_FAIL_MSG);
			}

			coverArtMode = NO_DWNLD_COVER_ART;
			coverArtIndex = c + 1;
			++c;
		}else if(strcmp("--keep-art", argv[c]) == 0){
			keepArt = 1;
		}else{
			fprintf(stderr, PNT_RED"Invalid argument %s\n"PNT_RESET, argv[c]);
			exit(EXIT_FAILURE);
		}
	}//finished parsing arguments

	if(keepArt && coverArtMode == NO_DWNLD_COVER_ART)
		printError(EXIT_FAILURE, "Can not keep cover art and specify a cover art");

	char* sendAudio = NULL;
	char* sendVideo = NULL;
	void (*moveFunction)(MovePackage*) = NULL;
	char id [ID_BUFFER] = "";
	MovePackage movementInfo;
	ModePackage modeInfo = {DEFAULT_MODE, coverArtMode};
	char* coverArt = NULL;

	if(coverArtMode == NO_DWNLD_COVER_ART && strcmp(argv[coverArtIndex], "NO-ART") != 0)
		coverArt = argv[coverArtIndex];

	//obtain where to send files
	//depending on the flags only certain paths can be skipped
	if(coverArtMode != DEFAULT_MODE){
		puts(PNT_RED"NOTE: for this option there is no skipping MP3 paths since the cover art option is selected."PNT_RESET);
		sendVideo = askUserForPath(MP4_MODE, ALLOW_SKIPS);
		sendAudio = askUserForPath(MP3_MODE, DISALLOW_SKIPS);
	}else{
		int valid = 0;
		do{
			sendVideo = askUserForPath(MP4_MODE, ALLOW_SKIPS);
			sendAudio = askUserForPath(MP3_MODE, ALLOW_SKIPS);

			if((strcmp(sendAudio, "SKIP") == 0 && strcmp(sendVideo, "SKIP") == 0)){
				puts(PNT_RED"Both destinations can not be skipped!"PNT_RESET);
			}else{
				valid = 1;
			}
		}while(!valid);
	}

	//check if skipped entirely
	if(sendAudio == NULL) printError(EXIT_FAILURE, SKIP_VALID_MSG);
	if(sendVideo == NULL) printError(EXIT_FAILURE, SKIP_VALID_MSG);

	getModeFromSelection(sendVideo, sendAudio, &modeInfo, &moveFunction);
	movementInfo = (MovePackage){id, sendAudio, sendVideo, modeInfo.coverMode, coverArt};

	//default and cover art mode
	if(!fileMode){
		int repeat = 0;
		do{
			//execution for downloading
			char* url = getURL();

			//snprintf will rewrite the id string
			snprintf(movementInfo.id, ID_BUFFER, "%s", strstr(url, "?v=") + 3);

			downloadFromURL(url, modeInfo.downloadMode, modeInfo.coverMode);
			(*moveFunction)(&movementInfo);

			free(sendVideo);
			free(sendAudio);
			sendAudio = NULL;
			sendVideo = NULL;

			if((repeat = askToRepeat()) == 1){
				if(coverArtMode != DEFAULT_MODE){
					puts(PNT_RED"NOTE: for this option there is no skipping MP3 paths since the cover art option is selected."PNT_RESET);
					sendVideo = askUserForPath(MP4_MODE, ALLOW_SKIPS);
					sendAudio = askUserForPath(MP3_MODE, DISALLOW_SKIPS);
				}else{
					int valid = 0;
					do{
						sendVideo = askUserForPath(MP4_MODE, ALLOW_SKIPS);
						sendAudio = askUserForPath(MP3_MODE, ALLOW_SKIPS);

						if((strcmp(sendAudio, "SKIP") == 0 && strcmp(sendVideo, "SKIP") == 0)){
							puts(PNT_RED"Both destinations can not be skipped!"PNT_RESET);
						}else{
							valid = 1;
						}
					}while(!valid);
				}

				//check if skipped entirely
				if(sendAudio == NULL) printError(EXIT_FAILURE, SKIP_VALID_MSG);
				if(sendVideo == NULL) printError(EXIT_FAILURE, SKIP_VALID_MSG);

				//reset the modes
				modeInfo.downloadMode = DEFAULT_MODE;
				modeInfo.coverMode = coverArtMode;

				//reset movementPackage
				getModeFromSelection(sendVideo, sendAudio, &modeInfo, &moveFunction);
				movementInfo = (MovePackage){id, sendAudio, sendVideo, modeInfo.coverMode, coverArt};
			}
		}while(repeat);

	//file execution
	}else{
		FILE* inFile = fopen(argv[fileFlagIndex], "r");
		if(inFile == NULL) printError(EXIT_FAILURE, FILE_FAIL_MSG);

		//normal file execution
		//it's basically default execution with line parsing
		if(coverArtMode == DEFAULT_MODE){
			char urlBuffer [YT_URL_BUFFER] = "";

			while(exactInput(inFile, urlBuffer, YT_URL_BUFFER) != 0){
				snprintf(movementInfo.id, ID_BUFFER, "%s", strstr(urlBuffer, "?v=") + 3);
				//adding a sleep so it doesn't rapid fire youtube
				sleep(1);
				downloadFromURL(urlBuffer, modeInfo.downloadMode, coverArtMode);
				(*moveFunction)(&movementInfo);
			}

		//file mode and cover art mode
		}else{
			char* buffer = NULL;
			while(unknownInput(inFile, &buffer) != 0){
				if(strstr(buffer, YOUTUBE_URL) != NULL){
					char shortenedURL [YT_URL_BUFFER] = "";

					//snprintf was complaining about truncation and
					//I didn't want compliation warnings
					strncpy(shortenedURL, buffer, YT_URL_BUFFER);
					shortenedURL[YT_URL_BUFFER - 1] = '\0';

					snprintf(movementInfo.id, ID_BUFFER, "%s", strstr(shortenedURL, "?v=") + 3);

					//adding a sleep so it doesn't rapid fire youtube
					sleep(1);
					downloadFromURL(shortenedURL, modeInfo.downloadMode, NO_DWNLD_COVER_ART);
					(*moveFunction)(&movementInfo);
				}else if(strstr(buffer, ".mp3") != NULL){
					if(checkIfExists(buffer)){
						//change this later
						if(coverArt == NULL){
							moveFile(buffer, sendAudio);
						}else{
							writeCover(buffer, coverArt);
							moveFile(buffer, sendAudio);
						}
					}else{
						printf(PNT_RED"can't find .mp3 %s via its path\n"PNT_RESET, buffer);
					}
				}else{
					printf(PNT_RED"Line obtained is not a youtube URL or .MP3-> %s\n"PNT_RESET, buffer);
				}
				free(buffer);
				buffer = NULL;
			}
		}

		free(sendVideo);
		free(sendAudio);
		sendAudio = NULL;
		sendVideo = NULL;
	}

	free(MP3_BASE_DIR);
	free(MP4_BASE_DIR);
	return 0;
}
