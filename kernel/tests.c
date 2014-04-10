#include "include/tests.h"

//This function executes and displays results of test set given to it.
void run_tests(Test *tests[], int num_tests)
{
  int i;
  for(i = 0; i < num_tests; i++)
  {
    //This needs to changed to os_printf eventuallly.
    os_printf("%s",tests[i]->test_name);
    os_printf(" ... ");
    //evaluates test here, if it passes prints PASSES else FAILS
    if((tests[i]->testptr)())
    {
      os_printf("PASSES\n");
    }
    else
    {
      os_printf("FAILS\n");
    }
  }
}

Test* create_test(char *name, int (*test_function)(void*))
{
  Test *test = mem_alloc(sizeof(Test));
  test->test_name = name;
  test->testptr = test_function;
  return test;
}
