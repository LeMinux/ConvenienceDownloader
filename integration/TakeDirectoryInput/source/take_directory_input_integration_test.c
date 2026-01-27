#include "../includes/take_directory_input_integration_test.h"

enum ACCEPTING_TRAILING_SLASH {NO = 0, YES};
enum ACCEPTING_TRAILING_SLASH want_slash = NO;

void testTakeDirectoryInputAbsolutePathToDirWithTrailingSlash(void** state){
    (void) state;
    char directory_input [] = "/tmp/";

    will_return(__wrap_boundedInput, directory_input);

    char* act_result = takeDirectoryInput();

    assert_string_equal(act_result, "/tmp");
}

void testTakeDirectoryInputAbsolutePathToDirWithoutTrailingSlash(void** state){
    (void) state;
    char directory_input [] = "/tmp";

    will_return(__wrap_boundedInput, directory_input);

    char* act_result = takeDirectoryInput();

    assert_string_equal(act_result, "/tmp");
}

void testTakeDirectoryInputRelativePathToDirWithTrailingSlash(void** state){
    (void) state;
    char directory_input [] = "../../../../../../../../../tmp/";

    will_return(__wrap_boundedInput, directory_input);

    char* act_result = takeDirectoryInput();

    assert_string_equal(act_result, "/tmp");
}

void testTakeDirectoryInputRelativePathToDirWithoutTrailingSlash(void** state){
    (void) state;
    const char directory_input [] = "../../../../../../../../../tmp";

    will_return(__wrap_boundedInput, directory_input);

    char* act_result = takeDirectoryInput();

    assert_string_equal(act_result, "/tmp");
}

void testTakeDirectoryInputAbsolutePathToNonDir(void** state){
    (void) state;
    const char directory_input [] = "/etc/passwd";

    will_return(__wrap_boundedInput, directory_input);

    char* act_result = takeDirectoryInput();

    assert_null(act_result);
}

void testTakeDirectoryInputRelativePathToNonDir(void** state){
    (void) state;
    const char directory_input [] = "../../../../../../../../../../etc/passwd";

    will_return(__wrap_boundedInput, directory_input);

    char* act_result = takeDirectoryInput();

    assert_null(act_result);
}

void testTakeDirectoryInputTildePathToDir(void** state){
    (void) state;
    const char directory_input [] = "~/.config/";

    will_return(__wrap_boundedInput, directory_input);

    char* act_result = takeDirectoryInput();

    assert_null(act_result);
}

void testTakeDirectoryInputTildePathToFile(void** state){
    (void) state;
    const char directory_input [] = "~/.bashrc";

    will_return(__wrap_boundedInput, directory_input);

    char* act_result = takeDirectoryInput();

    assert_null(act_result);
}

void testTakeDirectoryInputPathToNonExist(void** state){
    (void) state;
    const char directory_input [] = "/wow/this/is/some/path/";

    will_return(__wrap_boundedInput, directory_input);

    char* act_result = takeDirectoryInput();

    assert_null(act_result);
}

void testTakeDirectoryInputLinkToDir(void** state){
    (void) state;
    const char directory_input [] = "./test_env/link_test_dir";

    will_return(__wrap_boundedInput, directory_input);

    char* act_result = takeDirectoryInput();

    assert_null(act_result);
}

//For what ever reason at least for my system, if lstat encounters a link
//that goes to a directory if the path name has a trailing slash at the end
//it resolves the link to the directory despite using lstat.
//The contents of the link can have an ending slash or not it doesn't matter.
//It's just if the path passed to lstat has a trailing slash.
void testTakeDirectoryInputLinkToDirWithTrailingSlash(void** state){
    (void) state;
    const char directory_input [] = "./test_env/link_test_dir/";

    will_return(__wrap_boundedInput, directory_input);

    char* act_result = takeDirectoryInput();

    assert_null(act_result);
}

void testTakeDirectoryInputLinkToFile(void** state){
    (void) state;
    const char directory_input [] = "./test_env/link_test_file";

    will_return(__wrap_boundedInput, directory_input);

    char* act_result = takeDirectoryInput();

    assert_null(act_result);
}

void testTakeDirectoryInputPathToBrokenLink(void** state){
    (void) state;
    const char directory_input [] = "./test_env/link_broken/";

    will_return(__wrap_boundedInput, directory_input);

    char* act_result = takeDirectoryInput();

    assert_null(act_result);
}


void testTakeDirectoryInputLinkToNoPerms(void** state){
    (void) state;
    const char directory_input [] = "./test_env/link_no_perm/";

    will_return(__wrap_boundedInput, directory_input);

    char* act_result = takeDirectoryInput();

    assert_null(act_result);
}


void testTakeDirectoryInputNoPermsAbsolutePath(void** state){
    (void) state;
    const char directory_input [] = "/root/";

    will_return(__wrap_boundedInput, directory_input);

    char* act_result = takeDirectoryInput();

    assert_null(act_result);
}

void testTakeDirectoryInputNoPermsRelativePath(void** state){
    (void) state;
    const char directory_input [] = "../../../../../../../../../../../root/";

    will_return(__wrap_boundedInput, directory_input);

    char* act_result = takeDirectoryInput();

    assert_null(act_result);
}

void testTakeDirectoryInputNoWritePerms(void** state){
    (void) state;
    const char directory_input [] = "/usr/bin/";

    will_return(__wrap_boundedInput, directory_input);

    char* act_result = takeDirectoryInput();

    assert_null(act_result);
}


void testTakeDirectoryInputPathHasSpaces(void** state){
    (void) state;
    //mkdtemp doesn't want const char
    char dir_template [] = "/tmp/some dir haXXXXXX";

    char* directory_input = mkdtemp(dir_template);
    char* exp_result = directory_input;

    will_return(__wrap_boundedInput, directory_input);

    char* act_result = takeDirectoryInput();

    assert_string_equal(act_result, exp_result);
    rmdir(directory_input);
}
