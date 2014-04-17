#include "include/global_defs.h"
#include <stdint.h>
#include "include/argparse.h"
#include "include/klibc.h"
#include "include/process.h"

void start(void *p_bootargs) {
   char *cmdline_args = read_cmdline_tag(p_bootargs);

   print_uart0("arguments: ");
   print_uart0(cmdline_args);
   print_uart0("\n");
   print_uart0("CourseOS!\n");

   // Separate the command-line arguments into separate Strings
   char** arg_list = (char **)mem_alloc(number_of_words(cmdline_args));
   split_string(cmdline_args, arg_list);
   int arg_count = sizeof(arg_list) / sizeof(arg_list[0]);

   // Parse and analyze each String
   parse_args(arg_count, arg_list);

   free(arg_list);

   // TODO: initialize GLOBAL_PID and PCB table
}
