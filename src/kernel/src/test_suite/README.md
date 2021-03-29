# Course OS Testing framework

This directory is a custom-build testing framework by [

To run tests, run

```bash
make test
```

If you are sitting here, thinking: the way this test framework works is one enormous hack: yes it is. Unfortunately that's how it is in
operating systems. The testing framework depends on  the kernel booting, because how else could we run any code? Testing complex
interactions in the kernel is very hard as test may use resources of the kernel itself. My advice: deal with it.

## Creating tests.

To create a new test, you can use the `TEST_CREATE` macro. It takes two arguments, a test name (which must be globally unique) and a block of code.
An example test would be

```c
#include <test.h>

TEST_CREATE(test_one_plus_one, {
    ASSERT_EQ(1 + 1, 2);
})

```

Tests created like this will be automatically detected and compiled into a file `src/test/test.c` which will then be compiled by gcc into the kernel.
Tests can be created in any file in any subdirectory of the src file. Tests will be randomized in order, and you cannot depend on the order in which tests are executed.
This has been done deliberately since we have had many problems with accidentally depending on this.

When the MEM_DEBUG definition is given in the makefile (default for tests), the testing framework will record the allocator's number of bytes allocated before the test. If this number is not equal after the test, the test will fail.
When running with ENABLE_TESTS on, WARN macros will report file and line number, and DATA_ABORT handlers will panic and fail the test.

## Assert macros

There are a number of assertion macros included in `test.h`.

They are listed here:

| Macro name            | usage                                                  |
| ---                   | ---                                                    |
| PASS()                | Will immediately return and pass the test              |
| FAIL()                | Will immediately return and fail the test              |
| ASSERT(expr)          | Will fail the test if `expr` evaluates to zero/false   |
| ASSERT_EQ(l, r)       | Will fail the test if `l != r`                         |
| ASSERT_GT(l, r)       | Will fail the test if `!(l >= r)`                      |
| ASSERT_GTEQ(l, r)     | Will fail the test if `!(l > r)`                       |
| ASSERT_GT(l, r)       | Will fail the test if `!(l >= r)`                      |
| ASSERT_LT(l, r)       | Will fail the test if `!(l < r)`                       |
| ASSERT_LTEQ(l, r)     | Will fail the test if `!(l <= r)`                      |
| ASSERT_NEQ(l, r)      | Will fail the test if `l == r`                         |
| ASSERT_NOT_NULL(expr) | Will fail the test if `expr` evaluates to NULL         |
| ASSERT_NULL(expr)     | Will fail the test if `expr` doesn't evaluates to NULL |
| ASSERT_STRCMP         | Will fail if the two strings aren't equal              |
| ASSERT_STRNCMP        | Will fail if the two string are equal                  |
| ASSERT_INT_EQ         | Will fail if the two integers aren't equal             |
| ASSERT_CHR_EQ         | Will fail if the two chars aren't equal                |
| ASSERT_FLT_EQ         | Will fail if the two floats aren't equal               |
| ASSERT_DBL_EQ         | Will fail if the two doubles aren't equal              |
| ASSERT_STR_EQ         | Will fail if the two strings aren't equal              |
| ASSERT_LONG_EQ        | Will fail if the two longs aren't equal                |
| ASSERT_PTR_EQ         | Will fail if the two pointers aren't equal             |
