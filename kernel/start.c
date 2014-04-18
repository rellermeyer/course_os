#include "include/global_defs.h"
#include <stdint.h>
#include "include/argparse.h"
#include "include/mmap.h"
#include "include/pmap.h"
#include "include/vmlayout.h"

void start(void *p_bootargs) {
   print_uart0("arguments: ");
   print_uart0(read_cmdline_tag(p_bootargs));
   print_uart0("\n");
   print_uart0("CourseOS!\n");
   init_vector_table();


   mmap();

   //Test: UART0 mapped to the correct virtual address   
   print_vuart0("Virtual Memory!!!\n");


   //setup new stack pointers
   asm volatile (".include \"stacks.s\"");
  

   asm volatile("wfi");
}
