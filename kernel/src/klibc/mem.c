#include <string.h>

void * memcpy(void * dest, void * src, size_t count) {
    const char * sp = (const char *)src;
    char * dp = (char *)dest;
    for (; count != 0; count--) *dp++ = *sp++;
    return dp;
}

void * memset(void * dest, uint32_t val, size_t count) {
    char * temp = (char *)dest;
    for (; count != 0; count--) *temp++ = (char)val;
    return dest;
}

uint16_t * memsetw(uint16_t * dest, uint16_t val, size_t count) {
    uint16_t * temp = (uint16_t *)dest;
    for (; count != 0; count--) *temp++ = val;
    return dest;
}

void * memmove(void * dest, const void * src, size_t n) {
    unsigned char tmp[n];

    memcpy(tmp, (void *)src, n);
    memcpy(dest, tmp, n);

    return dest;
}
