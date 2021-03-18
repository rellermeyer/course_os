#ifndef STRING_H
#define STRING_H

#include <stdbool.h>
#include <stdint.h>
#include <vp_singly_linked_list.h>
typedef char* string;

char * strcpy(char * restrict dst, const char * restrict src);
char * strncpy(char * dst, const char * restrict src, size_t n);
char * strcat(char * restrict dst, const char * restrict src);
char * strncat(char * restrict dst, const char * restrict src, size_t n);
char * strchr(const char * s, int32_t c);
char * strrchr(const char * s, int32_t c);
char * strstr(const char * haystack, const char * needle);
uint32_t strcmp(char * s1, char * s2);
uint32_t strlen(const char * str);
uint32_t strncmp(char * s1, char * s2, size_t n);

void *memcpy(void * dest, void * src, size_t count);
void *memset(void * dest, uint32_t val, size_t count);
uint16_t *memsetw(uint16_t * dest, uint16_t val, size_t count);
void *memmove(void * dest, const void * src, size_t n);
int32_t memcmp(const void * s1, const void * s2, size_t n);
void *memchr(const void *s, int32_t c, size_t n);

VPSinglyLinkedList *ktokenize(const string src, const string* tokens, size_t n);

#endif
