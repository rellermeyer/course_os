#include "include/global_defs.h"
#include <stdint.h>
#include "include/argparse.h"
#include "include/timer.h"


void start(void *p_bootargs) {
   print_uart0("arguments: ");
   print_uart0(read_cmdline_tag(p_bootargs));
   print_uart0("\n");
   print_uart0("CourseOS!\n");
   initialize_timers();
   timer_start();
}

