#ifndef QSTR_H
#define QSTR_H

#include <stdbool.h>
#include <stdint.h>

// A Qstr or quickstring, is a string which stores it's length,
// and on first compare calculates and stores its hash to speed up
// further comparisons. Apart from storing the length, the last byte of a Qstr's
// data is always a null. Therefore a pointer to a Qstr is *always* also a valid null
// terminated string.

typedef struct Qstr {
    char * data;
    size_t length;
    uint32_t hash;
} Qstr;

Qstr qstr_from_null_terminated_string(char * arr);
Qstr qstr_from_length_string(char * arr, size_t length);

void qstr_free(Qstr * qstr);

// Quick eq. Either compares hashes and lengths to quickly determine non equality.
// While doing full string compare, calculates the hash of left to store. If equal
// this automatically also is the hash of right and both are set. Otherwise just the
// hash of left is set.
bool qstr_eq(Qstr * left, Qstr * right);

// Normal strincompare. Slightly slower than default compare.
// Also calculates hash of left to speed up further stringcompares.
bool qstr_eq_null_terminated(Qstr * left, char * right);

// Hash the qstring
void qstr_hash(Qstr * q);

#endif
