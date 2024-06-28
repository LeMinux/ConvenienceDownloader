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
 * !!!!!Change main to handle errors in the case downloadFromURL fails
 * For file execution if errors occur try to log what URLS or paths were ignored
 *	instead of just crashing the program which results in manual cleansing of the file
 * Fix bug that creates two // in getting sub dirs
 * * Fix how typing the dests is. Is it really necessary to type the entire path?
 *	perhaps replace the linked list with an array. This would making it a lot easier if the user inputs a number to select
 * Have a way to specify multiple destinations with -w4 -w3 and -wc
 * Add cover art related things such as keeping or discarding them
*/

/*TODO
 * For file execution think about adding a way to change destinations mid-way.
 *	(!3>../Bangers/Extreme)
 *	(!4>../Bangers/Extreme)
 *	(!c>../Bangers/Extreme)
 * Adding NULL protection in writeCover()?
 * mv command or rename function?
 * perhaps have a way to add more metadata to mp3 files?
*/

/*CONTINUOUS
 * Use program :P
*/

#include "../includes/globals.h"
#include "../includes/userInput.h"
#include "../includes/helpers.h"
#include "../includes/writeArt.h"
#include "../includes/fileOps.h"
#include "../includes/pathMap.h"

#define W4_NOT_GIVEN "Destination to send video does not exist. Please specify where to send them with the -w4 flag"
#define W3_NOT_GIVEN "Destination to send audio does not exist. Please specify where to send them with the -w3 flag\n"
#define NO_PERMISSION "Writting privleges for the given path is not availiable\n"

#define USER_MP4_PROMPT "Where do you want to send the MP4? Type exit to leave or type skip to avoid sending files: "
#define USER_MP3_PROMPT "Where do you want to send the MP3? Type exit to leave or type skip to avoid sending files: "
#define USER_MP3_PROMPT_NO_SKIP "Where do you want to send the MP3? Type exit to leave: "
#define USER_MP4_PROMPT_NO_SKIP "Where do you want to send the MP4? Type exit to leave: "

#define DES "./Destinations/"
#define DES_MP4 "./Destinations/videoDest.txt"
#define DES_MP3 "./Destinations/audioDest.txt"
#define DES_COVER "./Destinations/coverDest.txt"

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

#define DEST_AMOUNT 3
#define MP4_INDEX 0
#define MP3_INDEX 1
#define COVER_INDEX 2

#define ID_BUFFER 12

//this struct is here so that function pointers can be used for moving files
//this helps generalize the code a little, which does make it less efficient,
//but it makes changes much easier
typedef struct MovePackage{
	char* id;
	char* audioDest;
	char* videoDest;
	char* coverDest;
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
static MapArray_t* destMaps = NULL;

void __attribute__((constructor)) initPaths (){
	if(!checkIfExists(DES)){
		PRINT_ERROR("Destinations directory is not present. Please run the makefile with \"make\"");
		exit(EXIT_FAILURE);
	}
	int valid = 1;
	if(!checkIfExists(DES_MP4)){
		PRINT_ERROR("Destinations for videos have not been initalized.\n Please use the -w4 flag to specify them");
		valid = 0;
	}

	if(!checkIfExists(DES_MP3)){
		PRINT_ERROR("Destinations for audio have not been initalized.\n Please use the -w3 flag to specify them");
		valid = 0;
	}

	if(!checkIfExists(DES_COVER)){
		PRINT_ERROR("Destinations for covers have not been initalized.\n Please use the -wc flag to specify them");
		valid = 0;
	}

	if(!valid){
		PRINT_ERROR("Use the corresponding flags to add the directories");
		exit(EXIT_FAILURE);
	}

	//making an array so that using a loop is easier
	FILE* destFiles [] = {NULL, NULL, NULL};
	destFiles[0] = fopen(DES_MP4, "r");
	destFiles[1] = fopen(DES_MP3, "r");
	destFiles[2] = fopen(DES_COVER, "r");

	if(destFiles[0] == NULL) printError(EXIT_FAILURE, "Constructor Failed to open video destination file");
	if(destFiles[1] == NULL) printError(EXIT_FAILURE, "Constructor Failed to open audio destination file");
	if(destFiles[2] == NULL) printError(EXIT_FAILURE, "Constructor Failed to open cover destination file");

	destMaps = malloc(sizeof(*destMaps) * DEST_AMOUNT);
	if(destMaps == NULL){
		PRINT_ERROR(FAILED_MALLOC_MSG);
		exit(EXIT_FAILURE);
	}

	//build a map per each directory entry in the destination files
	char* buffer = NULL;
	int index = 0;
	for(; index < DEST_AMOUNT; ++index){
		int dirCount = 0;
		while(unknownInput(destFiles[index], &buffer) != 0){
			destMaps[index].mapArray = realloc(destMaps[index].mapArray, sizeof(Map_t) * dirCount + 1);
			if(destMaps[index].mapArray == NULL){
				PRINT_ERROR(FAILED_MALLOC_MSG);
				exit(EXIT_FAILURE);
			}

			destMaps[index].mapArray[dirCount] = *(obtainPathMap(buffer));
			++dirCount;
		}
		destMaps[index].length = dirCount;
		fclose(destFiles[index]);
	}
}

//mode specifies audio or video
//for skipping, 0 is false and 1 is true
static char* askUserForPath(int mode, int allowSkipping){
	const char* prompt = NULL;
	int index = 0;

	switch(mode){
		case MP3_MODE:
			index=MP3_INDEX;
			if(allowSkipping) prompt = USER_MP3_PROMPT;
			else prompt = USER_MP3_PROMPT_NO_SKIP;
		break;

		case MP4_MODE:
			index=MP4_INDEX;
			if(allowSkipping) prompt = USER_MP4_PROMPT;
			else prompt = USER_MP4_PROMPT_NO_SKIP;
		break;

		default:
			PRINT_ERROR("Client passed invalid mode for getting path");
			exit(EXIT_FAILURE);
		break;
    }
	return getSelection(&destMaps[index], prompt, allowSkipping);
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

static void moveVideo (MovePackage* movingInfo){
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
			(void)puts("v List of availiable directories for MP4");
			printMapArray(&destMaps[MP4_INDEX]);

			(void)puts("\nv List of availiable directories for MP3");
			printMapArray(&destMaps[MP3_INDEX]);

			(void)puts("\nv List of availiable directories for Cover Arts");
			printMapArray(&destMaps[COVER_INDEX]);

			exit(EXIT_SUCCESS);
		}else if(strcmp("-f", argv[c]) == 0){
			if(!checkIfExists(argv[c + 1])) printError(EXIT_FAILURE, FILE_FAIL_MSG);

			fileMode = 1;
			fileFlagIndex = ++c;

		//where to send mp4(video) files
		}else if(strcmp("-w4", argv[c]) == 0 || strcmp("-w3", argv[c]) == 0 || strcmp("-wc", argv[c]) == 0){
			if(argc == c + 1){
				PRINT_ERROR("Did not give a list of paths");
				exit(EXIT_FAILURE);
			}

			FILE* destFile = NULL;
			char* prompt = NULL;

			//clear file
			switch(argv[c][2]){
				case '4':
					destFile = fopen(DES_MP4, "w");
					prompt = GREEN "Successfully written where to send video files" RESET;
				break;
				case '3':
					destFile = fopen(DES_MP3, "w");
					prompt = GREEN "Successfully written where to send video files" RESET;
				break;
				case 'c':
					destFile = fopen(DES_COVER, "w");
					prompt = GREEN "Successfully written where to send cover arts" RESET;
				break;
				default: PRINT_ERROR("I dunno how this broke"); exit(EXIT_FAILURE);break;
			}

			if(destFile == NULL){
				PRINT_ERROR("Could not open destination file for writing");
				exit(EXIT_FAILURE);
			}

			int canWrite = 1;
			int index = ++c;

			//it would be more efficient to write as it goes, but
			//having partial completion isn't as user friendly
			for(; index < argc; ++index){
				if(validateDirPath(argv[index]) == HAD_ERROR) canWrite = 0;
			}

			if(canWrite){
				for(; c < argc; ++c){
					if(argv[c][strlen(argv[c]) - 1] == '/')
						(void)fprintf(destFile, "%s\n", argv[c]);
					else
						(void)fprintf(destFile, "%s/\n", argv[c]);
				}
				(void)puts(prompt);
				(void)fclose(destFile);
				exit(EXIT_SUCCESS);
			}else{
				PRINT_ERROR("Make sure all paths are valid directories");
				exit(EXIT_FAILURE);
			}
		}else if(strcmp("-ca", argv[c]) == 0){
			if(argc == c + 1){
				PRINT_ERROR("Did not give a valid flag or path");
				exit(EXIT_FAILURE);
			}

			if(strcmp("NO-ART", argv[c + 1]) != 0){
				if(!checkIfExists(argv[c + 1])) printError(EXIT_FAILURE, FILE_FAIL_MSG);
			}

			coverArtMode = NO_DWNLD_COVER_ART;
			coverArtIndex = c + 1;
			++c;
		}else if(strcmp("--keep-art", argv[c]) == 0){
			keepArt = 1;
		}else{
			printf(RED "Invalid argument: \"%s\"\n" RESET, argv[c]);
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
		(void)puts(PNT_RED"NOTE: for this option there is no skipping MP3 paths since the cover art option is selected."PNT_RESET);
		sendVideo = askUserForPath(MP4_MODE, ALLOW_SKIPS);
		sendAudio = askUserForPath(MP3_MODE, DISALLOW_SKIPS);
	}else{
		int valid = 0;
		do{
			sendVideo = askUserForPath(MP4_MODE, ALLOW_SKIPS);
			sendAudio = askUserForPath(MP3_MODE, ALLOW_SKIPS);

			if((strcmp(sendAudio, "SKIP") == 0 && strcmp(sendVideo, "SKIP") == 0)){
				(void)puts(PNT_RED"Both destinations can not be skipped!"PNT_RESET);
			}else{
				valid = 1;
			}
		}while(!valid);
	}

	//check if skipped entirely
	if(sendAudio == NULL) printError(EXIT_FAILURE, SKIP_VALID_MSG);
	if(sendVideo == NULL) printError(EXIT_FAILURE, SKIP_VALID_MSG);

	getModeFromSelection(sendVideo, sendAudio, &modeInfo, &moveFunction);
	movementInfo = (MovePackage){id, sendAudio, sendVideo, NULL, modeInfo.coverMode, coverArt};

	//default and cover art mode
	if(!fileMode){
		int repeat = 0;
		do{
			char url [YT_URL_INPUT_SIZE] = "";
			//execution for downloading
			getURL(url);

			//snprintf will rewrite the id string
			snprintf(movementInfo.id, ID_BUFFER, "%s", strstr(url, "?v=") + 3);

			if(downloadFromURL(url, modeInfo.downloadMode, modeInfo.coverMode) == NO_ERROR){
				(*moveFunction)(&movementInfo);
			}

			sendAudio = NULL;
			sendVideo = NULL;

			if((repeat = askToRepeat()) == 1){
				if(coverArtMode != DEFAULT_MODE){
					(void)puts(PNT_RED"NOTE: for this option there is no skipping MP3 paths since the cover art option is selected."PNT_RESET);
					sendVideo = askUserForPath(MP4_MODE, ALLOW_SKIPS);
					sendAudio = askUserForPath(MP3_MODE, DISALLOW_SKIPS);
				}else{
					int valid = 0;
					do{
						sendVideo = askUserForPath(MP4_MODE, ALLOW_SKIPS);
						sendAudio = askUserForPath(MP3_MODE, ALLOW_SKIPS);

						if((strcmp(sendAudio, "SKIP") == 0 && strcmp(sendVideo, "SKIP") == 0)){
							(void)puts(PNT_RED"Both destinations can not be skipped!"PNT_RESET);
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
				movementInfo = (MovePackage){id, sendAudio, sendVideo, NULL, modeInfo.coverMode, coverArt};
			}
		}while(repeat);

	//file execution
	}else{
		FILE* inFile = fopen(argv[fileFlagIndex], "r");
		FILE* logFile = fopen("FailedDownloads.txt", "w");
		if(inFile == NULL){
			PRINT_ERROR("Failed to open file given for file downloading");
			exit(EXIT_FAILURE);
		}
		if(logFile == NULL){
			PRINT_ERROR("Failed to create log file for logging errors");
			exit(EXIT_FAILURE);
		}
		int logsWritten = 0;

		//normal file execution
		//it's basically default execution with line parsing
		if(coverArtMode == DEFAULT_MODE){
			char urlBuffer [YT_URL_INPUT_SIZE] = "";
			while(exactInput(inFile, urlBuffer, YT_URL_INPUT_SIZE) != 0){
				snprintf(movementInfo.id, ID_BUFFER, "%s", strstr(urlBuffer, "?v=") + 3);
				//adding a sleep so it doesn't rapid fire youtube
				sleep(1);
				if(downloadFromURL(urlBuffer, modeInfo.downloadMode, coverArtMode) == NO_ERROR){
					(*moveFunction)(&movementInfo);
				}else{
					(void)printf("Adding URL %s to log file\n", urlBuffer);
					(void)fprintf(logFile ,"Failed to download from url: %s\n", urlBuffer);
					++logsWritten;
				}
			}

		//file mode and cover art mode
		}else{
			char* buffer = NULL;
			while(unknownInput(inFile, &buffer) != 0){
				if(strstr(buffer, YOUTUBE_URL) != NULL){
					char shortenedURL [YT_URL_INPUT_SIZE] = "";

					//snprintf was complaining about truncation and
					//I didn't want compliation warnings
					strncpy(shortenedURL, buffer, YT_URL_INPUT_SIZE);
					shortenedURL[YT_URL_INPUT_SIZE - 1] = '\0';

					snprintf(movementInfo.id, ID_BUFFER, "%s", strstr(shortenedURL, "?v=") + 3);

					//adding a sleep so it doesn't rapid fire youtube
					sleep(1);
					if(downloadFromURL(shortenedURL, modeInfo.downloadMode, NO_DWNLD_COVER_ART) == NO_ERROR){
						(*moveFunction)(&movementInfo);
					}else{
						(void)printf("Adding URL %s to log file\n", shortenedURL);
						(void)fprintf(logFile ,"Failed to download from url: %s\n", shortenedURL);
						++logsWritten;
					}
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
						(void)printf(PNT_RED"can't find .mp3 %s via its path\n"PNT_RESET, buffer);
						(void)fprintf(logFile, "Failed to find file path: %s\n", buffer);
						++logsWritten;
					}
				//check if buffer has a swap tag
				}else if(strnlen(buffer, 3) == 3 && buffer[0] == '!' && buffer[2] == '>'){
					char* newDest = NULL;
					int offset = 0;
					switch(buffer[1]){
						case '4':
							newDest = findPath(&destMaps[MP4_INDEX], buffer + 3);
							offset = 1;
						break;
						case '3':
							newDest = findPath(&destMaps[MP3_INDEX], buffer + 3);
							offset = 2;
						break;
						case 'c': case 'C':
							findPath(&destMaps[COVER_INDEX], buffer + 3);
							offset = 3;
						break;
						default:
							//exiting is done here to be more user friendly
							//their intention is to move to a new place, but an error stops that
							(void)printf(PNT_RED"Invalid tag to change destinations exiting program: %s\n"PNT_RESET, buffer);
							(void)fprintf(logFile, "Invalid tag used (it should be ![4,3, or c]>: %s\n", buffer);
							exit(EXIT_FAILURE);
						break;
					}

					//pointer math to set the string address directly
					//into the struct to avoid more conditionals
					//exiting is done here to be more user friendly
					//their intention is to move to a new place, but an error stops that
					if(newDest != NULL && checkIfExists(buffer + 3)){
						*((char**)(&movementInfo) + offset) = newDest;
					}else{
						(void)printf(PNT_RED"Path specified does not exist or is not a path from Destinations exiting program: %s\n"PNT_RESET, buffer);
						(void)fprintf(logFile, "Path does not exist or can not be found from what is given in Destinations\nUse the -l flag to know what are availiable: %s\n", buffer);
						exit(EXIT_FAILURE);
					}
				}else{
					(void)printf(PNT_RED"Line obtained is not a youtube URL or .MP3: %s\n"PNT_RESET, buffer);
					(void)fprintf(logFile, "String is not a youtube URL or mp3 path: %s\n", buffer);
					++logsWritten;
				}
				free(buffer);
				buffer = NULL;
			}
		}
		fclose(inFile);
		fclose(logFile);
		if(logsWritten == 0)system("rm FailedDownloads.txt");
		else PRINT_ERROR("Failed to download some of the specified lines. Check FailedDownloads.txt to see which ones.");

		sendAudio = NULL;
		sendVideo = NULL;
	}

	//freeing destMaps would go here, but it's going to exit anyway
	return 0;
}
