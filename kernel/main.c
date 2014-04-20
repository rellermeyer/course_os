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
  
  print_vuart0("arguments: ");
  print_vuart0(read_cmdline_tag(bootargs));
  print_vuart0("\n");

  disable_interrupts();
  //Unmap one-to-one kernel and pt mappings
  *(v_first_level_pt+(KERNDSBASE>>20)) = 0;   
  *(v_first_level_pt) = 0;
  enable_interrupts();

  //initialize GLOBAL_PID and PCB table
  int (*iproc)();
  iproc = &init_all_processes;
  v_printf("&init_all_processes=%x\n", iproc);

  init_all_processes();

  void(*handler_ptr)(void);
  handler_ptr = &data_abort_handler;
  v_printf("&handler=%x\n", handler_ptr);

  data_abort_handler();

  uint32_t* abt = 0xefb00000; 
  //*abt = 0x786;

  asm volatile("wfi");

}