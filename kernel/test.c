#include "test.h"
#include <stdlib.h>

TEST create_test(char* name, int (*function_to_test)()) {
    TEST test = {name, function_to_test};

    return test;
}
