#include <stdio.h>
#include <stdlib.h>
#include <test.h>

TEST_CREATE(test_free_null, {
    kfree(NULL);
})
