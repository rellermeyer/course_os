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


char* read_cmdline_tag(uint32_t *tag_base)
{
  uint32_t *tag_pointer = tag_base;
  uint32_t ATAG_CMDLINE = 0x54410009; // "Type" of cmdline tag

  // Look through the tag headers
  uint32_t size = 0;
  uint32_t type = 0;
  while (type != ATAG_CMDLINE)
  {
    tag_pointer += size;

    // Each tag has a header containing the tag's size and type.
    size = *tag_pointer; // In bytes, including the header
    tag_pointer += 1;
    type = *tag_pointer; // Specifies which tag follows
    tag_pointer -= 1; // Size includes the size of the header
  }

  // We are now pointing at the ATAG_CMDLINE header
  tag_pointer += 2; // Move past the tag header to the data
  char *command_line_args = (char *)tag_pointer;

  return command_line_args;
}
