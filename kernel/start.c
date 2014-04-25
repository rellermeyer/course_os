#include "include/global_defs.h"
#include "include/argparse.h"
#include "include/klibc.h"
#include "include/process.h"

#include <stdint.h>


void start(void *p_bootargs) {
  char *cmdline_args = read_cmdline_tag(p_bootargs);

  init_heap(32*0x100000);
  test_heap_manager();

  print_uart0("arguments: ");
  print_uart0(cmdline_args);
  print_uart0("\n");
  print_uart0("CourseOS!\n");

  // Separate the command-line arguments into separate Strings
  int num_args = number_of_words(cmdline_args);
  char* arg_list[num_args];
  split_string(cmdline_args, arg_list);
  int arg_count = sizeof(arg_list) / sizeof(arg_list[0]);

  // Parse and analyze each String
  parse_arguments(arg_count, arg_list);

  // TODO: initialize GLOBAL_PID and PCB table
}

void test_heap_manager()
{
  uint32_t* block0 = kmalloc(8);
  uint32_t* block1 = kmalloc(10);
  uint32_t* block2 = kmalloc(13);
  mcheck();
  os_printf("\n");
  kfree(block0);
  mcheck();
  os_printf("\n");
  kfree(block2);
  mcheck();
  os_printf("\n");
  kfree(block1);
  os_printf("\n");
  mcheck();

}
