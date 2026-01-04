#include "../includes/databaseUserInput.h"

/*
*   Takes input a depth input and removes white space and commas.
*   I dunno what crazy person would need a depth of 1,000, but if they need it
*   at least they can enter a comma.
*   The goal isn't to sanitize, but make it so strtol will handle a more consistent number format.
*
*   return: returns a value < INT_MAX or the value of INF_DEPTH
*/
static void normalizeDepthInput(char* input){
    int inplace_insert = 0;
    for(int i = 0; input[i] != '\0'; ++i){
        if(!isspace(input[i]) && input[i] != ','){
            input[inplace_insert++] = input[i];
        }
    }
    input[inplace_insert] = '\0';
}

int takeDepthInput(void){
    char depth_input [15] = "";
    long depth = 0;

    printf("What depth do you want for the path? If depth is not a concern enter %s:", INF_STRING);

    if(boundedInput(stdin, depth_input, sizeof(depth_input)) == 0){
        ADVISE_USER("Enter something");
        depth = INVALID;
    }

    normalizeDepthInput(depth_input);

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

    puts("Enter the path you want:");
    if(boundedInput(stdin, input, sizeof(input)) == 0){
        ADVISE_USER("You need to enter something\n");
        return NULL;
    }
    char* absolute_path = realpath(input, NULL);

    if(absolute_path == NULL){
        perror("Couldn't make an absolute path:");
        return NULL;
    }

    if(checkDirPath(absolute_path) == INVALID){
        free(absolute_path);
        absolute_path = NULL;
        return NULL;
    }

    return absolute_path;
}
