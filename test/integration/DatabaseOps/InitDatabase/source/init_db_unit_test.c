#include "../includes/init_db_unit_test.h"

void testInitDB(void** state){
    (void) state;
    enum ERROR exp_ret = NO_ERROR;
    enum ERROR act_ret = initDatabase();
    assert_true(act_ret == exp_ret);
}
