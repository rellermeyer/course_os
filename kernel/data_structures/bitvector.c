#include <stdint.h>
#include "klibc.h"
#include "data_structures/bitvector.h"

uint32_t const WORD_SIZE = 32;

/* return a pointer to a bit_vector struct of length size */
bit_vector *make_vector(uint32_t size) {
	// make the array 
	bit_vector *BV = (bit_vector*) kmalloc(sizeof(bit_vector));
	if((size % 32) == 0) {
		uint32_t array[size >> 5];
		uint32_t x;
		for(x = 0; (x < (size >> 5)); x++){
			array[x] = 0;
		}
		uint32_t *bit_vec = (uint32_t*) kmalloc(sizeof(array)); 
		BV->vector = bit_vec;
		BV->length = size;
		BV->actualLength = (size >> 5);
		return BV;
	} else {
		uint32_t array[(size >> 5) + 1]; 
		uint32_t y;
		for(y = 0; (y < ((size >> 5)+1)); y++){
			array[y] = 0;
		}
		uint32_t *bit_vec = (uint32_t*) kmalloc(sizeof(array)); 
		BV->vector = bit_vec;
		BV->length = size;
		BV->actualLength = (size >> 5) + 1;
		return BV;
	}
}

/* return a whatever number, 1 or 0 is at position index */
int32_t bv_get (uint32_t index, bit_vector* bit_vec) {
	
    if(index < bit_vec->length && index >= 0) {
		
        uint32_t val = (index >> 5);
		uint32_t oneWord = bit_vec->vector[val];
		oneWord = oneWord >> (31 - (index % WORD_SIZE));
		uint32_t mask = 0x1;
		return oneWord & mask;
	} else {return -1;} // invalid index
}

/* toggles the bit a position index */
int32_t bv_toggle (uint32_t index, bit_vector* bit_vec) {
	
    if(index < bit_vec->length && index >= 0) {
		
        uint32_t val = (index >> 5);
		uint32_t oneWord = bit_vec->vector[val];
		uint32_t mask = 0x1 << (31 - (index % WORD_SIZE));
		bit_vec->vector[val] = oneWord ^ mask;
		return 1;
	} else {return -1;}  // invalid index
}

/* puts a 1 at the position index */
int32_t bv_set (uint32_t index, bit_vector* bit_vec) {
	
    if(index < bit_vec->length && index >= 0) {
		
        uint32_t val = (index >> 5);
		uint32_t oneWord = bit_vec->vector[val];
		uint32_t mask = 0x1 << (31 - (index % WORD_SIZE));
		bit_vec->vector[val] = oneWord | mask;
		return 1;
	} else {return -1;} // invalid index
}

/* puts a 0 at the position index */
int32_t bv_lower (uint32_t index, bit_vector* bit_vec) {
	
    if(index < bit_vec->length && index >= 0) {
		
        uint32_t val = (index >> 5);
		uint32_t oneWord = bit_vec->vector[val];
		uint32_t mask = ~(0x1 << (31 - (index % WORD_SIZE)));
		bit_vec->vector[val] = oneWord & mask;
		return 1; 
	} else {return -1;} // invalid index
}

/* returns the first free index, if none are free return -1 */
int32_t bv_firstFree (bit_vector* bit_vec) {
	
    uint32_t mask = 0x1;
	uint32_t returner = 0;
	uint32_t oneWord;
	uint32_t x;
	oneWord = bit_vec->vector[0];
	
    for(x = 0; x < bit_vec->actualLength; x++) {
		
        uint32_t index = 0;
		while(index < WORD_SIZE) {
			
            oneWord = bit_vec->vector[x];
			oneWord = (oneWord >> (31 - index)) & mask;
			
            if(!oneWord) {
				return returner += index;
			} 
			index++;
		} returner += WORD_SIZE;
	}
	return -1; //no free spots
}

/* returns whether the given index is free; 1 - true, 0 - false, -1 - invalid index*/
int32_t bv_isfree(uint32_t index, bit_vector* bit_vec)
{
	uint32_t val;
	uint32_t word;
	if (index < bit_vec->length && index >= 0){
		val = (index >> 5);
		word = bit_vec->vector[val];
		word = (word >> (31 - index)) & 0x1;
		if (!word){
			return 1;
		}
		return 0;	
	}	
	return -1;
}

/* free the bv from memory */
int32_t bv_free (bit_vector* bit_vec) {
	
    kfree(bit_vec->vector);
	kfree(bit_vec);
	return 1;
}
