#ifndef USERINPUT_H
#define USERINPUT_H

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "globals.h"
#include "fileOps.h"
#include "databaseOps.h"
#include "downloading.h"

//Another NASA sin :(
#ifdef PREVENT_INTERNAL_LINKAGE
    #include "testWrapInput.h"
#endif

#define AUDIO_STRING "audio"
#define VIDEO_STRING "video"
#define COVER_STRING "cover"
#define BLACK_STRING "black"
#define INF_STRING "INF"

#define MAX_DEPTH 2048

#define OPTION_LEN 5

enum REPEAT {ASK_AGAIN = -1, NO_REPEAT, REPEAT};
enum FILE_INPUT {BAD_LINE = -1, DONE, GOOD_LINE};

#include <stdio.h>

/*
*	gets the URL from the user.
*	the parameter is a passed by reference string to place the ID into
*	It's not necessary to carry around the entire string as sometimes there can
*	be really long youtube URLs, and the only thing that's really needed to get
*	to a youtube video is the beginning youtube.com protion and the video ID parameter
*
*	return:
*    VALID for a valid URL INVALID to prompt input again.
*    If input is VALID the video ID is placed into the parameter.
*    Don't try to use the ID if INVALID is passed as this method won't overwrite
*    previous contents so you will use what ever was in ret_id before calling.
*/
enum INPUT getURLFromUser(char* ret_url);

/*
*	downloads a song given the URL for it
*	different modes are specified depending on what is desired
*	preferably this should only be called once per URL to minimize network traffic.
*
*	youtubeURL: the youtubeURL given
*	mode: 3 to specify to download audio 4 to specify to download video
*	downloadCoverArt: 0 to for not downloading thumbnails and 1 to download thumbnails
*
*	return: success or failure
int downloadFromURL(const char* youtubeURL, int mode, int downloadCoverArt);
*/

/*
*	asks the user if they would like to continue downloading
*
*	return: REPEAT for yes NO_REPEAT for no ASK_AGAIN if input is invalid
*/
enum REPEAT askToRepeat(void);


/*
*	method for obtaining user input where size is bounded.
*	Best used for strings on the stack where bounds is known.
*	Input is read until it encounters a newline or EOF.
*	If a newline is encountered it's replaced with a nul byte and length is adjusted.
*	This is meant for c-strings, so a bound of 1 isn't useful as that'll just give you a nul byte.
*
*	stream: file stream which can include stdin
*	dest: destination string and should not be NULL
*	buffer: size of input buffer. This should account for the nul byte like you would for fgets.
*
*   return:
*   length of what was read excluding the nul byte.
*   The amount returned is able to be used to determine the position of the nul byte.
*/
int boundedInput(FILE* stream, char* dest, size_t size);


/*
 * Translate the user's command line argument into a config option
 *
 * input: User's command line input for the -e option
 *
 * return: AUDIO, VIDEO, COVER, BLACK, or NOT_A_CONFIG
 */
enum CONFIG getConfigToEdit(const char* input);

//these below used to be in their own separate file,
//but now they are here

/*
*   Takes input from the user to accept a directory.
*
*   return: NULL on error or a malloced absolute path
*/
char* takeDirectoryInput(void);

/*
*   Function for taking in input for what depth a root path should have.
*   There is no index 0, so index 1 is the first index.
*
*   return: returns a value < INT_MAX or the value of INF_DEPTH or INVALID
*/
int takeDepthInput(void);

/*
*   Function for taking in input for what index a root path is.
*
*   param: max_index specifies what is the maximum index allowed
*
*   return: returns a value < max_index or INVALID
*/
int takeIndexInput(int max_index);

/*
*   Presents the user with an indexed selection for what path to send stuff to
*   The selection is for paths and not roots.
*   The user can opt to skip this choice if they don't need it.
*
*   return:
*       path_id associate with the selection
*       OR
*       SKIPPING if the user wants to skip
*/
int getUserChoiceForDir(enum CONFIG type);

/*
*   Sanitization is needed due to how yt-dlp handles reading OUTPUT TEMPLATES.
*   They have a format of %(<all the options>)s and if the user inputs
*   an output template then yt-dlp crashes trying to double interpret.
*   My guess to why it crashes is because it's trying to access a JSON key:value
*   from a non-JSON such as just the variable or someting.
*   A % is fine to include it's mostly the () that cause problems.
*   semicolons do need to be escaped, but I'm just not going to worry about it and remove them.
*
*   return:
*   meta_arg in-place replaced data removing potentially bad characters
*   size_t return of the new length of meta_arg
*/
size_t sanitizeMetaString(char* meta_arg);


enum FILE_INPUT readFileLine(FILE* list, char* url_buffer, MetaData_t* data);
#endif
