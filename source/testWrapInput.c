#include "../includes/testWrapInput.h"

enum CALL call_real_function = NO_CALL;

//really here just so the compiler doesn't say undefiend references to the __real_
//functions that aren't wrapped
//If there is no corresponding __real_ then that function hasn't been wrapped
//, and neither __wrap_ nor __real_ will be called because they don't exist.
int __real_takeDepthInput(void) __attribute__((weak));
int __real_takeIndexInput(int) __attribute__((weak));
char* __real_takeDirectoryInput(void) __attribute__((weak));

int __wrap_boundedInput(FILE* stream, char* dest, size_t dest_size){
    //don't care about these just want to return a string
    (void) stream;
    (void) dest_size;

    const char* stubbed_string = mock_type(char*);
    int write_len  = mock_type(int);
    memcpy(dest, stubbed_string, write_len);
    dest[write_len] = '\0';
    return write_len;
}

//If a mock is available it will return the mock first.
//the usage of __real_ function is there to determine behavior once the mock
//is consumed or if there is no mock to be had.
//This may be wanted because the below functions also use boundedInput, so if
//the actual behavior of these functions are wanted it can be tested.

int __wrap_takeIndexInput(int max){
    function_called();
    int ret_int = 0;

    if(has_mock()){
        ret_int = mock_type(int);
    }else{
        if(call_real_function == CALL){
            ret_int = __real_takeIndexInput(max);
        }else{
            stop();
        }
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
        if(call_real_function == CALL){
            ret_string = __real_takeDirectoryInput();
        }else{
            stop();
        }
    }

    return ret_string;
}

int __wrap_takeDepthInput(void){
    function_called();
    int ret_int = 0;

    if(has_mock()){
        ret_int = mock_type(int);
    }else{
        if(call_real_function == CALL){
            ret_int = __real_takeDepthInput();
        }else{
            stop();
        }
    }

    return ret_int;
}
