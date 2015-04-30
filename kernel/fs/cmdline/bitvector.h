#ifndef BITVECTOR_H
#define BITVECTOR_H

#include<stdint.h>

typedef struct bit_vector {
        uint32_t length;
        uint32_t *vector;
        uint32_t actualLength;
} bit_vector;

#endif
