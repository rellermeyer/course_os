#include <stdint.h>


#include "include/argparse.h"


/* Parse the list of strings (argv) and process each argument */
void parse_arguments(int argc, char **argv)
{
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


void copy_cmdline_tag(uint32_t *tag_base)
{
  uint32_t *tag_pointer = tag_base;
  print_uart0("In copy_cmdline_tag\n"); // TODO: remove

  uint32_t ATAG_CMDLINE = 0x54410009; // Value of cmdline tag will be this

  // Look through the headers starting at tag_base
  uint32_t size = 0;
  uint32_t type = 0;
  while (type != ATAG_CMDLINE)
  {
    size = *tag_pointer;
    tag_pointer += 1;
    type = *tag_pointer;
    tag_pointer -= 1;

    if (*tag_pointer == 0x54410002)
      print_uart0("CORE");

    print_uart0(size);
    print_uart0(type);
    tag_pointer += size;
  }

  // TODO: Get the ATAG_CMDLINE
  char *command_line_args = tag_pointer;
  command_line_args = 'a';
  print_uart0(command_line_args);

  print_uart0("\nDone with copy_cmdline_tag\n"); // TODO: remove
}
