#include <qstr.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

Qstr qstr_from_null_terminated_string(char * arr) {
    size_t length = strlen(arr);
    return qstr_from_length_string(arr, length);
}

Qstr qstr_from_length_string(char * arr, size_t length) {
    char * data = kmalloc(length + 1);
    memcpy(data, arr, length);
    data[length] = 0; // make null terminated.

    return (Qstr){
            .data = data,
            .length = length,
            .hash = 0,
    };
}

void qstr_free(Qstr* qstr) {
    kfree(qstr->data);
}

static inline bool __eq(Qstr * left, Qstr * right, bool fake) {
    // Hash function from http://www.cse.yorku.ca/~oz/hash.html (djb2)
    char * rightstr;
    if(!fake){
        if (left->hash != 0 && right->hash != 0 && left->hash != right->hash) {
            return false;
        }
        if(left->length != right->length) {
            return false;
        }
        rightstr = right->data;
    } else {
        rightstr = (char *) right;
    }

    char * leftstr = left->data;

    uint32_t hash  = 5381;

    bool equal = true;
    char c;
    while ((c = *(leftstr++)) != '\0') {
        hash = ((hash << 5) + hash) + c;  /* hash * 33 + c */

        if (c != (*rightstr++)) {
            equal = false;
        }
    }

    if (hash == 0) {
        hash = 42; // just has to be non-zero as to not be confused with empty hash.
    }

    left->hash = hash;
    if (equal && !fake) {
        right->hash = hash;
    }

    return equal;
}

bool qstr_eq(Qstr * left, Qstr * right) {
    return __eq(left, right, false);
}

bool qstr_eq_null_terminated(Qstr * left, char * right) {
    return __eq(left, (Qstr *) right, true);
}
