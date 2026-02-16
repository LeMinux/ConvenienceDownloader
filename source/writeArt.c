#include "../includes/writeArt.h"

enum ERROR writeCover(const char* file_with_path, const char* cover_path){
    assert(file_with_path != NULL);
    assert(cover_path != NULL);

    FILE* file = openFile(file_with_path, "r");
    if(file == NULL){
        PRINT_ERROR("Could not open temp file to get file name");
        return HAD_ERROR;
    }

    char* file_path = NULL;
    size_t path_len = 0;
    int str_len = getline(&file_path, &path_len, file);
    if(str_len < 0){
        PRINT_ERROR("Could not get file name from temp file");
        free(file_path);
        (void)fclose(file);
        (void)remove(file_with_path);
        return HAD_ERROR;
    }

    if(file_path[str_len - 1] == '\n') file_path[str_len - 1] = '\0';

    char* command_arguments [] ={
        "ffmpeg",
        "-xerror",
        "-y",
        "-i", file_path,
        "-i", cover_path,
        "-map", "0:0",
        "-map", "1:0",
        "-codec", "copy",
        TEMP_FILE,
        NULL
    };

    enum ERROR status = execProgram("/usr/bin/ffmpeg", command_arguments);
    if(status == NO_ERROR){
        char* mv_command_args [] = {
            "mv",
            "-f",
            TEMP_FILE,
            file_path,
            NULL
        };
        status = execProgram("/usr/bin/mv", mv_command_args);
    }

    free(file_path);
    (void)fclose(file);
    (void)remove(file_with_path);
    (void)remove(TEMP_FILE);
    return status;
}
