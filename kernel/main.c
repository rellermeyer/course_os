#include "include/mmap.h"
#include "include/pmap.h"
#include "include/vmlayout.h"
#include "include/klibc.h"
#include "include/mem_alloc.h"
#include "include/interrupt.h"
#include "include/process.h"
#include "include/hw_handlers.h"
#include <stdint.h>
#include "timer.h"

void main(void){

  //flush TLB
  asm volatile(
    "eor r0, r0 \n\t"
    "MCR p15, 0, r0, c8, c7, 0 \n\t"); 
     
  print_vuart0("Virtual Memory (no paging yet)\n");  

  disable_interrupts();
  //Unmap one-to-one kernel and pt mappings
  *(v_first_level_pt+(KERNDSBASE>>20)) = 0;   
  *(v_first_level_pt) = 0;
  enable_interrupts();

  //NOTE: Make sure to init heap before
  //calling any malloc functions
  uint32_t* kheap = init_kheap(4096);

  /* Allocate in kernel region using k_malloc()
   * Allocate in user region using u_malloc()
   */

  //initialize GLOBAL_PID and PCB table
  v_printf("\nBegin Test\n");

  init_pcb_table();
  v_printf("\n");

  initialize_timers();

  test_heap_manager();

  uint32_t* test = kmalloc(sizeof(uint32_t*));
  v_printf("&test=%x\n", test);
  *test = 0x786;

  uint32_t* abt = 0xefb00000; 
  *abt = 0x786;
  
  asm volatile("svc 11");

  v_printf("\nEnd Test\n");

   /*initialize_timers();*/
   /*int timer_to_use = 2;*/
   /*set_load_value(timer_to_use, 1000);*/
   /*set_background_load_value(timer_to_use, 1000);*/
   /*start_timer(timer_to_use);*/
   /*os_printf("timer %d control = 0x%x\n",timer_to_use, timer_pointers[timer_to_use]->control);*/
   /*set_32_bit_mode(timer_to_use);*/
   /*os_printf("timer %d control = 0x%x\n",timer_to_use, timer_pointers[timer_to_use]->control);*/
   /*int i;*/
   /*for(i=0;i<10;i++) {*/
     /*os_printf("cur val = %d\n",get_current_value(timer_to_use));*/
   /*}*/
   /*get_time();*/
   /*//timer_start();*/

  asm volatile("wfi");


}

void test_heap_manager(){
  uint32_t* uheap1 = init_uheap(1024);

  uint32_t* block0 = umalloc(8);
  uint32_t* block1 = umalloc(10);
  uint32_t* block2 = umalloc(13);
  mcheck(uheap1, 1024);
  v_printf("\n");
  ufree(block0);
  mcheck(uheap1, 1024);
  v_printf("\n");
  ufree(block2);
  mcheck(uheap1, 1024);
  v_printf("\n");
  ufree(block1);
  v_printf("\n");
  mcheck(uheap1, 1024);

}
