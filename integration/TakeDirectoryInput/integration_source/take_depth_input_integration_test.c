#include "../integration_includes/take_directory_input_integration_test.h"

void testTakeDirectoryInputAbsolutePathToDir(void** state){
    (void) state;
    char directory_input [] = "/tmp/";

    will_return(__wrap_boundedInput, directory_input);
    will_return(__wrap_boundedInput, strlen(directory_input));

    char* act_result = takeDirectoryInput();

    assert_string_equal(act_result, directory_input);
}

void testTakeDirectoryInputRelativePathToDir(void** state){
    (void) state;
    char directory_input [] = "./test_env/test_dir/";

    char cwd [PATH_MAX];
    if(getcwd(cwd, sizeof(cwd)) == NULL){
        fail_msg("Could not get current directory for assertion\n");
    }

    char absolute_path [4096] = "";
    //need to add two to not add './' into the check
    size_t amt_written = snprintf(absolute_path, sizeof(absolute_path), "%s/%s", cwd, (directory_input + 2));

    if(amt_written > sizeof(absolute_path)){
        fail_msg("Could not write entire path to test because it was truncated");
    }

    will_return(__wrap_boundedInput, directory_input);
    will_return(__wrap_boundedInput, strlen(directory_input));

    char* act_result = takeDirectoryInput();

    assert_string_equal(act_result, absolute_path);
}

void testTakeDirectoryInputAbsolutePathToFile(void** state){
    (void) state;
    char directory_input [] = "/etc/passwd";

    will_return(__wrap_boundedInput, directory_input);
    will_return(__wrap_boundedInput, strlen(directory_input));

    char* act_result = takeDirectoryInput();

    assert_null(act_result);
}

void testTakeDirectoryInputRelativePathToFile(void** state){
    (void) state;
    char directory_input [] = "../../../../../../../../../../etc/passwd";

    will_return(__wrap_boundedInput, directory_input);
    will_return(__wrap_boundedInput, strlen(directory_input));

    char* act_result = takeDirectoryInput();

    assert_null(act_result);
}

void testTakeDirectoryInputTildePathToDir(void** state){
    (void) state;
    char directory_input [] = "~/.config/";

    will_return(__wrap_boundedInput, directory_input);
    will_return(__wrap_boundedInput, strlen(directory_input));

    char* act_result = takeDirectoryInput();

    assert_null(act_result);
}

void testTakeDirectoryInputTildePathToFile(void** state){
    (void) state;
    char directory_input [] = "~/.bashrc";

    will_return(__wrap_boundedInput, directory_input);
    will_return(__wrap_boundedInput, strlen(directory_input));

    char* act_result = takeDirectoryInput();

    assert_null(act_result);
}

void testTakeDirectoryInputNoSlashAtEndAboslute(void** state){
    (void) state;
    char directory_input [] = "/tmp";

    will_return(__wrap_boundedInput, directory_input);
    will_return(__wrap_boundedInput, strlen(directory_input));

    char* act_result = takeDirectoryInput();

    assert_string_equal(act_result, "/tmp/");
}

void testTakeDirectoryInputNoSlashAtEndRelative(void** state){
    (void) state;
    char directory_input [] = "../../../../../../../../../tmp";

    will_return(__wrap_boundedInput, directory_input);
    will_return(__wrap_boundedInput, strlen(directory_input));

    char* act_result = takeDirectoryInput();

    assert_string_equal(act_result, "/tmp/");
}

void testTakeDirectoryInputPathToNonExist(void** state){
    (void) state;
    char directory_input [] = "/wow/this/is/some/path/";

    will_return(__wrap_boundedInput, directory_input);
    will_return(__wrap_boundedInput, strlen(directory_input));

    char* act_result = takeDirectoryInput();

    assert_null(act_result);
}

void testTakeDirectoryInputLinkToDir(void** state){
    (void) state;
    char directory_input [] = "./test_env/link_test_dir";

    will_return(__wrap_boundedInput, directory_input);
    will_return(__wrap_boundedInput, strlen(directory_input));

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
    char directory_input [] = "./test_env/link_test_dir/";

    will_return(__wrap_boundedInput, directory_input);
    will_return(__wrap_boundedInput, strlen(directory_input));

    char* act_result = takeDirectoryInput();

    assert_null(act_result);
}

void testTakeDirectoryInputLinkToFile(void** state){
    (void) state;
    char directory_input [] = "./test_env/link_test_file";

    will_return(__wrap_boundedInput, directory_input);
    will_return(__wrap_boundedInput, strlen(directory_input));

    char* act_result = takeDirectoryInput();

    assert_null(act_result);
}

void testTakeDirectoryInputPathToBrokenLink(void** state){
    (void) state;
    char directory_input [] = "./test_env/link_broken/";

    will_return(__wrap_boundedInput, directory_input);
    will_return(__wrap_boundedInput, strlen(directory_input));

    char* act_result = takeDirectoryInput();

    assert_null(act_result);
}


void testTakeDirectoryInputLinkToNoPerms(void** state){
    (void) state;
    char directory_input [] = "./test_env/link_no_perm/";

    will_return(__wrap_boundedInput, directory_input);
    will_return(__wrap_boundedInput, strlen(directory_input));

    char* act_result = takeDirectoryInput();

    assert_null(act_result);
}


void testTakeDirectoryInputNoPermsAbsolutePath(void** state){
    (void) state;
    char directory_input [] = "/root/";

    will_return(__wrap_boundedInput, directory_input);
    will_return(__wrap_boundedInput, strlen(directory_input));

    char* act_result = takeDirectoryInput();

    assert_null(act_result);
}

void testTakeDirectoryInputNoPermsRelativePath(void** state){
    (void) state;
    char directory_input [] = "../../../../../../../../../../../root/";

    will_return(__wrap_boundedInput, directory_input);
    will_return(__wrap_boundedInput, strlen(directory_input));

    char* act_result = takeDirectoryInput();

    assert_null(act_result);
}

void testTakeDirectoryInputNoWritePerms(void** state){
    (void) state;
    char directory_input [] = "/usr/bin/";

    will_return(__wrap_boundedInput, directory_input);
    will_return(__wrap_boundedInput, strlen(directory_input));

    char* act_result = takeDirectoryInput();

    assert_null(act_result);
}
