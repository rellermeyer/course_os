#include "../include/tests.h"

#define NUMTESTS 2

//This is where you define the tests you want to run. They return 1 on success and 0 on failure.
int test1()
{
  return 1;
}

int test2()
{
  return 0;
}

/*
main()
{
  //This is the array of test functions that we pass to the test runner.
  Test *tests[NUMTESTS];

  //Tests are created by providing a descriptor that will be printed out when the test is run, and the address of the test function.
  tests[0] = create_test("This passes", &test1);

  tests[1] = create_test("This fails", &test2);

  run_tests(tests, NUMTESTS);
}
*/
