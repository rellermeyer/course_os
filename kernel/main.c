#include "include/mmap.h"
#include "include/pmap.h"
#include "include/vmlayout.h"
#include "include/klibc.h"
#include "include/mem_alloc.h"
#include "include/interrupt.h"
#include "include/process.h"
#include "include/hw_handlers.h"
#include <stdint.h>

extern void* bootargs;

void main(void){

  //flush TLB
  asm volatile(
    "eor r0, r0 \n\t"
    "MCR p15, 0, r0, c8, c7, 0 \n\t"); 
     
  print_vuart0("Virtual Memory (no paging yet)\n");  
  
  // print_vuart0("arguments: ");
  // print_vuart0(read_cmdline_tag(bootargs));
  // print_vuart0("\n");

  disable_interrupts();
  //Unmap one-to-one kernel and pt mappings
  *(v_first_level_pt+(KERNDSBASE>>20)) = 0;   
  *(v_first_level_pt) = 0;
  enable_interrupts();

 /* 
  * NOTE: All function addresses need KERNSTART offset
  * otherwise, they will be the same as when they were
  * loaded, which is now unmapped. == ABORT
  * However, if you call them, then it seems to go to the
  * correct location.
  * 
  * Working on a fix.
  * 
  */

  //initialize GLOBAL_PID and PCB table
  int (*iproc)();
  iproc = &init_all_processes;
  v_printf("&init_all_processes=%x\n", iproc);

  init_all_processes();

  void(*handler_ptr)(void);
  handler_ptr = &data_abort_handler;
  v_printf("&handler=%x\n", handler_ptr);


  //data_abort_handler();

  // uint32_t* abt = 0xefb00000; 
  // *abt = 0x786;

  init_heap(4096);
  uint32_t* test = allocate(sizeof(uint32_t*));
  v_printf("&test=%x\n", test);
  *test = 0x786;

  // uint32_t* test2 = allocate(40);
  // v_printf("&test2=%x\n", test2);

  // deallocate(test);
  // uint32_t* test3 = allocate(sizeof(uint32_t*));
  // v_printf("&test3=%x\n", test);

  asm volatile("wfi");

}