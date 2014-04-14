#include <stdint.h>
#include "include/mmap.h"
#include "include/vmlayout.h"
#include "include/klibc.h"
#include "include/pmap.h"


void main(void){
   print_vuart0("New supervisor stack at 0xefffd000\n");

   asm volatile(
      "eor r0, r0 \n\t"
      "MCR p15, 0, r0, c8, c7, 0 \n\t");

   first_level_pt[0] = 0;  
     
   asm volatile("wfi");
}
