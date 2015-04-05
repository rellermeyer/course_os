#include <stdint.h>
#include <math.h>
#include <mem_alloc.h>
#include <bitvector.h>

// set is 1
// lowered is 0 (free)

uint64_t const WORD_SIZE = 64;
uint64_t length;
uint64_t *bitVec;

uint64_t *makeVector(uint32_t size) {
	// make the array 
	uint64_t bitVec = (uint64_t*) mem_alloc(ceil(size/64)); // Can I use ceil in the kernel?
	length = size;
	return bitVec;
}

uint32_t get (uint32_t index) {
	if(index < size) {
		uint64_t val = floor(index/WORD_SIZE); // can I use floor in the kernel?
		uint64_t oneWord = bitVec[val];
		oneWord >> 63 - (index % WORD_SIZE);
		uint64_t mask = 0x1;
		return oneWord & mask;
	} else {return 0;}
}

// toggles a given bit
uint32_t toggle (uint32_t index) {
	if(index < size) {
		uint64_t val = floor(index/WORD_SIZE); // can I use floor in the kernel?
		uint64_t oneWord = bitVec[val];
		uint64_t mask = 0x1 << 63 - (index % WORD_SIZE);
		bitVec[val] = oneWord ^ mask;
		return 1;
	} else { return 0;}  // invalid index
}

uint32_t set (uint32_t index) {
	if(index < size) {
		uint64_t val = floor(index/WORD_SIZE);
		uint64_t oneWord = bitVec[val];
		uint64_t mask = 0x1 << 63 - (index % WORD_SIZE);
		bitVec[val] = oneWord | mask;
		return 1;
	} else {return 0;}
}

uint32_t lower (uint32_t index) {
	if(index < size) {
		uint64_t val = floor(index/WORD_SIZE);
		uint64_t oneWord = bitVec[val];
		uint64_t mask = !(0x1 << 63 - (index % WORD_SIZE));
		bitVec[val] = oneWord & mask;
		return 1; 
	} else {return 0}
}

uint32_t firstFree () {
	uint64_t mask = 0x1;
	uint32_t returner = 0;
	uint64_t oneWord;
	for(uint64_t x = 0; x < length; x++) {
		uint64_t index = 0;
		while(index < WORD_SIZE) {
			oneWord =  bitVec[x];
			if(!(oneWord >> 63 - index) & mask) {
				return returner += index;
			} 
		} returner += WORD_SIZE;
	}
}