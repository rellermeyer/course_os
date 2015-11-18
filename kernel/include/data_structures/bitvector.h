#ifndef BITVECTOR_H_
#define BITVECTOR_H_

#include <stdint.h>

typedef struct bit_vector {
	uint32_t length;
	uint32_t *vector;
	uint32_t actualLength;
} bit_vector;

/* creates butvector of "size" number of bits */
bit_vector *make_vector(uint32_t size);

/* gets the value of bit number "index" */
int32_t bv_get(uint32_t index, bit_vector* bit_vec);

/* flips the bit number "index" */
int32_t bv_toggle(uint32_t index, bit_vector* bit_vec);

/* sets the value of bit number "index" to 1 (meaning taken) */
int32_t bv_set(uint32_t index, bit_vector* bit_vec);

/* sets the value of bit number "index" to 0 (meaning free) */
int32_t bv_lower(uint32_t index, bit_vector* bit_vec);

/* returns the first free index (first 0 in the vector) */
int32_t bv_firstFree(bit_vector* bit_vec);

/* frees the memory used by the vector */
int32_t bv_free(bit_vector* bit_vec);

/* returns whether the given index is free - Noel*/
int32_t bv_isfree(uint32_t index, bit_vector* bit_vec);

/* transfer a bit_vector to disk */
int32_t bv_serialize(bit_vector* bit_vec, uint32_t start_block, uint32_t end_block);

/* transfer a bit_vector from disk */
int32_t bv_unserialize(bit_vector* bit_vec, uint32_t start_block, uint32_t end_block);

#endif 
