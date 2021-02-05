#include <string.h>

char * strcpy(char * dst, char * src) {
    uint32_t i;
    for (i = 0; src[i] != 0; i++) { dst[i] = src[i]; }
    dst[i] = 0;
    return dst;
}

char * strncpy(char * dest, char * src, size_t n) {
    char * s = (char *)src;
    char * d = (char *)dest;
    while ((*d++ = *s++) && n--)
        ;
    return dest;
}

char * strcat(char * dst, char * src) {
    uint32_t i = 0;
    uint32_t j = 0;
    for (i = 0; dst[i] != 0; i++)
        ;
    for (j = 0; src[j] != 0; j++) { dst[i + j] = src[j]; }
    dst[i + j] = 0;
    return dst;
}

char * strcatc(char * dst, char src) {
    uint32_t i = 0;
    for (i = 0; dst[i] != 0; i++)
        ;
    dst[i + 0] = src;
    dst[i + 1] = 0;
    return dst;
}

uint32_t strcmp(char * s1, char * s2) {
    for (uint32_t i = 0;; i++) {
        if (s1[i] != s2[i]) { return s1[i] < s2[i] ? -1 : 1; }

        if (s1[i] == '\0') { return 0; }
    }
}

uint32_t strncmp(char * s1, char * s2, size_t n) {
    while (n && *s1 && (*s1 == *s2)) {
        ++s1;
        ++s2;
        --n;
    }
    if (n == 0) {
        return 0;
    } else {
        return (*(uint8_t *)s1 - *(uint8_t *)s2);
    }
}

uint32_t strlen(char * str) {
    for (uint32_t i = 0;; i++) {
        if (str[i] == '\0') { return i; }
    }
    return 0;
}

bool starts_with(char * s1, char * s2) {
    if (strncmp(s1, s2, strlen(s2)) == 0) { return true; }
    return false;
}

/*
 * Implementation taken from FreeBSD under BSD-3-Clause license.
 *
 * Copyright (c) 1990, 1993
 *     The Regents of the University of California.  All rights reserved.
 */
int memcmp(const void *s1, const void *s2, size_t n) {
    if (n != 0) {
        const unsigned char *p1 = s1, *p2 = s2;

        do {
            if (*p1++ != *p2++)
                return (*--p1 - *--p2);
        } while (--n != 0);
    }
    return (0);
}
