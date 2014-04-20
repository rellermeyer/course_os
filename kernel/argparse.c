#include <stdint.h>


#include "include/global_defs.h"
#include "include/argparse.h"
#include "include/klibc.h"
#include "include/tests.h"
#include "include/mem_alloc.h"


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
int analyze_arguments(char **argv)
{
  int i = 0;

  /* This is where we test argv[0] to be some option. Example:

  if (x)
  {
    call_function(argv[0], argv[1]);
    return 2; // Consumed 2 arguments
  }

  */

  
  // TODO: load process(es) and run them
  if (os_strcmp(argv[i], "-load") == 0)
  {
    /* Format:
       -load position, size
       -load 0x1234, 0xabcd
       position and size can be separated by commas, spaces, or both
    */
    char prgm_pos_and_size[sizeof(argv[i+1])];
    os_strcpy(&prgm_pos_and_size, argv[i + 1]);

    // Comma and space delimiters used to separate position and size
    char *delimeters = ", ";
    char *position = (char *)os_strtok(prgm_pos_and_size, delimeters);
    char *size = (char *)os_strtok(NULL, delimeters);

    // Check that we have valid input
    if (position != NULL && size != NULL)
    {
      // TODO: load program into memory using position and size
      return 2; // Read 2 arguments
    }
    else
    {
      os_printf("Error loading process via qemu arguments.\n");
      os_printf("USAGE: -load position, size");
    }
  }

  // Run some tests
  if (os_strcmp(argv[i], "-test") == 0)
  {
    os_printf("Running tests...\n");
    Test *tests[2];
    tests[0] = create_test("This passes", &test1);
    tests[1] = create_test("This fails", &test2);
    run_tests(tests, 2);
    return 1; // Read 1 argument
  }

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


/* Separate the string line based on whitespace.
   Return this array of strings.
*/
char** split_string(char* line, char** list)
{
  char *piece = NULL; // One null-terminated part of the string
  const char *delimiters = " \t"; // Space and tab

  piece = (char *)os_strtok(line, delimiters);
  list[0] = piece;
  int i = 1;

  while (piece != NULL)
  {
    // os_strtok() advances pointer to the next whitespace delimiter
    piece = (char *)os_strtok(NULL, delimiters);
    list[i] = piece;
    i++;
  }

  return list;
}


/* Return the number of whitespace-delimited words in String line. */
int number_of_words(char *line)
{
  const char *delimiters = " \t"; // Space and tab
  char *pos = (char *)os_strtok(line, delimiters);
  int count = 0;

  // Count the number of words
  while (pos != NULL)
  {
    pos = (char *)os_strtok(NULL, delimiters); // Advance to the next word
    count++;
  }

  return count;
}
