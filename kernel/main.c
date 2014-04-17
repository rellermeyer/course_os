#include "include/mmap.h"
#include "include/pmap.h"
#include "include/vmlayout.h"
#include "include/klibc.h"

void main(void){
   print_vuart0("New supervisor stack at 0xefffd000\n");

   //Confirm new stack by push test value
   asm volatile(
      "ldr r0, =0x313 \n\t"
      "push {r0}");   

   int i;
   for(i=0;i<4096;i++){
  		if((i>=2048 && i<=2050) || (i>=3835 && i<=3840)){ 	
    		os_printf("i= %d\n",i);
    		os_printf("l1pt addr= %x\t", (first_level_pt+i));  
   	   		os_printf("l1pte= %x\n", *(first_level_pt+i));  
   	   		os_printf("l1vpt addr= %x\t", (v_first_level_pt+i));  
       		os_printf("l1vpte= %x\n", *(v_first_level_pt+i));  
 		}		
   }

   //Unmap one-to-one kernel and pt mappings
   *(first_level_pt+127) = 0;   
   *(first_level_pt) = 0;

   v_first_level_pt[1] = 0x100000|0x0400|2;

   //int x = *(v_first_level_pt+0xf0000000+3840);
   //asm volatile("mov r0, %0" : : "r" (x));

   asm volatile("wfi");


}