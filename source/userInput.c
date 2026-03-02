#include "../includes/userInput.h"
#include "../includes/fileOps.h"
#include "../includes/databaseOps.h"

/*
*	 helper method to flush the stream to the next line.
*	 This does not use fflush because it is meant for output streams not input,
*	 this should be used after user input is taken
*	 To avoid having to press enter twice consider where the new line character
*	 is when parsing the input
*
*	stream: file stream to take input from
*/
static void clearLine(FILE* stream){
	int data = '\0';
	while ((data = getc(stream)) != '\n' && data != EOF) { }

	if(ferror(stream)){
		PRINT_ERROR("Encountered a stream error while clearing the file/stream");
		exit(EXIT_FAILURE);
	}
}

/*
*   Validates the id segment of a url.
*   The actual ID portion is passed in since that gives more control, and the
*   caller gives what they believe is the id segment.
*
*   id_segment: character array of YT_ID_LEN + 1 size
*/
static enum INPUT validIDPortion(const char* id_segment){
    assert(id_segment != NULL);
    assert(strlen(id_segment) == YT_ID_LEN);

    const char id_white_list [] =
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789"
        "_-";

    enum INPUT is_good;
    if(strspn(id_segment, id_white_list) != YT_ID_LEN){
        is_good = INVALID;
    }else{
        is_good = VALID;
    }
    return is_good;
}

/*
*   Validates input as a youtube URL.
*   This doesn't support the mobile version of a yt URL.
*   Checks if a standard URL is present and is at the correct location and
*   not in the middle of the string.
*   Longer urls are acceptable, but anything below the expected length isn't passable.
*   The id segment is also validated to make sure it does even point to a video.
*
*   url: input expected to be a URL
*   url_len: The length of the input taken
*/
static enum INPUT validateURL(const char* url, size_t url_len){
    char* begin_point = NULL;
    if((begin_point = strstr(url, YOUTUBE_URL)) == NULL || begin_point - url != 0){
        ADVISE_USER_FORMAT("'%s' is not a youtube URL. It should look like %sXXXXXXXXXXX", url, YOUTUBE_URL);
        return INVALID;
    }

    if(url_len < YT_URL_INPUT_LEN){
        ADVISE_USER_FORMAT("'%s' Id segement should have 11 characters (?v=XXXXXXXXXXX)", url);
        return INVALID;
    }

    char id [YT_ID_SIZE];
    memcpy(id, url + LEN_BEFORE_ID, YT_ID_LEN);
    id[YT_ID_LEN] = '\0';
    if(validIDPortion(id) == INVALID){
        ADVISE_USER_FORMAT("%s doesn't have a proper youtube ID", url);
        return INVALID;
    }

    return VALID;
}

/*
*   Takes a depth input and removes white space and commas.
*   I dunno what crazy person would need a depth of 1,000, but if they need it
*   at least they can enter a comma.
*   The goal isn't to sanitize, but make it so strtol will handle a more consistent number format.
*
*   return: returns a value < INT_MAX or the value of INF_DEPTH
*/
static int normalizeNumericInput(char* input){
    assert(input != NULL);

    int inplace_insert = 0;
    for(int i = 0; input[i] != '\0'; ++i){
        if(!isspace(input[i]) && input[i] != ','){
            input[inplace_insert++] = input[i];
        }
    }
    input[inplace_insert] = '\0';
    return inplace_insert;
}

/*
*   Takes a path input and sets it to an absolute path.
*
*   return: absolute path without a trailing slash that is allocated on the heap
*/
static char* canonizePath(const char* input, char* result){
    assert(input != NULL);
    assert(result != NULL);

    char* absolute_path = realpath(input, result);

    if(absolute_path == NULL){
        int error = errno;
        switch(error){
            case EACCES: PRINT_FORMAT_ERROR("Path given is not accessible: %s", input); break;
            case ELOOP: PRINT_FORMAT_ERROR("You trying to mess with me by using symlinks?: %s", input); break;
            case ENAMETOOLONG: PRINT_FORMAT_ERROR("Path given is too long. Max length is %d: %s", PATH_MAX, input); break;
            case ENOENT: PRINT_FORMAT_ERROR("Path given does not exist: %s", input); break;
            default: PRINT_ERROR("System error occured preventing making absolute path"); break;
        }
        return NULL;
    }

    return absolute_path;
}

static void printQuestion(enum CONFIG type){
    assert(type == AUDIO_CONFIG ||
           type == VIDEO_CONFIG ||
           type == COVER_CONFIG);

    switch(type){
        case AUDIO_CONFIG: puts("Where do you want to send audio files?"); break;
        case VIDEO_CONFIG: puts("Where do you want to send video files?"); break;
        case COVER_CONFIG: puts("Where do you want to send cover arts?"); break;
        default: exit(EXIT_FAILURE); break;
    }
}

/*
 * The condional compiler flag is here since --wrap won't work on internal linkage.
 * --wrap is still needed, but now a separate file isn't needed for those
 *  functions that depend on boundedInput.
 *  Unlike the other wrapped functions this shouldn't need to call the original
 *  as that would more than likely go to stdin and lock up testing.
 *  Additionally --wrap should still selectively choose what to wrap.
 */
#ifndef PREVENT_INTERNAL_LINKAGE
size_t boundedInput(FILE* stream, char* dest, size_t dest_size, enum FOUND_END* is_found){
    assert(stream != NULL);
    assert(dest != NULL);
    assert(dest_size > 0);

    //fgets is normally the better choice for taking user input,
    //However, On input where it's just EOF, length it can't be determined if previous input wasn't overwritten.
    //As a result it'll return the length of what was there before

    size_t amount_written = 0;
    int data = '\0';
    enum IS_END {NO = 0, YES};
    enum IS_END found_end = NO;
    while(!found_end && amount_written < dest_size - 1){
        data = fgetc(stream);
        switch(data){
            case '\n': found_end = YES; break;
            case EOF:
                if(ferror(stream)){
                    PRINT_ERROR("Encountered an error reading stream for input!\n Input is a core function of the program thus this is unrecoverable and the program will exit.");
                    exit(EXIT_FAILURE);
                }
                found_end = YES;
            break;
            default: dest[amount_written++] = data; break;
        }
    }

    if(!found_end){
        if(is_found != NULL) *is_found = NOT_PRESENT;

        clearLine(stream);
    }else{
        if(is_found != NULL) *is_found = WAS_PRESENT;
    }

    dest[amount_written] = '\0';
    return amount_written;
}
#else
size_t boundedInput(FILE* stream, char* dest, size_t dest_size, enum FOUND_END* is_found){
    return __wrap_boundedInput(stream, dest, dest_size, is_found);
}
#endif

char* takeDirectoryInput(void){
    //realpath uses PATH_MAX
    char input [PATH_MAX];
    char* absolute_path = calloc(sizeof(char), PATH_MAX);
    int length = 0;

    puts("Enter the path you want:");
    enum FOUND_END newline;
    length = boundedInput(stdin, input, sizeof(input), &newline);

    if(length == 0) return absolute_path;

    if(newline == NOT_PRESENT){
        ADVISE_USER("Path entered is too large");
        goto bad_dir;
    }

    if(input[0] == '~'){
        ADVISE_USER("Sorry, usage of ~ is a shell expansion");
        goto bad_dir;
    }

    //traling slashes can apparenlty mask a link if it points to a dir due to POSIX
    if(input[length - 1] == '/'){
        input[length - 1] = '\0';
    }

    if(checkDirPath(input) == INVALID) goto bad_dir;

    //For secure input canonicalization should be the first step
    //but here I don't want to canonize end point links
    if(canonizePath(input, absolute_path) == NULL){
        PRINT_ERROR("Couldn't make an absolute path:");
        goto bad_dir;
    }

    return absolute_path;

    bad_dir:
    free(absolute_path);
    absolute_path = NULL;
    return NULL;
}

int takeIndexInput(int max_index){
    puts("Enter the index you want. If you want to skip, only press enter:");

    char index_input [15] = "";

    long index = SKIPPING;
    enum FOUND_END newline;
    if(boundedInput(stdin, index_input, sizeof(index_input), &newline) == 0){
        return index;
    }else if(newline == NOT_PRESENT){
        ADVISE_USER("Input given is too large");
        return INVALID;
    }

    if(normalizeNumericInput(index_input) == 0) return INVALID;

    index = VALID;
    errno = 0;
    char* non_digit = NULL;
    index = strtol(index_input, &non_digit, 10);
    if(*non_digit != '\0'){
        ADVISE_USER("This isn't an index.");
        index = INVALID;
    }else{
        if(errno == ERANGE || index > INT_MAX){
            ADVISE_USER("Index given is too large.");
            index = INVALID;
        }else if(index < 0){
            ADVISE_USER("Index can't be negative.");
            index = INVALID;
        }else if(index == 0){
            ADVISE_USER("Index can't be zero. Index starts at 1.");
            index = INVALID;
        }else if(index > max_index){
            ADVISE_USER("Index is larger than what is available.");
            index = INVALID;
        }
    }

    return index;
}

int takeDepthInput(void){
    char depth_input [15] = "";
    long depth = 0;

    (void)printf("What depth do you want for the path? If depth is not a concern enter %s:\n", INF_STRING);

    enum FOUND_END newline;
    if(boundedInput(stdin, depth_input, sizeof(depth_input), &newline) == 0){
        return SKIPPING;
    }else if(newline == NOT_PRESENT){
        ADVISE_USER("Input entered is too large");
        return INVALID;
    }

    if(normalizeNumericInput(depth_input) == 0) return INVALID;

    errno = 0;
    char* non_digit = NULL;
    depth = strtol(depth_input, &non_digit, 10);
    if(*non_digit != '\0'){
        if(strcasecmp(depth_input, INF_STRING) == 0){
            depth = INF_DEPTH;
        }else{
            ADVISE_USER("Use 'INF' or enter digits");
            depth = INVALID;
        }
    }else{
        if(errno == ERANGE || depth > MAX_DEPTH){
            ADVISE_USER("Depth given is too large");
            depth = INVALID;
        }

        if(depth < 0){
            ADVISE_USER("Depth can't be negative");
            depth = INVALID;
        }
    }

    return depth;
}

//YT urls that have stuff like the radio or playlist in the URL is fine
//since the only important part is the URL and the ID.
enum INPUT getURLFromUser(char* ret_url){
    assert(ret_url != NULL);

    (void)printf("Enter the youtube URL that you want to download -> ");

    enum INPUT is_good_input = INVALID;
    size_t len = boundedInput(stdin, ret_url, YT_URL_INPUT_SIZE, NULL);
    if(len != 0){
        is_good_input = validateURL(ret_url, len);
    }

    return is_good_input;
}

enum REPEAT askToRepeat(void){
    (void)printf("Do you want to download more? Y/N: ");
    char input_buf [4] = "";

    int user_input_len = boundedInput(stdin, input_buf, sizeof(input_buf), NULL);

    enum REPEAT wants_to = ASK_AGAIN;
    switch(user_input_len){
        case 1:
            switch(input_buf[0]){
                case 'y': case 'Y': wants_to = REPEAT; break;
                case 'n': case 'N': wants_to = NO_REPEAT; break;
                default: break;
            }
        break;
        case 2:
            if(strcasecmp(input_buf, "no") == 0) wants_to = NO_REPEAT;
        break;
        case 3:
            if(strcasecmp(input_buf, "yes") == 0) wants_to = REPEAT;
        break;
        default: break;
    }

    return wants_to;
}

enum CONFIG getConfigToEdit(const char* input){
    assert(input != NULL);

    size_t input_length = strlen(input);
    if(input_length == 1){
        switch(*input){
            case 'A': case 'a': return AUDIO_CONFIG; break;
            case 'V': case 'v': return VIDEO_CONFIG; break;
            case 'C': case 'c': return COVER_CONFIG; break;
            case 'B': case 'b': return BLACK_CONFIG; break;
            default: return -1; break;
        }
    }else if(input_length == OPTION_LEN){
        if(strcasecmp(input, AUDIO_STRING) == 0){
            return AUDIO_CONFIG;
        }else if(strcasecmp(input, VIDEO_STRING) == 0){
            return VIDEO_CONFIG;
        }else if(strcasecmp(input, COVER_STRING) == 0){
            return COVER_CONFIG;
        }else if(strcasecmp(input, BLACK_STRING) == 0){
            return BLACK_CONFIG;
        }else{
            return NOT_A_CONFIG;
        }
    }else{
        return NOT_A_CONFIG;
    }
}

int getUserChoiceForDir(enum CONFIG type){
    assert(type == AUDIO_CONFIG ||
           type == VIDEO_CONFIG ||
           type == COVER_CONFIG);

    int num_of_rows = getNumOfPathRowsForConfig(type);
    printf("Rows count %d\n",num_of_rows);
    if(num_of_rows == 0){
        switch(type){
            case AUDIO_CONFIG: ADVISE_USER("Nothing to be selected for AUDIOs"); break;
            case VIDEO_CONFIG: ADVISE_USER("Nothing to be selected for VIDEOs"); break;
            case COVER_CONFIG: ADVISE_USER("Nothing to be selected for COVERs"); break;
            default: assert(300 != 300); break;
        }
        return SKIPPING;
    }

    int index = 0;
    int take_input = 1;
    while(take_input){
        listConfigRootsWithPaths(type);
        printQuestion(type);

        //Dirty but it'll have to do
        #ifndef PREVENT_INTERNAL_LINKAGE
        index = takeIndexInput(num_of_rows);
        #else
        index = __wrap_takeIndexInput(num_of_rows);
        #endif
        if(index != INVALID) take_input = 0;
    }

    int path_id = (index != SKIPPING) ? translatePathIndexToRow(index, type) : SKIPPING;

    return path_id;
}

size_t sanitizeMetaString(char* meta_arg){
    assert(meta_arg != NULL);

    //not really sure of every character that can cause issues,
    //, but you know that's the point of a white list.
    static const char white_list [] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789"
        " ?!@#$%^&*_-~+=.<>|";

    size_t inplace_insert = 0;
    for(int i = 0; meta_arg[i] != '\0'; ++i){
        if(strchr(white_list, meta_arg[i]) != NULL){
            meta_arg[inplace_insert++] = meta_arg[i];
        }else{
            ADVISE_USER_FORMAT("Detected invalid character for meta data: '%c'. This will be removed.", meta_arg[i]);
        }
    }
    meta_arg[inplace_insert] = '\0';
    return inplace_insert;
}

//I am aware that if a metatag being added contains a colon then it'll stop
//at the colon since I have no form of escaping it.
enum FILE_INPUT readFileLine(FILE* list, char* url_buffer, MetaData_t* data){
    char file_line [FILE_LINE_BUF_SIZE];
    enum FOUND_END end_line;
    size_t line_len = boundedInput(list, file_line, FILE_LINE_BUF_SIZE, &end_line);
    if(line_len == 0){
        if(feof(list) != 0){
            return DONE;
        }else{
            return BAD_LINE;
        }
    }

    if(end_line == NOT_PRESENT) return BAD_LINE;

    enum FILE_INPUT is_valid = BAD_LINE;
    if(validateURL(file_line, line_len) == VALID){
        memcpy(url_buffer, file_line, YT_URL_INPUT_LEN);
        url_buffer[YT_URL_INPUT_LEN] = '\0';
        enum OPTIONS {GENRE = 0, ARTIST, ALBUM};
        enum OPTIONS separator_count = GENRE;
        char* separator_pos = strchrnul(file_line, '|');

        while(*separator_pos != '\0' && separator_count <= ALBUM){
            char* colon_pos = strchrnul(separator_pos + 1, ':');
            size_t separation_len = colon_pos - separator_pos - 1;
            if(separation_len > 0){
                char* new_meta;
                switch(separator_count){
                    case GENRE:  new_meta = data->genre;  break;
                    case ARTIST: new_meta = data->artist; break;
                    case ALBUM:  new_meta = data->album;  break;
                }

                size_t copy_len = separation_len;
                if(separation_len > META_LEN){
                    WARN_USER_FORMAT("Metadata in file is longer than %d characters it will be truncated", META_LEN);
                    copy_len = META_LEN;
                }

                memcpy(new_meta, separator_pos + 1, copy_len);
                new_meta[copy_len] = '\0';
                sanitizeMetaString(new_meta);
            }

            assert(separator_count <= ALBUM);
            ++separator_count;
            separator_pos = colon_pos;
        }
        is_valid = GOOD_LINE;
    }

    return is_valid;
}
