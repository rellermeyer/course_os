#include <stdint.h>


#include "include/argparse.h"


void start(void *p_bootargs) {
   print_uart0("arguments ");
   print_uart0(44 + p_bootargs);
   print_uart0("\n");
   print_uart0("CourseOS!\n");
//   copy_cmdline_tag((uint32_t)(&p_bootargs));
   copy_cmdline_tag(p_bootargs);
}

