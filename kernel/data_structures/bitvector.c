#include <stdint.h>
#include "../include/klibc.h"
#include "../include/bitvector.h"

uint32_t const WORD_SIZE = 32;

/* return a pointer to a bitVector struct of length size */
bitVector *makeVector(uint32_t size) {
	// make the array 
	bitVector *BV = (bitVector*) kmalloc(sizeof(bitVector));
	uint32_t val = size % 32;
	if((size % 32) == 0) {
		uint32_t array[size >> 5];
		uint32_t x;
		for(x = 0; (x < (size >> 5)); x++){
			array[x] = 0;
		}
		uint32_t *bitVec = (uint32_t*) kmalloc(sizeof(array)); 
		BV->vector = bitVec;
		BV->length = size;
		BV->actualLength = (size >> 5);
		return BV;
	} else {
		uint32_t array[(size >> 5) + 1]; 
		uint32_t y;
		for(y = 0; (y < ((size >> 5)+1)); y++){
			array[y] = 0;
		}
		uint32_t *bitVec = (uint32_t*) kmalloc(sizeof(array)); 
		BV->vector = bitVec;
		BV->length = size;
		BV->actualLength = (size >> 5) + 1;
		return BV;
	}
}

/* return a whatever number, 1 or 0 is at position index */
int32_t bv_get (uint32_t index, bitVector* bitVec) {
	os_printf("index is: %u\n", index);
	if(index < bitVec->length && index >= 0) {
		os_printf("We are in the wrong place...\n");
		uint32_t val = (index >> 5);
		uint32_t oneWord = bitVec->vector[val];
		oneWord = oneWord >> (31 - (index % WORD_SIZE));
		uint32_t mask = 0x1;
		return oneWord & mask;
	} else {return -1;} // invalid index
}

/* toggles the bit a position index */
int32_t bv_toggle (uint32_t index, bitVector* bitVec) {
	if(index < bitVec->length && index >= 0) {
		uint32_t val = (index >> 5);
		uint32_t oneWord = bitVec->vector[val];
		uint32_t mask = 0x1 << (31 - (index % WORD_SIZE));
		bitVec->vector[val] = oneWord ^ mask;
		return 1;
	} else {return -1;}  // invalid index
}

/* puts a 1 at the position index */
int32_t bv_set (uint32_t index, bitVector* bitVec) {
	if(index < bitVec->length && index >= 0) {
		uint32_t val = (index >> 5);
		uint32_t oneWord = bitVec->vector[val];
		uint32_t mask = 0x1 << (31 - (index % WORD_SIZE));
		bitVec->vector[val] = oneWord | mask;
		return 1;
	} else {return -1;} // invalid index
}

/* puts a 0 at the position index */
int32_t bv_lower (uint32_t index, bitVector* bitVec) {
	if(index < bitVec->length && index >= 0) {
		uint32_t val = (index >> 5);
		uint32_t oneWord = bitVec->vector[val];
		uint32_t mask = !(0x1 << (31 - (index % WORD_SIZE)));
		bitVec->vector[val] = oneWord & mask;
		return 1; 
	} else {return -1;} // invalid index
}

/* returns the first free index, if none are free return -1 */
int32_t bv_firstFree (bitVector* bitVec) {
	uint32_t mask = 0x1;
	uint32_t returner = 0;
	uint32_t oneWord;
	uint32_t x;
	oneWord = bitVec->vector[0];
	for(x = 0; x < bitVec->actualLength; x++) {
		uint32_t index = 0;
		while(index < WORD_SIZE) {
			oneWord = bitVec->vector[x];
			oneWord = (oneWord >> (31 - index)) & mask;
			if(!oneWord) {
				return returner += index;
			} 
			index++;
		} returner += WORD_SIZE;
	}
	return -1; //no free spots
}

int32_t bv_free (bitVector* bitVec) {
	kfree(bitVec->vector);
	kfree(bitVec);
	return 1;
}