#include "../includes/init_db_unit_test.h"

/*
* you may be confused why this is here as it is a unit.
* Well to get the user's home directory it would require a real
* user that can't really be faked. I mean you could by stubbing, but that's more
* complicated for no reason.
*/
int main(void){
    const struct CMUnitTest init_test[] = {
        cmocka_unit_test(testInitDB),
    };
    return cmocka_run_group_tests(init_test, NULL, NULL);
}
