/**
 * An implementation of the memory methods of string.h which,
 * for historical reasons, has been implemented into this separate
 * file rather than with the other methods.
 *
 * All mem methods have been implemented. In future these should
 * probably optimized further, possibly written in Assembly.
 */
#include <string.h>
#include <stdint.h>

/**
 * Copy a number of bytes from the source to the destination
 * buffer.
 */
void * memcpy(void * dest, void * src, size_t count) {
    const char * sp = (const char *)src;
    char * dp = (char *)dest;
    for (; count != 0; count--) *dp++ = *sp++;
    return dp;
}

/**
 * Set n dwords of the destination buffer to some value.
 */
void * memset(void * dest, uint32_t val, size_t count) {
    char * temp = (char *)dest;
    for (; count != 0; count--) *temp++ = (char)val;
    return dest;
}

/**
 * Set n words of the destination buffer to some value.
 */
uint16_t * memsetw(uint16_t * dest, uint16_t val, size_t count) {
    uint16_t * temp = (uint16_t *)dest;
    for (; count != 0; count--) *temp++ = val;
    return dest;
}

/**
 * Copy the first n bytes of the source to a destination buffer.
 */
void * memmove(void * dest, const void * src, size_t n) {
    unsigned char tmp[n];

    memcpy(tmp, (void *)src, n);
    memcpy(dest, tmp, n);

    return dest;
}

/**
 * Compare the first n characters of any two objects.
 */
int32_t memcmp(const void *s1, const void *s2, size_t n) {
    // TODO: Assembly
    int32_t result = 0;

    // For ANSI C99 compliance, you could speed up the code
    // a bit by ignoring this requirement.
    if (n == 0) {
        return 0;
    }

    // We don't need to check whether s2 is NULL since in that case result != 0
    do {
        result = *(const unsigned char *) s1++ - *(const unsigned char*) s2++;
    } while (*(const unsigned char *) s1 != '\0' && result == 0 && n-- != 0);

    return result;
}

/**
 * Get the first occurence of some double word in the
 * first n bytes of a source.
 **/
void *memchr(const void *s, int32_t c, size_t n) {
    unsigned char *p =  (unsigned char *) s;

    while (*p != '\0' && n-- != 0) {
        if (*p == (unsigned char) c) {
            return p;
        }

        p++;
    }

    return NULL;
}
