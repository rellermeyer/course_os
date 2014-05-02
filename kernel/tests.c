
#include "include/global_defs.h"
#include "include/tests.h"
#include "include/klibc.h"
#include <stdint.h>

//This function executes and displays results of test set given to it.
void run_tests(Test *tests[], int num_tests)
{
  uint32_t i;
  for(i = 0; i < num_tests; i++)
  { 
    //evaluates test here, if it passes prints PASSES else FAILS
    if((tests[i]->testptr)())
    {
      os_printf(tests[i]->test_name);
      os_printf(" ... ");
      os_printf("PASSES\n");
    }
    else
    {
      os_printf(tests[i]->test_name);
      os_printf(" ... ");
      os_printf("FAILS\n");
    }
  }
}

Test* create_test(char *name, uint32_t (*test_function)(void))
{
  Test *test = kmalloc(sizeof(Test));
  test->test_name = name;
  test->testptr = test_function;
  return test;
}

