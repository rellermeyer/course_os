#include <test.h>
#include <stdlib.h>

TEST_CREATE(test_free_null, {
    kfree(NULL);
})
