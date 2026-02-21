#include "../includes/execOthers.h"

enum ERROR execProgram(const char* bin_path, char* const command_args []){
    assert(bin_path != NULL);
    assert(command_args != NULL);

    pid_t pid = fork();
    if(pid < 0){
        PRINT_ERROR("failed to fork");
        return HAD_ERROR;
    }

    enum ERROR status = HAD_ERROR;
    if(pid == 0){
        execv(bin_path, command_args);
    }else{
        int child_ret = 0;
        waitpid(pid, &child_ret, 0);
        if(child_ret == 0){
            status = NO_ERROR;
        }
    }

    return status;
}
