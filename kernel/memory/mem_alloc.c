#include "mem_alloc.h"
#include "vmlayout.h"
#include "pmap.h"
#include "../include/klibc.h"


uint32_t* next_ublock = UHEAPSTART;
uint32_t* next_kblock = KHEAPSTART;

//bump pointer allocation
uint32_t* mem_alloc(uint32_t size, uint32_t priv) {

	uint32_t* alloc_block = 0;

	uint32_t temp = size / 4;

	if((size % 4) > 0) {
		temp++;
	}

	if(priv == 0){
		alloc_block = next_kblock;
		next_kblock += temp;
	}

	if(priv == 1){
		alloc_block = next_ublock;
		next_ublock += temp;
	}
	v_printf("returning block=%x\n", alloc_block);	
	//asm volatile("wfi");
	return alloc_block;
}

uint32_t* u_malloc(uint32_t size){
	return mem_alloc(size, 0);
}

uint32_t* k_malloc(uint32_t size){
  int pc, lr, sp, fp;
  asm volatile("mov %0, pc" : "=r" (pc));
  asm volatile("mov %0, lr" : "=r" (lr));
  asm volatile("mov %0, sp" : "=r" (sp));
  asm volatile("mov %0, fp" : "=r" (fp));
  v_printf("k_malloc: pc=%x, lr=%x, sp=%x, fp=%x\n", pc, lr, sp, fp);
	return mem_alloc(size, 1);
}
