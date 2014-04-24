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
   int timer_to_use = 2;
   set_load_value(timer_to_use, 1000);
   set_background_load_value(timer_to_use, 1000);
   start_timer(timer_to_use);
   os_printf("timer %d control = 0x%x\n",timer_to_use, timer_pointers[timer_to_use]->control);
   set_32_bit_mode(timer_to_use);
   os_printf("timer %d control = 0x%x\n",timer_to_use, timer_pointers[timer_to_use]->control);
   int i;
   for(i=0;i<10;i++) {
     os_printf("cur val = %d\n",get_current_value(timer_to_use));
   }
   get_time();
   //timer_start();
}

