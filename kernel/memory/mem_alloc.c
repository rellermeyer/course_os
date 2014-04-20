#include "mem_alloc.h"
#include "vmlayout.h"
#include "pmap.h"
#include "../include/klibc.h"


uint32_t* next_ublock = UHEAPSTART;
uint32_t* next_kblock = KHEAPSTART;

//bump pointer allocation
uint32_t* mem_alloc(uint32_t size, priv_t priv) {

	uint32_t* alloc_block = 0;

	uint32_t temp = size / 4;

	if((size % 4) > 0) {
		temp++;
	}

	if(priv == KERN){
		alloc_block = next_kblock;
		next_kblock += temp;
	}

	if(priv == USER){
		alloc_block = next_ublock;
		next_ublock += temp;
	}
	//v_printf("returning block %x\n", alloc_block);
	return alloc_block;
}

uint32_t* u_malloc(uint32_t size){
	uint32_t* block = mem_alloc(size, USER);
	return block;
}

uint32_t* k_malloc(uint32_t size){
	uint32_t* block = mem_alloc(size, KERN);
	return block;
}
