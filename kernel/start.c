#include "include/global_defs.h"
#include <stdint.h>
#include "include/argparse.h"


void start(void *p_bootargs) {
   char *cmdline_args = read_cmdline_tag(p_bootargs);

   print_uart0("arguments: ");
   print_uart0(cmdline_args);
   print_uart0("\n");
   print_uart0("CourseOS!\n");

   char** arg_list = split_string(cmdline_args);
   int arg_count = sizeof(arg_list) / sizeof(arg_list[0]);
   parse_args(arg_count, arg_list);
}

