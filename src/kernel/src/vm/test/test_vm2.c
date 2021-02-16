#include <test.h>
#include <vm2.h>

TEST_CREATE(test_size, {
    ASSERT_EQ(sizeof(L2PagetableEntry), 4);
    ASSERT_EQ(sizeof(L1PagetableEntry), 4);
})
