#include "../includes/testWrapInput.h"

int __wrap_boundedInput(FILE* stream, char* dest, size_t dest_size){
    //don't care about these just want to return a string
    (void)stream;
    (void)dest_size;

    const char* stubbed_string = mock_type(char*);
    int write_len  = mock_type(int);
    memcpy(dest, stubbed_string, write_len);
    dest[write_len] = '\0';
    return write_len;
}

int __wrap_takeIndexInput(int max){
    (void) max;
    function_called();
    if(has_mock()){
        return mock_type(int);
    }else{
        stop();
    }
}

//malloc is used as the path returned is expected to be malloced
char* __wrap_takeDirectoryInput(void){
    function_called();
    if(has_mock()){
        char* ret_string = NULL;
        const char* stubbed_string = mock_type(char*);
        if(stubbed_string != NULL){
            int stubbed_size = strlen(stubbed_string) + 1;
            ret_string = malloc(stubbed_size);
            memcpy(ret_string, stubbed_string, stubbed_size);
        }

        return ret_string;
    }else{
        stop();
    }
}

int __wrap_takeDepthInput(){
    function_called();
    if(has_mock()){
        return mock_type(int);
    }else{
        stop();
    }
}
