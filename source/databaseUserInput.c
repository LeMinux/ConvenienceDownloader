#include "../includes/databaseUserInput.h"

/*
*   Takes a depth input and removes white space and commas.
*   I dunno what crazy person would need a depth of 1,000, but if they need it
*   at least they can enter a comma.
*   The goal isn't to sanitize, but make it so strtol will handle a more consistent number format.
*
*   return: returns a value < INT_MAX or the value of INF_DEPTH
*/
static void normalizeNumericInput(char* input){
    assert(input != NULL);

    int inplace_insert = 0;
    for(int i = 0; input[i] != '\0'; ++i){
        if(!isspace(input[i]) && input[i] != ','){
            input[inplace_insert++] = input[i];
        }
    }
    input[inplace_insert] = '\0';
}

/*
*   Takes a path input and sets it to an absolute path.
*
*   return: absolute path without a trailing slash
*/
static char* canonizePath(const char* input){
    assert(input != NULL);

    char* absolute_path = realpath(input, NULL);

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

int takeDepthInput(void){
    char depth_input [15] = "";
    long depth = 0;

    printf("What depth do you want for the path? If depth is not a concern enter %s:", INF_STRING);

    if(boundedInput(stdin, depth_input, sizeof(depth_input)) == 0){
        ADVISE_USER("Enter something");
        depth = INVALID;
    }

    normalizeNumericInput(depth_input);

    if(depth != INVALID){
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
            if(errno == ERANGE || depth > INT_MAX){
                ADVISE_USER("Depth given is too large");
                depth = INVALID;
            }

            if(depth < 0){
                ADVISE_USER("Depth can't be negative");
                depth = INVALID;
            }
        }
    }

    return depth;
}

char* takeDirectoryInput(void){
    //realpath uses PATH_MAX
    char input [PATH_MAX];
    size_t length = 0;

    puts("Enter the path you want:");
    length = boundedInput(stdin, input, sizeof(input));

    if(length == 0){
        ADVISE_USER("You need to enter something\n");
        return NULL;
    }

    if(input[0] == '~'){
        ADVISE_USER("Sorry, usage of ~ is a shell expansion");
        return NULL;
    }

    //traling slashes can apparenlty mask a link if it points to a dir
    if(input[length - 1] == '/'){
        input[length - 1] = '\0';
    }

    if(checkDirPath(input) == INVALID){
        return NULL;
    }

    char* absolute_path = canonizePath(input);

    if(absolute_path == NULL){
        PRINT_ERROR("Couldn't make an absolute path:");
        return NULL;
    }

    return absolute_path;
}

int takeIndexInput(int max_index){
    puts("Enter the index you want to change.");

    char index_input [15] = "";

    if(boundedInput(stdin, index_input, sizeof(index_input)) == 0){
        ADVISE_USER("Enter something");
        return INVALID;
    }

    normalizeNumericInput(index_input);

    long index = 0;
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
        }

        if(index < 0){
            ADVISE_USER("Index can't be negative.");
            index = INVALID;
        }

        if(index == 0){
            ADVISE_USER("Index can't be zero. Index starts at 1.");
            index = INVALID;
        }

        if(index > max_index){
            ADVISE_USER("Index is larger than was is available.");
            index = INVALID;
        }
    }

    return index;
}
