#include "../includes/testWrapInput.h"

int __wrap_boundedInput(FILE* stream, char* dest, size_t dest_size){
    //don't care about these just want to return a string
    (void)stream;
    (void)dest_size;

    char* set_string = mock_type(char*);
    int write_len  = mock_type(int);
    memcpy(dest, set_string, write_len);
    dest[write_len] = '\0';
    return write_len;
}
