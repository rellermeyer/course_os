#include "include/global_defs.h"
#include <stdint.h>
#include "include/argparse.h"
#include "include/mmap.h"

//extern void setup_stacks(void);

void start(void *p_bootargs) {
   print_uart0("arguments: ");
   print_uart0(read_cmdline_tag(p_bootargs));
   print_uart0("\n");
   print_uart0("CourseOS!\n");
   
   mmap();

   //Test: UART0 mapped to the correct virtual address   
   print_vuart0("Virtual Memory!!!\n");

   //setup new stack pointers
   //setup_stacks();
   asm volatile("b _stacks");

   /*
   //flush TLB
   asm volatile(
      "eor r0, r0 \n\t"
      "MCR p15, 0, r0, c8, c7, 0 \n\t");
   */

}


