/**
 * This file implements the string library as defined by the ANSI C99
 * standard for those functions useful for course_os. Currently, the
 * following methods have an implementation.
 *
 * - strcpy
 * - strncpy
 * - strcmp
 * - strncmp
 * - strcat
 * - strncat
 * - strchr
 * - strrchr
 * - strstr
 *
 * The following methods have not been implemented:
 * - strcoll & strxfrm
 *   Unneeded since the kernel doesn't have localization
 * - sterrrno
 *   Currently, it isn't decided on how to deal errors within the kernel.
 * - strpbrk, strcspn, strspn
 *   Not very common functions to use
 * - strtok,
 *   Awkward and cumbersome API, instead use the kernel extension
 *   ktokenize
 *
 * Kernel extensions:
 * - ktokenize
 *   A simpler tokenizer which gives you a singly linked list with
 *   all substrings in it. The first element of the list is the
 *   first element it found.
 **/
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <vp_singly_linked_list.h>

/**
 * Copy all characters from the source to the destination, please
 * consider using strncpy instead.
 */
char * strcpy(char * restrict dst, const char * restrict src) {
    do {
        *dst++ = *src;
    } while (*src++ != '\0');

    return dst;
}

/**
 * Copy the first n character from the source to the destination
 */
char * strncpy(char * restrict dest, const char * restrict src, size_t n) {
    char * s = (char *)src;
    char * d = (char *)dest;
    while ((*d++ = *s++) && n--)
        ;
    return dest;
}

/**
 * Add all the characters from the source to the destination
 * Please consider using strncat instead.
 */
char * strcat(char * restrict dst, const char * restrict src) {
    while (*dst++ != '\0')
        ;

    dst--;

    do {
        *dst++ = *src;
    } while (*src++ != '\0');

    *dst = '\0';

    return dst;
}

/**
 * Add the first n characters from the source string to the
 * the destination string.
 */
char * strncat(char * restrict dst, const char * restrict src, size_t n) {
    while(*dst++ != '\0') {
        ;
    }

    // Overwrite the NULL
    dst--;

    while (n-- != 0) {
        *dst++ = *src;

        // C99 required us to write all n characters
        if (*src != '\0')
            src++;
    }


    return dst;
}

/**
 * Find the first instance of the character c in the
 * the string.
 */

char *strchr(const char *s, int32_t c) {
    char *p = (char *) s;
    while (*p != '\0') {
        if (*p == c) {
            return p;
        }

        p++;
    }

    return NULL;
}

/**
 * Find the first instance of the character c starting
 * from the end of the given string.
 */
char *strrchr(const char *s, int32_t c) {
    char *res = NULL;

    while (*s != '\0') {
        if (*(const char *) s == c)
            res = (char *)s;
        s++;
    }

    return res;
}

/**
 * Find the first instance of needle in haystack.
 */
char * strstr(const char * haystack, const char * needle) {
    if (*needle == '\0')
        return (char*) haystack;

    // Naive O(N^2 implementation)
    for (char *p = (char*) haystack; *p != '\0'; p++) {
        char *q = (char *) needle, *u = p;
        while (*q == *u && *q != '\0') {
            q++; u++;
        }

        if (*q == '\0')
            return p;
    }

    return NULL;
}

/**
 * Compare two strings together, preferably use strncmp where
 * possible.
 */
uint32_t strcmp(char * s1, char * s2) {
    for (uint32_t i = 0;; i++) {
        if (s1[i] != s2[i]) { return s1[i] < s2[i] ? -1 : 1; }

        if (s1[i] == '\0') { return 0; }
    }
}

/**
 * Compare the first n characters of two strings
 */
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

/**
 * Calculate the length of a string
 */
uint32_t strlen(const char * str) {
    char *p = (char*) str;

    do {
        ;
    } while (*p++ != '\0');

    // Avoid having an branch subtracting our
    // current place minus the start of the
    // string
    return p - str - 1;
}

/******************************/
/* course_os specific methods */
/******************************/
/**
 * A simple tokenizer which takes an array of variable length
 * tokens and a src string. It returns a singly linked list of
 * all the subdivided strings based on the left-to-right token order.
 *
 * Please keep in mind that this method allocates memory, so all memory
 * that it creates ought to be deallocated manually.
 */
VPSinglyLinkedList *ktokenize(const string src, const string* tokens, const size_t n) {
    VPSinglyLinkedList *lst = vpsll_create();
    char *cur = (char *) src;

    while (true) {
        // Find the next occurence of the token in the string
        uint8_t min = -1;
        string p = NULL;
        size_t count = n;
        int num = 0;

        while (count-- != 0) {
            string found = strstr(cur, *(tokens + count));
            int temp = found - cur;
            if (temp < min) {
                p = found ;
                min = temp;
                num = count;
            }
        }

        if (p == NULL) {
            int length = strlen(cur);
            char *dst = kmalloc(sizeof(char) * length);
            strncpy(dst, cur, length);
            vpsll_push_to_back(lst, dst);
            break;
        }

        // If it is found, it is farther than were we began so we can
        // use that to create memory to store it in and to copy the
        // data.
        size_t length = p - cur - 1;
        char *dst = kmalloc(sizeof(char) * length);
        strncpy(dst, cur, length);
        vpsll_push_to_back(lst, dst);
        cur = p+strlen(*(tokens + num));
    }

    return lst;
}
