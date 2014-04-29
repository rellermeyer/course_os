#include "include/global_defs.h"
#include "include/argparse.h"
#include "include/klibc.h"
#include "include/tests.h"


/* Get the command-line arguments and run the functions to process them. */
void run_argparse(uint32_t p_bootargs)
{
  char *cmdline_args = read_cmdline_tag(p_bootargs);

  // Separate the command-line arguments into separate Strings
  int num_args = number_of_words(cmdline_args);
  char* arg_list[num_args];
  split_string(cmdline_args, arg_list);
  int arg_count = sizeof(arg_list) / sizeof(arg_list[0]);

  // Examine each String and do something based on the argument
  parse_arguments(arg_count, arg_list);
}


/* Parse the list of strings (argv) and process each argument */
void parse_arguments(int argc, char **argv)
{
  char **argument_list = argv;

  int i = 0;
  while (i < argc)
  {
    int delta_args_read = 0;

    // Read argument(s) and process them
    delta_args_read += analyze_arguments(argument_list);

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
       -load position size
       -load 0x1234 0xabcd
       position and size must be separated by spaces
       position is the location in memory of the program
       size is te size of the program
    */
    char *position = argv[i + 1];
    char *size = argv[i + 2];

    // Check that we have valid input
    if (position != NULL && size != NULL)
    {
      int BASE = 16;
      int position_i = string_to_unsigned_int(position, BASE);
      int size_i = string_to_unsigned_int(size, BASE);

      os_printf("Ready to load program at position 0x%x of size 0x%x\n",
        position_i, size_i);
      // TODO: load and run process

      return 3; // Consumed 3 arguments
    }
    else
    {
      os_printf("Error loading process via qemu arguments.\n");
      os_printf("USAGE: -load position size\n");
      if (argv[i + 1] == NULL)
        return 1; // Consumed 1 argument
      else
        return 2; // Consumed 2 arguments
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
  char line_copy[os_strlen(line)];
  os_strcpy(line_copy, line);
  const char *delimiters = " \t"; // Space and tab

  char *pos = (char *)os_strtok(line_copy, delimiters);

  // Count the number of words
  int count = 0;
  while (pos != NULL)
  {
    pos = (char *)os_strtok(NULL, delimiters); // Advance to the next word
    count++;
  }

  return count;
}


/* We return a signed integer because we need a way to detect errors.

   Read a string of characters and interpret them as an unsigned integer.
   Return -1 if the string cannot be read as an unsigned integer.
   Return an integer representing the value of the string otherwise.
*/
int string_to_unsigned_int(char *input, int base)
{
  int i = os_strlen(input) - 1; // Index in the string

  if (hex_value_of_character(input[i]) == -1)
  {
    // Return -1 if the string cannot be read as an unsigned integer
    return -1;
  }

  int n = 0; // Number of digits
  int result = 0;

  // Starting from the end of the string, read each character
  while (i >= 0)
  {
    // Get the value of one character
    int digit = hex_value_of_character(input[i]);

    if (digit == -1)
    {
      // If we hit a non-digit character such as 'x', return
      return result;
    }

    // Add the digit's value to the integer
    result = result | (digit << (n * 4));

    i--;
    n++;
  }
  return result;
}


/* Return the hexidecimal value of the single character c */
int hex_value_of_character(char c)
{
  if (c >= '0' && c <= '9')
  {
    return (int)(c - '0');
  }
  else if (c >= 'A' && c <= 'F')
  {
    return (int)(c - 'A' + 10);
  }
  else if (c >= 'a' && c <= 'f')
  {
    return (int)(c - 'a' + 10);
  }

  // Return -1 if c was not a hex digit
  return -1;
}
