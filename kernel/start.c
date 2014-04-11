#include "include/global_defs.h"
#include <stdint.h>
#include "include/argparse.h"

void start(void *p_bootargs) {
   print_uart0("arguments: ");
   print_uart0(read_cmdline_tag(p_bootargs));
   print_uart0("\n");
   print_uart0("CourseOS!\n");
   mmap();

   //Test: UART0 mapped to the correct virtual address
   print_vuart0("Virtual Memory!!!\n");

   //TODO: more test to verify layout
   //test_vmlayout();

   //Unmap where the kernel was originally loaded
   unmap();

   //returns and waits for interrupt in startup.s
   //uncomment below to wait here instead
   //asm volatile("wfi");
}

