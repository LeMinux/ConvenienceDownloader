#include "../includes/testWrapInput.h"

size_t __wrap_boundedInput(FILE* stream, char* dest, size_t dest_size){
    //don't care about these just want to return a string
    (void) stream;
    (void) dest_size;

    const char* stubbed_string = mock_type(char*);
    size_t write_len = strlen(stubbed_string);

    size_t written_amt = 0;
    if(write_len < dest_size){
        memcpy(dest, stubbed_string, write_len);
        dest[write_len] = '\0';
        written_amt = write_len;
    }else{
        memcpy(dest, stubbed_string, dest_size - 1);
        dest[dest_size - 1] = '\0';
        written_amt = dest_size - 1;
    }

    return written_amt;
}

//If a mock is available it will return the mock first.
//the usage of __real_ function is there to determine behavior once the mock
//is consumed or if there is no mock to be had.
//This may be wanted because the below functions also use boundedInput, so if
//the actual behavior of these functions are wanted it can be tested.

int __wrap_takeIndexInput(int max){
    (void) max;
    function_called();
    int ret_int = 0;

    if(has_mock()){
        ret_int = mock_type(int);
    }else{
        stop();
    }
    return ret_int;
}

//malloc is used as the path returned is expected to be malloced
char* __wrap_takeDirectoryInput(void){
    function_called();
    char* ret_string = NULL;

    if(has_mock()){
        const char* stubbed_string = mock_type(char*);
        if(stubbed_string != NULL){
            int stubbed_size = strlen(stubbed_string) + 1;
            ret_string = malloc(stubbed_size);
            memcpy(ret_string, stubbed_string, stubbed_size);
        }
    }else{
        stop();
    }

    return ret_string;
}

int __wrap_takeDepthInput(void){
    function_called();
    int ret_int = 0;

    if(has_mock()){
        ret_int = mock_type(int);
    }else{
        stop();
    }

    return ret_int;
}
