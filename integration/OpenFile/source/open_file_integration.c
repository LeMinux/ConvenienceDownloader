#include "../includes/open_file_integration.h"

static void closeFile(FILE* file){
    if(file != NULL){
        fclose(file);
    }
}

void testOpenFileOpenRegFile(void** state){
    (void) state;
    char input [] = "./test_env/test_file";
    char mode [] = "r";
    FILE* act_file = openFile(input, mode);
    assert_non_null(act_file);
    closeFile(act_file);
}

void testOpenFileOpenDir(void** state){
    (void) state;
    char input [] = "./test_env/test_dir";
    char mode [] = "r";
    FILE* act_file = openFile(input, mode);
    assert_null(act_file);
}

void testOpenFileNonExist(void** state){
    (void) state;
    char input [] = "ealtiatheiahng";
    char mode [] = "r";
    FILE* act_file = openFile(input, mode);
    assert_null(act_file);
    closeFile(act_file);
}

void testOpenFileDevFile(void** state){
    (void) state;
    char input [] = "/dev/mem";
    char mode [] = "r";
    FILE* act_file = openFile(input, mode);
    assert_null(act_file);
    closeFile(act_file);
}

void testOpenFileLinkToFile(void** state){
    (void) state;
    char input [] = "./test_env/link_test_file";
    char mode [] = "r";
    FILE* act_file = openFile(input, mode);
    assert_non_null(act_file);
    closeFile(act_file);
}

void testOpenFileLinkToDir(void** state){
    (void) state;
    char input [] = "./test_env/link_test_dir";
    char mode [] = "r";
    FILE* act_file = openFile(input, mode);
    assert_null(act_file);
    closeFile(act_file);
}

void testOpenFileBrokenLink(void** state){
    (void) state;
    char input [] = "./test_env/link_broken";
    char mode [] = "r";
    FILE* act_file = openFile(input, mode);
    assert_null(act_file);
    closeFile(act_file);
}

void testOpenFileAbsolutePathToFile(void** state){
    (void) state;
    char input [] = "/etc/passwd";
    char mode [] = "r";
    FILE* act_file = openFile(input, mode);
    assert_non_null(act_file);
    closeFile(act_file);
}

void testOpenFileNoPerms(void** state){
    (void) state;
    char input [] = "/etc/shadow";
    char mode [] = "r";
    FILE* act_file = openFile(input, mode);
    assert_null(act_file);
    closeFile(act_file);
}
