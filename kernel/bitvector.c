#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <../include/mem_alloc.h>
#include <../include/bitvector.h>


uint64_t *makeVector(uint32_t size) {
	// make the array 
	uint64_t bitVec = (uint64_t*) mem_alloc(ceil(size/64)); // Can I use ceil in the kernel?
	return bitVec;
}

// toggles a given bit
// bug when you toggle the last or first bit of the array, will work on tomorrow 
uint32_t toggle (uint32_t index) {
	uint64_t val = floor(index/64); // can I use floor in the kernel?
	uint64_t onePart = bitVec[val];
	uint64_t mask = 0x1 << (index - (val * 64));
	bitVec[val] = bitVec[val] ^ mask;
	return 1;
}