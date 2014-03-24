#include "mem_alloc.h"

static const uint32_t MEM_START = 0x500000;

uint32_t nextBlock = 0x500000;  

void *mem_alloc(uint32_t size) {
    int actualSize;
    void *allocBlock = (void *)nextBlock;

    actualSize = (size % 8 > 0) ? (size + 8) % 8 : size;
    nextBlock += actualSize;
    
    return allocBlock;
}
