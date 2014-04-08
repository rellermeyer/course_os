#include <stdio.h>
#include <stdlib.h>
#include "include/tests.h"

//This function executes and displays results of test set given to it.
void run_tests(Test *tests[], int num_tests)
{
  int i;
  for(i = 0; i < num_tests; i++)
  {
    //This needs to changed to os_printf eventuallly.
    printf("%s",tests[i]->test_name);
    printf(" ... ");
    //evaluates test here, if it passes prints PASSES else FAILS
    if((tests[i]->testptr)())
    {
      printf("PASSES\n");
    }
    else
    {
      printf("FAILS\n");
    }
  }
}

Test* create_test(char *name, int (*test_function)(void*))
{
  //This malloc needs to be changed before we can run it in the kernel. Still working on that though.
  Test *test = malloc(sizeof(Test));
  test->test_name = name;
  test->testptr = test_function;
  return test;
}
