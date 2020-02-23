#include <test.h>
#include <qstr.h>

TEST_CREATE(test_compare_nonequal_length, {
    Qstr a = qstr_from_null_terminated_string("test");
    Qstr b = qstr_from_null_terminated_string("toast");
    ASSERT(!qstr_eq(&a, &b));
    ASSERT_EQ(a.hash, 0);
    ASSERT_EQ(b.hash, 0);
    qstr_free(&a);
    qstr_free(&b);
})

TEST_CREATE(test_compare_equal_length, {
    Qstr a = qstr_from_null_terminated_string("tosti");
    Qstr b = qstr_from_null_terminated_string("toast");
    ASSERT(!qstr_eq(&a, &b));
    ASSERT_NEQ(a.hash, 0);
    ASSERT_EQ(b.hash, 0);
    qstr_free(&a);
    qstr_free(&b);
})

TEST_CREATE(test_compare_equal_length_eq, {
    Qstr a = qstr_from_null_terminated_string("toast");
    Qstr b = qstr_from_null_terminated_string("toast");
    ASSERT(qstr_eq(&a, &b));
    ASSERT_EQ(a.hash, b.hash);
    qstr_free(&a);
    qstr_free(&b);
})

TEST_CREATE(test_compare_equal_length_eq_null, {
    Qstr a = qstr_from_null_terminated_string("toast");
    ASSERT(qstr_eq_null_terminated(&a, "toast"));
    ASSERT_NEQ(a.hash, 0);

    qstr_free(&a);
})


TEST_CREATE(test_compare_various, {
    Qstr a = qstr_from_null_terminated_string("toast");
    Qstr b = qstr_from_null_terminated_string("tosti");
    Qstr c = qstr_from_null_terminated_string("toast");

    ASSERT(!qstr_eq(&a, &b));
    ASSERT_NEQ(a.hash, 0);
    ASSERT_EQ(b.hash, 0);

    ASSERT(!qstr_eq(&b, &a));
    ASSERT_NEQ(a.hash, 0);
    ASSERT_NEQ(b.hash, 0);
    ASSERT_NEQ(a.hash, b.hash);

    ASSERT(qstr_eq(&c, &a));
    ASSERT(qstr_eq(&a, &c));
    ASSERT_NEQ(c.hash, 0);

    qstr_free(&a);
    qstr_free(&b);
    qstr_free(&c);
})
