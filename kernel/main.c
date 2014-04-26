#include "include/mmap.h"
#include "include/pmap.h"
#include "include/vmlayout.h"
#include "include/klibc.h"
#include "include/mem_alloc.h"
#include "include/interrupt.h"
#include "include/process.h"
#include "include/hw_handlers.h"
#include <stdint.h>

void main(void){

  //flush TLB
  asm volatile(
    "eor r0, r0 \n\t"
    "MCR p15, 0, r0, c8, c7, 0 \n\t"); 
     
  print_uart0("Virtual Memory (no paging yet)\n");  

  disable_interrupts();
  //Unmap one-to-one kernel and pt mappings
  *(v_first_level_pt+(KERNDSBASE>>20)) = 0;   
  *(v_first_level_pt) = 0;
  enable_interrupts();

  //NOTE: Make sure to init heap before
  //calling any malloc functions
  uint32_t* kheap = init_kheap(4096);
  //initialize GLOBAL_PID and PCB table  
  init_all_processes();

  uint32_t* file1 = kmalloc(sizeof(uint32_t*));
  pcb* p1 = process_create(file1);
  execute_process(p1);

  uint32_t* file2 = kmalloc(sizeof(uint32_t*));
  pcb* p2 = process_create(file2);
  execute_process(p2);  

  os_printf("\nBegin Test\n");

  test_heap_manager();

  uint32_t* test = kmalloc(sizeof(uint32_t*));
  os_printf("&test=%x\n", test);
  *test = 0x786;

  uint32_t* abt = 0xefb00000; 
  *abt = 0x786;
  
  asm volatile("svc 11");

  os_printf("\nEnd Test\n");

  asm volatile("wfi");

}

void test_heap_manager(){
  uint32_t* uheap1 = init_uheap(1024);

  uint32_t* block0 = umalloc(8);
  uint32_t* block1 = umalloc(10);
  uint32_t* block2 = umalloc(13);
  mcheck(uheap1, 1024);
  os_printf("\n");
  ufree(block0);
  mcheck(uheap1, 1024);
  os_printf("\n");
  ufree(block2);
  mcheck(uheap1, 1024);
  os_printf("\n");
  ufree(block1);
  os_printf("\n");
  mcheck(uheap1, 1024);

}
