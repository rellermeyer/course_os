#include "include/mmap.h"
#include "include/pmap.h"
#include "include/vmlayout.h"
#include "include/klibc.h"

void main(void){
  print_vuart0("Running at virtual stack location\n");  

  //flush TLB
  asm volatile(
    "eor r0, r0 \n\t"
    "MCR p15, 0, r0, c8, c7, 0 \n\t");

  //Unmap one-to-one kernel and pt mappings
  *(v_first_level_pt+KERNSTART+(KERNDSBASE>>20)) = 0;   
  *(v_first_level_pt+KERNSTART) = 0;

  /* Allocate in kernel region using k_malloc()
   * Allocate in user region using u_malloc()
   */

  //initialize GLOBAL_PID and PCB table
  init_all_processes();

  asm volatile("wfi");

}