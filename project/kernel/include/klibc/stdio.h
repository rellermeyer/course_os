
#ifndef STDIO_H
#define STDIO_H

#define BUFSIZE 1024
#define _IOBF 0
#define _IOLBF 1
#define _IOFBF 2

#ifndef NULL
    #define NULL ((void *)0)
#endif

#define UNUSED(x) (void)(x)

#include <klibc.h>
#include <stdarg.h>

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
int os_vsnprintf(char * buf, int buflen, const char * str_buf, va_list args);
int os_snprintf(char * buf, int buflen, const char * fmt_string, ...);
int kprintf(const char * str_buf, ...);

int getchar(void);
char *gets(char *s);
int putchar(int c);
int puts(const char *s);

ssize_t os_getline(char **lineptr, size_t *n);
ssize_t os_getdelim(char **lineptr, int delim);

#endif
