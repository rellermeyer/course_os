/*
Log
4/2: Adjusted bump allocation algorithm: Sean V, Faseeh A, John G, Taylor S
4/7: Fixed mem_alloc again works properly: Sean V, Faseeh A, Taylor S.

*/
#ifndef __MEM_ALLOC_H__
#define __MEM_ALLOC_H__

#include <stdint.h> 

#define MEM_START 0x500000

char* heap;
uint32_t heap_size;

uint32_t* mem_alloc(uint32_t);
void* init_heap(uint32_t);
char* allocate(uint32_t, char*, int32_t);
void deallocate(void*, char*, int32_t);
void mcheck();

#endif