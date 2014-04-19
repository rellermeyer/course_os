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

  int pc, lr, sp, fp;
  asm volatile("mov %0, pc" : "=r" (pc));
  asm volatile("mov %0, lr" : "=r" (lr));
  asm volatile("mov %0, sp" : "=r" (sp));
  asm volatile("mov %0, fp" : "=r" (fp));
  v_printf("MAIN1: pc=%x, lr=%x, sp=%x, fp=%x\n", pc, lr, sp, fp);  

  //Unmap one-to-one kernel and pt mappings
  //*(v_first_level_pt+(KERNDSBASE>>20)) = 0;   
  //*(v_first_level_pt) = 0;

  unsigned int entry_addr = (v_first_level_pt);   
  v_printf("entry_addr=%x\n", entry_addr);

  unsigned int entry_addr_off = (v_first_level_pt + KERNSTART);   
  v_printf("entry_addr_off=%x\n", entry_addr_off);  


  // asm volatile("mov %0, pc" : "=r" (pc));
  // asm volatile("mov %0, lr" : "=r" (lr));
  // asm volatile("mov %0, sp" : "=r" (sp));
  // asm volatile("mov %0, fp" : "=r" (fp));
  // v_printf("MAIN1: pc=%x, lr=%x, sp=%x, fp=%x\n", pc, lr, sp, fp);


  //  Allocate in kernel region using k_malloc()
  //  * Allocate in user region using u_malloc()
   
  // uint32_t * test = mem_alloc(sizeof(uint32_t*), KERN);
  // if(test == 0){
  //   v_printf("mem_alloc error\n");
  // }
  // v_printf("testaddr=%x\n", test);
  // *test = 0x33333;
  // //initialize GLOBAL_PID and PCB table
  // init_all_processes();

  asm volatile("wfi");

}