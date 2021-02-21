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
