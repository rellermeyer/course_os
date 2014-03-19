#include "include/argparse.h"


/* Parse the list of strings (argv) and process each argument */
void parse_arguments(int argc, char **argv)
{
  // TODO: free original argv?
  char **argument_list = argv;

  int i = 0;
  while (i < argc)
  {
    int delta_args_read = 0;

    // Read argument(s) and process them
    delta_args_read += analyze_args(argument_list);

    argument_list += delta_args_read;
    i += delta_args_read;
  }
}


/* Read arguments and do something based on the arguments.
   Return the number of arguments read.
   char **args: array of pointers to each argument
*/
int analyze_args(char **argv)
{
  int i = 0;

  /* This is where we test argv[0] to be some option. Example:

  if (x)
  {
    call_function(argv[0], argv[1]);
    return 2; // Consumed 2 arguments
  }

  */

  // Default case: read one argument
  return 1;
}
