/********************************************************************
*	libc.h
*
*       (Any collaborators, please add your name)
*	Author: Jared McArthur, Taylor Smith, Sheldon Sandbekkhaug, Kaelen Haag
*
*	Last edited: 20 April 2014
*
*	Purpose:	Provide basic libc funtionality for CourseOS
*				This header provides function skeletons
*				for libc.c
*
*	Usage:	Compile into kernel. Adaptations of normal libc functions
*			can be used by prepending os_ suffix.
********************************************************************/

/* LOG:
 * 3/30 added os_printf function - Taylor Smith
 * 4/1 working more on os_printf - Taylor Smith
 * 4/20 Added os_memset, os_strchrnul, os_strcpy, os_strlen, os_strtok,
 *      os_strspn, and os_strcspn from MUSL - Sheldon
 * 4/21 Added os_memcpy for loader - Kaelen
 * --------------Spring 2015---------------
 * 4/15/15: Added implementation of assert() 
 */
#include <stdint.h>
#include <stdarg.h>
#ifndef __klibc_h
#define __klibc_h
typedef unsigned int os_size_t;


/* string.h type functionality for comparing strings or mem blocks */
int os_memcmp ( const void *left, const void *right, os_size_t num );
int os_strcmp ( const char *left, const char *right);


//4-17-15: Working assert implementation - Prakash
#define assert(X){\
    if ( (X) || _assert_fail(__FILE__, __LINE__, #X));\
}

/**
 * Note: os_printf is restricted to printing only 256 characters.
 * Supported format string conversions:
 * X: upper-case hexadecimal print.
 * x: lower-case hexadecimal print.
 * d: signed integer.
 * u: unsigned integer.
 * c: ASCII character.
 * s: string.
 * %: the percent sign itself.
 *
 * Supported options:
 * 0: zero-pad the result (applies to X,x,d,u). For example:
 *    os_printf("'%05d %05d %05u'\n", 15, -15, -15);
 *    prints '00015 -0015 4294967281'
 */
int os_vsnprintf(char *buf, int buflen, const char *str_buf, va_list args);
int os_snprintf(char *buf, int buflen, const char *fmt_string, ...);
int os_printf (const char *str_buf, ...);

void *os_memset(void *dest, char c, os_size_t n);
char *__strchrnul(const char *s, char c);
char *os_strcpy(char *dest, const char *src);
char *os_strncpy(char *dest, const char *src, os_size_t n);
os_size_t os_strlen(const char *s);
char *os_strtok(char *s, const char *sep);
os_size_t os_strspn(const char *s, const char *accept);
os_size_t os_strcspn(const char *s, const char *reject);

void os_memcpy(uint32_t * source, uint32_t * dest, os_size_t size);
/* TODO: create print function for kernel debugging purposes */

void* kmalloc(uint32_t);
void kfree(void*);

int32_t abs(int32_t);

//4-17-15: Initial panic * assert_fail functions added
void panic();
int _assert_fail(char *_file, unsigned int _line, char *_func);
    //__attribute__ ((__noreturn__));

#endif