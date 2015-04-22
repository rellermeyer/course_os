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
 */
#include <stdint.h>
#include <stdarg.h>
#ifndef __klibc_h
#define __klibc_h
typedef unsigned int os_size_t;

// useful macros
#define MAX(a, b) ((a) > (b) ? a : b)
#define MIN(a, b) ((a) < (b) ? a : b)

// basic constants
#define M_E 2.71828182845904523536
#define M_LOG2E 1.44269504088896340736
#define M_LOG10E 0.434294481903251827651
#define M_LN2 0.693147180559945309417
#define M_LN10 2.30258509299404568402
#define M_PI 3.14159265358979323846
#define M_PI_2 1.57079632679489661923
#define M_PI_4 0.785398163397448309616
#define M_1_PI 0.318309886183790671538
#define M_2_PI 0.636619772367581343076
#define M_1_SQRTPI 0.564189583547756286948
#define M_2_SQRTPI 1.12837916709551257390
#define M_SQRT2 1.41421356237309504880
#define M_SQRT_2 0.707106781186547524401

/* string.h type functionality for comparing strings or mem blocks */
int os_memcmp(const void *left, const void *right, os_size_t num);
int os_strcmp(const char *left, const char *right);

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
int os_printf(const char *str_buf, ...);

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
void* kmalloc_aligned(uint32_t, uint32_t);
void kfree(void*);
STATUS kmcheck();

int32_t abs(int32_t);
unsigned int rand();

#endif

