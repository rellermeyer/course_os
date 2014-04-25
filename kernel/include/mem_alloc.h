/*
Log
4/2: Adjusted bump allocation algorithm: Sean V, Faseeh A, John G, Taylor S
4/7: Fixed mem_alloc again works properly: Sean V, Faseeh A, Taylor S.

*/

#include <stdint.h> 

typedef enum{
	KERN,
	USER,
} priv_t;

uint32_t* mem_alloc(uint32_t, priv_t);
uint32_t * umalloc(uint32_t size);
uint32_t * kmalloc(uint32_t size);
