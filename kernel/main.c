#include "include/mmap.h"
#include "include/pmap.h"
#include "include/vmlayout.h"
#include "include/klibc.h"
#include "include/mem_alloc.h"
#include <stdint.h>

extern void* bootargs;

void main(void){
  print_vuart0("Running at virtual stack location\n");  
  print_vuart0("arguments: ");
  print_vuart0(read_cmdline_tag(bootargs));
  print_vuart0("\n");
  
  //flush TLB
  asm volatile(
    "eor r0, r0 \n\t"
    "MCR p15, 0, r0, c8, c7, 0 \n\t");  

  v_printf("v_first_level_pt addr=%x\n", v_first_level_pt);

  //Unmap one-to-one kernel and pt mappings
  *(v_first_level_pt+(KERNDSBASE>>20)) = 0;   
  *(v_first_level_pt) = 0; 

  //initialize GLOBAL_PID and PCB table
  init_all_processes();

  uint32_t* test2 = u_malloc(sizeof(uint32_t*));
  v_printf("testaddr=%x\n", test2);
  *test2 = 0x786;

  asm volatile("wfi");

}