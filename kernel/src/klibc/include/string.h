
#ifndef STRING_H
#define STRING_H

#include <stdbool.h>
#include <stdint.h>

char * strcpy(char * dst, char * src);
char * strncpy(char * dest, char * src, size_t n);
uint32_t strcmp(char * s1, char * s2);
uint32_t strlen(char * str);
uint32_t strncmp(char * s1, char * s2, size_t n);

void * memcpy(void * dest, void * src, size_t count);
void * memset(void * dest, uint32_t val, size_t count);
uint16_t * memsetw(uint16_t * dest, uint16_t val, size_t count);
void * memmove(void * dest, const void * src, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);

#endif
