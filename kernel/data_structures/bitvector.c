#include <stdint.h>
#include "../include/klibc.h"
#include "../include/bitvector.h"

uint32_t const WORD_SIZE = 32;

uint32_t modulo (uint32_t n) {
	return n&~((2<<WORD_SIZE)-1);
}

/* return a pointer to a bitVector struct of length size */
bitVector *makeVector(uint32_t size) {
	// make the array 
	bitVector *BV = (bitVector*) kmalloc(sizeof(bitVector));
	if(modulo(size) == 0) {
		uint32_t array[size >> 5]; 
		uint32_t *bitVec = (uint32_t*) kmalloc(sizeof(array)); 
		BV->vector = bitVec;
		BV->length = size;
		return BV;
	} else {
		uint32_t array[(size >> 5) + 1]; 
		uint32_t *bitVec = (uint32_t*) kmalloc(sizeof(array)); 
		BV->vector = bitVec;
		BV->length = size;
		return BV;
	}
}

/* return a whatever number, 1 or 0 is at position index */
uint32_t get (uint32_t index, bitVector* bitVec) {
	if(index < bitVec->length) {
		uint32_t val = (index >> 5);
		uint32_t oneWord = bitVec->vector[val];
		oneWord = oneWord >> (31 - modulo(index));
		uint32_t mask = 0x1;
		return oneWord & mask;
	} else {return 0;} // invalid index
}

/* toggles the bit a position index */
uint32_t toggle (uint32_t index, bitVector* bitVec) {
	if(index < bitVec->length) {
		uint32_t val = (index >> 5);
		uint32_t oneWord = bitVec->vector[val];
		uint32_t mask = 0x1 << (31 - modulo(index));
		bitVec->vector[val] = oneWord ^ mask;
		return 1;
	} else { return 0;}  // invalid index
}

/* puts a 1 at the position index */
uint32_t set (uint32_t index, bitVector* bitVec) {
	if(index < bitVec->length) {
		uint32_t val = (index >> 5);
		uint32_t oneWord = bitVec->vector[val];
		uint32_t mask = 0x1 << (31 - modulo(index));
		bitVec->vector[val] = oneWord | mask;
		return 1;
	} else {return 0;} // invalid index
}

/* puts a 0 at the position index */
uint32_t lower (uint32_t index, bitVector* bitVec) {
	if(index < bitVec->length) {
		uint32_t val = (index >> 5);
		uint32_t oneWord = bitVec->vector[val];
		uint32_t mask = !(0x1 << (31 - (modulo(index))));
		bitVec->vector[val] = oneWord & mask;
		return 1; 
	} else {return 0;} // invalid index
}

/* returns the first free index, if none are free return -1 */
int32_t firstFree (bitVector* bitVec) {
	uint32_t mask = 0x1;
	uint32_t returner = 0;
	uint32_t oneWord;
	uint32_t x;
	for(x = 0; x < bitVec->length; x++) {
		uint32_t index = 0;
		while(index < WORD_SIZE) {
			oneWord = bitVec->vector[x];
			oneWord = (oneWord >> (63 - index)) & mask;
			if(!oneWord) {
				return returner += index;
			} 
		} returner += WORD_SIZE;
	}
	return -1; //no free spots
}
