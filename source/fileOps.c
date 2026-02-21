#include "../includes/fileOps.h"

enum INPUT checkDirPath(const char* dir_path){
    assert(dir_path != NULL);
    assert(dir_path[0] != '\0');

    struct stat file_stat = {0};
    if(lstat(dir_path, &file_stat) < 0){
        int error = errno;
        switch(error){
            case ENOENT: ADVISE_USER("Path given doesn't exist"); break;
            case ENOTDIR: ADVISE_USER("Path given has broken components"); break;
            case EACCES: ADVISE_USER("Can't access path given"); break;
            case ENAMETOOLONG: ADVISE_USER_FORMAT("Path is too long. The limit is %d", PATH_MAX); break;
            default: ADVISE_USER("System limits prevented opening the directory"); break;
        }
        return INVALID;
    }

    if(S_ISLNK(file_stat.st_mode)){
        ADVISE_USER("The end point can't be a link");
        return INVALID;
    }

    if(!(S_ISDIR(file_stat.st_mode))){
        ADVISE_USER("Path given is not a directory");
        return INVALID;
    }

    //yes access can have TOCTOU issues, but this program really doesn't directly
    //handle paths and instead hands it off
    if(access(dir_path, R_OK | W_OK | X_OK)){
        ADVISE_USER("You don't have read, write, and execute permissions for the path");
        return INVALID;
    }

    return VALID;
}

FILE* openFile(const char* file_path, const char* mode){
    assert(file_path != NULL);
    assert(mode != NULL);
    assert(file_path[0] != '\0');
    assert(mode[0] != '\0');

    FILE* open_file = fopen(file_path, mode);
    if(open_file == NULL){
        PRINT_FORMAT_ERROR("Failed to open file: %s", file_path);
        return NULL;
    }

    int fd = fileno(open_file);
    if(fd < 0){
        PRINT_FORMAT_ERROR("Failed to get descriptor of file: %s", file_path);
        goto error_did_open;
    }


    struct stat file_stat = {0};
    if(fstat(fd, &file_stat) != 0){
        PRINT_FORMAT_ERROR("Couldn't get file information on %s\n", file_path);
        goto error_did_open;
    }

    //Directories can be opened with fopen without returning NULL
    if(S_ISREG(file_stat.st_mode)){
        goto success;
    }else{
        PRINT_FORMAT_ERROR("File name given is not a regular file: %s", file_path);
        goto error_did_open;
    }

    error_did_open:
        fclose(open_file);
        return NULL;

    success:
        return open_file;
}
