#include <stdint.h>
#include <math.h>
#include "../include/mem_alloc.h"
#include "../include/bitvector.h"

// in the .h file: struct thread
// struct thread *t1 = thread_create(f1, NULL);

// set is 1
// lowered is 0 (free)

uint32_t const WORD_SIZE = 32;

uint32_t floor(uint32_t n) {
	return n - (n % 1);
}
// uint64_t *bitVec;

bitVector *makeVector(uint32_t size) {
	// make the array 
	bitVector *BV = (bitVector*) mem_alloc(sizeof(struct bitVector));
	if(size % WORD_SIZE == 0) {
		uint32_t array[size/32]; // Can I use ceil in the kernel?
		uint32_t *bitVec = (uint32_t*) mem_alloc(sizeof(array)); 
		BV->vector = bitVec;
		BV->length = size;
		return BV;
	} else {
		uint32_t array[(size/32) + 1]; // Can I use ceil in the kernel?
		uint32_t *bitVec = (uint32_t*) mem_alloc(sizeof(array)); 
		BV->vector = bitVec;
		BV->length = size;
		return BV;
	}
}

uint32_t get (uint32_t index, bitVector* bitVec) {
	if(index < bitVec->length) {
		uint32_t val = floor(index/WORD_SIZE); // can I use floor in the kernel?
		uint32_t oneWord = bitVec->vector[val];
		oneWord >> 31 - (index % WORD_SIZE);
		uint32_t mask = 0x1;
		return oneWord & mask;
	} else {return 0;}
}

// toggles a given bit
uint32_t toggle (uint32_t index, bitVector* bitVec) {
	if(index < bitVec->length) {
		uint32_t val = floor(index/WORD_SIZE); // can I use floor in the kernel?
		uint32_t oneWord = bitVec->vector[val];
		uint32_t mask = 0x1 << 31 - (index % WORD_SIZE);
		bitVec->vector[val] = oneWord ^ mask;
		return 1;
	} else { return 0;}  // invalid index
}

uint32_t set (uint32_t index, bitVector* bitVec) {
	if(index < bitVec->length) {
		uint32_t val = floor(index/WORD_SIZE);
		uint32_t oneWord = bitVec->vector[val];
		uint32_t mask = 0x1 << 31 - (index % WORD_SIZE);
		bitVec->vector[val] = oneWord | mask;
		return 1;
	} else {return 0;}
}

uint32_t lower (uint32_t index, bitVector* bitVec) {
	if(index < bitVec->length) {
		uint32_t val = floor(index/WORD_SIZE);
		uint32_t oneWord = bitVec->vector[val];
		uint32_t mask = !(0x1 << 31 - (index % WORD_SIZE));
		bitVec->vector[val] = oneWord & mask;
		return 1; 
	} else {return 0;}
}

uint32_t firstFree (bitVector* bitVec) {
	uint32_t mask = 0x1;
	uint32_t returner = 0;
	uint32_t oneWord;
	uint32_t x;
	for(x = 0; x < bitVec->length; x++) {
		uint32_t index = 0;
		while(index < WORD_SIZE) {
			oneWord = bitVec->vector[x];
			if(!(oneWord >> 63 - index) & mask) {
				return returner += index;
			} 
		} returner += WORD_SIZE;
	}
	return NULL; //no free spots
}
