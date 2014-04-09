#include "mem_alloc.h"
#include "../include/klibc.h"

static const uint32_t MEM_START = 0x500000;

uint32_t *nextBlock = 0x500000;  

//bump pointer allocation
uint32_t * mem_alloc(uint32_t size) {
	uint32_t* allocBlock = nextBlock;
    nextBlock = nextBlock + size * 8;
   
   	//Debug
    //os_printf("%d\n", nextBlock);
    return allocBlock;
}
