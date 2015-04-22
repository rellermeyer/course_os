#ifndef BITVECTOR_H_
#define BITVECTOR_H_

#include <stdint.h>

typedef struct bitVector {
	uint32_t length;
	uint32_t *vector;
	uint32_t actualLength;
} bitVector;

bitVector *makeVector(uint32_t);
int32_t get(uint32_t, bitVector*);
int32_t toggle(uint32_t, bitVector*);
int32_t set(uint32_t, bitVector*);
int32_t lower(uint32_t, bitVector*);
int32_t firstFree(bitVector*);

#endif 