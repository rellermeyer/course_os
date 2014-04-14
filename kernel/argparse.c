#include <stdint.h>


#include "include/argparse.h"
#include "include/global_defs.h"
#include "include/tests.h"
#include "include/mem_alloc.h"


/* Parse the list of strings (argv) and process each argument */
void parse_args(int argc, char **argv)
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
   TODO: use argc to prevent reading too many arguments?
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

  // Run some tests
  if (os_strcmp(argv[i], "TEST") == 0)
  {
    os_printf("Running tests...\n");
    Test *tests[2];
    tests[0] = create_test("This passes", &test1);
    tests[1] = create_test("This fails", &test2);
    run_tests(tests, 2);
  }

  // Default case: read one argument
  return 1;
}


/* Read the cmdline tag to get the kernel arguments */
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


/* Separate the string line based on whitespace. Return this array of strings.
   TODO: Dynamically change the size of the result array. Might have to
   pass an empty array as a parameter. Count the number of spaces in the
   line to determine maximum number of entries.
*/
char** split_string(char* line, char** list)
{
  char *piece = NULL; // One null-terminated part of the string
  const char *delimiters = " \t"; // Space and tab

  piece = strtok(line, delimiters);
  list[0] = piece;
  int i = 1;

  while (piece != NULL)
  {
    piece = (char *)strtok(NULL, delimiters); // Advances pointer to the next token
    list[i] = piece;
    i++;
  }

  return list;
}


/* Return the number of whitespace-delimited words in String line. */
int number_of_words(char *line)
{
  const char *delimiters = " \t"; // Space and tab
  char *pos = strtok(line, delimiters);
  int count = 0;

  // Count the number of words
  while (pos != NULL)
  {
    pos = (char *)strtok(NULL, delimiters); // Advance to the next word
    count++;
  }

  return count;
}
