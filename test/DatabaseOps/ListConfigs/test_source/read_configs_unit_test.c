#include "../test_include/read_configs_unit_test.h"

void testListAllRoots(void **state){
    (void) state;
    int ret = listAllRoots();
    assert_true(ret == NO_ERROR);
}

void testListRootsForConfig(void **state){
    (void) state;
    int ret = listConfigRoots(AUDIO_CONFIG);
    assert_true(ret == NO_ERROR);
}

void testListAllRootsWithPaths(void **state){
    (void) state;
    int ret = listAllRootWithPaths();
    assert_true(ret == NO_ERROR);
}

void testListRootAndPathsForConfig(void **state){
    (void) state;
    int ret = listConfigRootsWithPaths(AUDIO_CONFIG);
    assert_true(ret == NO_ERROR);
}
