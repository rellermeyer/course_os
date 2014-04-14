#include "include/global_defs.h"
#include <stdint.h>
#include "include/argparse.h"


void start(void *p_bootargs) {
   char *cmdline_args = read_cmdline_tag(p_bootargs);

   print_uart0("arguments: ");
   print_uart0(cmdline_args);
   print_uart0("\n");
   print_uart0("CourseOS!\n");

   char** arg_list = (char **)mem_alloc(number_of_words(cmdline_args));
   split_string(cmdline_args, arg_list);
   int arg_count = sizeof(arg_list) / sizeof(arg_list[0]);
   parse_args(arg_count, arg_list);
   free(arg_list);
}

