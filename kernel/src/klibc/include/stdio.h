
#ifndef STDIO_H
#define STDIO_H

#ifndef NULL
#define NULL  ((void*) 0)
#endif

#define UNUSED(x) (void)(x)

#include <stdarg.h>
#include <klibc.h>

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



#endif
