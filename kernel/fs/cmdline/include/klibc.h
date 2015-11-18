#ifndef KLIBC_H
#define KLIBC_H 1

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef unsigned int os_size_t;

#define os_printf(...) printf(__VA_ARGS__)

void *os_memset(void *dest, char c, os_size_t n);
char *__strchrnul(const char *s, char c);
char *os_strcpy(char *dest, const char *src);
char *os_strncpy(char *dest, const char *src, os_size_t n);
os_size_t os_strlen(const char *s);
char *os_strtok(char *s, const char *sep);
os_size_t os_strspn(const char *s, const char *accept);
os_size_t os_strcspn(const char *s, const char *reject);
void *os_memcpy(void *src, void *dst, int len);
int os_strcmp(const char *left, const char *right);

void* kmalloc(uint32_t size);
void kfree(void* ptr);

#endif
