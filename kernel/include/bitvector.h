#ifndef BITVECTOR_H_
#define BITVECTOR_H_

#include <stdint.h>

typedef struct bitVector {
	uint32_t length;
	uint32_t *vector;
	uint32_t actualLength;
} bitVector;

bitVector *makeVector(uint32_t size); //creates butvector of "size" number of bits
int32_t bv_get(uint32_t index, bitVector* bitVec); //gets the value of bit number "index"
int32_t bv_toggle(uint32_t index, bitVector* bitVec); //flips the bit number "index"
int32_t bv_set(uint32_t index, bitVector* bitVec); //sets the value of bit number "index" to 1 (meaning taken)
int32_t bv_lower(uint32_t index, bitVector* bitVec); //sets the value of bit number "index" to 0 (meaning free)
int32_t bv_firstFree(bitVector* bitVec); //returns the first free index (first 0 in the vector)
int32_t bv_free(bitVector* bitVec); //frees the memory used by the vector

#endif 