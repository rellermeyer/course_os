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

   //static unsigned int * first_level_pt = L1PTBASE;

   mmap();//first_level_pt);
   //Test: UART0 mapped to the correct virtual address   
   print_vuart0("Virtual Memory!!!\n");

   //Copy over the page table entries into virtual page table
   int i;
   //for(i=0;i<4096;i++){
      //*(v_first_level_pt+i) = first_level_pt[i];
   //}
      //asm volatile("wfi");



   //flush TLB
   asm volatile(
      "eor r0, r0 \n\t"
      "MCR p15, 0, r0, c8, c7, 0 \n\t");

   //asm volatile("wfi");
   //setup new stack pointers
   asm volatile (".include \"stacks.s\"");
  
   print_vuart0("New supervisor stack at 0xefffd000\n");


   //Confirm new stack by push test value
   asm volatile(
      "ldr r0, =0x313  \n"
      "push {r0}");   

   asm volatile("wfi");
}
