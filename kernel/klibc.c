/********************************************************************
 * klibc.c
 *
 * (Any collaborators, please add your name)
 * Author: Jared McArthur, Taylor Smith, Sheldon Sandbekkhaug, Kaelen Haag
 *
 * Last edited: 21 April 2014
 *
 * Purpose:  Provide basic libc funtionality for CourseOS
 *     This file provides function implementations
 *     for skels in libc.h
 *
 * Usage:  Compile into kernel. Adaptations of normal libc functions
 *   can be used by prepending os_ suffix.
 *
 * Notes:  The following were adapted directly from musl-libc:
 *               memcmp, memset, strcmp, strchrnul, strcpy, strlen, strtok,
 ********************************************************************/
#include <stdint.h>
#include <stdarg.h>

#include "klibc.h"
#include "global_defs.h"
#include "mem_alloc.h"
//FIXME: decouple
#include "drivers/uart.h"
extern void print_char_uart0(char);

#define LOWER_CASE 0
#define UPPER_CASE 1
#define NO_CASE 2

// Helpers for MUSL String functions
#define UCHAR_MAX 0xFF
#define ALIGN (sizeof(os_size_t))
#define ONES ((os_size_t)-1/UCHAR_MAX)
#define HIGHS (ONES * (UCHAR_MAX/2+1))
#define HASZERO(x) (((x)-ONES) & (~(x)) & HIGHS)

static char lower_case_digits[16] = "0123456789abcdef";
static char upper_case_digits[16] = "0123456789ABCDEF";

/* string.h type functionality for comparing strings or mem blocks */
int os_memcmp(const void *left, const void *right, os_size_t num)
{
	const unsigned char *l = left, *r = right;
	for (; num && *l == *r; num--, l++, r++)
		;
	return num ? *l - *r : 0;
}

int os_strcmp(const char *left, const char *right)
{
	for (; *left == *right && *left; left++, right++)
		;
	return *(unsigned char *) left - *(unsigned char *) right;
}

//memory copy
//Responsibility is on the programmer to copy safely
void os_memcpy(uint32_t * source, uint32_t * dest, os_size_t size)
{
	int i = 0;
	for (; i < size; i++)
	{
		*(dest + i) = *(source + i);
	}
}

// base is between 2 and 16, inclusive
int print_int(char *buf, int buflen,
	      int val, int base, int is_unsigned, int padding, char pad_char,
	      int is_uppercase)
{
	int max_len = buflen;
	int orig_max_len = max_len;
	int negate = 0;
	if (val < 0 && !is_unsigned) {
		val = -val;
		negate = 1;
	}
        unsigned int temp = val;

	if (max_len == 0) return orig_max_len - max_len;
	if (negate) {
		*buf = '-';
		buf++;
		max_len--;
		if (max_len == 0) return orig_max_len - max_len;
	}

	char tmp_buf[64];
	int ndigits = 0;
	while (temp != 0) {
		if (is_uppercase) {
			tmp_buf[ndigits] = upper_case_digits[temp%base];
		} else {
			tmp_buf[ndigits] = lower_case_digits[temp%base];
		}
		temp = temp / base;
		ndigits++;
	}

	// Zero-pad the output
	int i;
	if (padding > 0) {
		for (i=0; i<padding-ndigits-negate; i++) {
			*buf = pad_char;
			buf++;
			max_len--;
			if (max_len == 0) return orig_max_len;
		}
	}

	// Output the digits
	for (i=ndigits-1; i>=0; i--) {
		*buf = tmp_buf[i];
		buf++;
		max_len--;
		if (max_len == 0) return orig_max_len;
	}
	if (ndigits == 0) {
		*buf = '0';
		buf++;
		max_len--;
		if (max_len == 0) return orig_max_len;
	}

	return orig_max_len - max_len;
}

// args must already have been started
int os_vsnprintf(char *buf, int buflen, const char *str_buf, va_list args)
{
	if (buflen == 0) return 0;
	buflen--;
	if (buflen == 0) {
		buf[0] = 0;
		return 1;
	}
	int nwritten = 0;
	int t_arg;
	char* str_arg;
	while (*str_buf != '\0')
	{
		int n;
		if (*str_buf == '%')
		{
			str_buf++;
			switch (*str_buf)
			{
			case 'X':
				t_arg = va_arg(args, int);
				n = print_int(buf, buflen, t_arg, 16, 1, -1, ' ', 1);
				break;
			case 'x':
				t_arg = va_arg(args, int);
				n = print_int(buf, buflen, t_arg, 16, 1, -1, ' ', 0);
				break;
			case 'd':
			case 'u':
				t_arg = va_arg(args, int);
				n = print_int(buf, buflen, t_arg, 10, 1, -1, ' ', 0);
				break;
			case 'c':
				t_arg = va_arg(args, int);
				*buf = t_arg;
				n = 1;
				break;
			case 's':
				str_arg = va_arg(args, char*);
				os_strncpy(buf, str_arg, buflen);
				n = os_strlen(str_arg);
				if (n > buflen) {
					n = buflen;
				}
				break;
			case '%':
				*buf = '%';
				n = 1;
				break;
			}
		}
		else
		{
			*buf = *str_buf;
			n = 1;
		}
		buf += n;
		buflen -= n;
		nwritten += n;
		if (buflen <= 0)
		{
			//Return!
			break;
		}
		str_buf++;
	}
	buf[0] = 0;
	nwritten++;

	return nwritten;
}

int os_snprintf(char *buf, int buflen, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int n = os_vsnprintf(buf, buflen, fmt, args);
	va_end(args);
	return n;
}

int os_printf(const char *str_buf, ...)
{
	va_list args;
	va_start(args, str_buf);
	char buf[256];
	int n = os_vsnprintf(buf, 255, str_buf, args);
	va_end(args);
	print_uart0(buf);
	return n;
}

/* Set the first n bytes of dest to be the value c.*/
void *os_memset(void *dest, int c, os_size_t n)
{
	unsigned char *s = dest;
	os_size_t k;

	/* Fill head and tail with minimal branching. Each
	 * conditional ensures that all the subsequently used
	 * offsets are well-defined and in the dest region. */

	if (!n)
		return dest;
	s[0] = s[n - 1] = c;
	if (n <= 2)
		return dest;
	s[1] = s[n - 2] = c;
	s[2] = s[n - 3] = c;
	if (n <= 6)
		return dest;
	s[3] = s[n - 4] = c;
	if (n <= 8)
		return dest;

	/* Advance pointer to align it at a 4-byte boundary,
	 * and truncate n to a multiple of 4. The previous code
	 * already took care of any head/tail that get cut off
	 * by the alignment. */

	k = -(uintptr_t) s & 3;
	s += k;
	n -= k;
	n &= -4;

#ifdef __GNUC__
	typedef uint32_t __attribute__((__may_alias__)) u32;
	typedef uint64_t __attribute__((__may_alias__)) u64;

	u32 c32 = ((u32) -1) / 255 * (unsigned char) c;

	/* In preparation to copy 32 bytes at a time, aligned on
	 * an 8-byte bounary, fill head/tail up to 28 bytes each.
	 * As in the initial byte-based head/tail fill, each
	 * conditional below ensures that the subsequent offsets
	 * are valid (e.g. !(n<=24) implies n>=28). */

	*(u32 *) (s + 0) = c32;
	*(u32 *) (s + n - 4) = c32;
	if (n <= 8)
		return dest;
	*(u32 *) (s + 4) = c32;
	*(u32 *) (s + 8) = c32;
	*(u32 *) (s + n - 12) = c32;
	*(u32 *) (s + n - 8) = c32;
	if (n <= 24)
		return dest;
	*(u32 *) (s + 12) = c32;
	*(u32 *) (s + 16) = c32;
	*(u32 *) (s + 20) = c32;
	*(u32 *) (s + 24) = c32;
	*(u32 *) (s + n - 28) = c32;
	*(u32 *) (s + n - 24) = c32;
	*(u32 *) (s + n - 20) = c32;
	*(u32 *) (s + n - 16) = c32;

	/* Align to a multiple of 8 so we can fill 64 bits at a time,
	 * and avoid writing the same bytes twice as much as is
	 * practical without introducing additional branching. */

	k = 24 + ((uintptr_t) s & 4);
	s += k;
	n -= k;

	/* If this loop is reached, 28 tail bytes have already been
	 * filled, so any remainder when n drops below 32 can be
	 * safely ignored. */

	u64 c64 = c32 | ((u64) c32 << 32);
	for (; n >= 32; n -= 32, s += 32)
	{
		*(u64 *) (s + 0) = c64;
		*(u64 *) (s + 8) = c64;
		*(u64 *) (s + 16) = c64;
		*(u64 *) (s + 24) = c64;
	}
#else
	/* Pure C fallback with no aliasing violations. */
	for (; n; n--, s++) *s = c;
#endif

	return dest;
}

/* Returns a pointer to the first instance of c in s, like indexOf().
 If c is not found, then return a pointer to the NULL character at
 the end of String s.
 */
char *__strchrnul(const char *s, int c)
{
	os_size_t *w, k;

	c = (unsigned char) c;
	if (!c)
		return (char *) s + os_strlen(s);

	for (; (uintptr_t) s % ALIGN; s++)
		if (!*s || *(unsigned char *) s == c)
			return (char *) s;
	k = ONES * c;
	for (w = (void *) s; !HASZERO(*w) && !HASZERO(*w ^ k); w++)
		;
	for (s = (void *) w; *s && *(unsigned char *) s != c; s++)
		;
	return (char *) s;
}

/* Copies the String src to dest */
char *os_strcpy(char *dest, const char *src)
{
	const unsigned char *s = (const unsigned char*) src;
	unsigned char *d = (unsigned char*) dest;
	while ((*d++ = *s++))
		;
	return dest;
}

/* Copies the String src to dest */
char *os_strncpy(char *dest, const char *src, os_size_t n)
{
	const unsigned char *s = (const unsigned char*) src;
	unsigned char *d = (unsigned char*) dest;
	while ((*d++ = *s++) && n--)
		;
	return dest;
}

/* Return the length of s */
os_size_t os_strlen(const char *s)
{
	const char *a = s;
	const os_size_t *w;
	for (; (uintptr_t) s % ALIGN; s++)
		if (!*s)
			return s - a;
	for (w = (const void *) s; !HASZERO(*w); w++)
		;
	for (s = (const void *) w; *s; s++)
		;
	return s - a;
}

/* A re-entrant function that returns a substring of s. The substring starts
 at the beginning of the string or the previous index of one of the
 delimiter characters (indicated by sep). The substring ends at the next
 delimeter character (indicated by sep).
 */
char *os_strtok(char *s, const char *sep)
{
	static char *p;
	if (!s && !(s = p))
		return NULL;
	s += os_strspn(s, sep);
	if (!*s)
		return p = 0;
	p = s + os_strcspn(s, sep);
	if (*p)
		*p++ = 0;
	else
		p = 0;
	return s;
}

/* Returns the length of the initial segment of s that only includes
 the characters in c.
 */
os_size_t os_strspn(const char *s, const char *accept)
{
	char c = s[0]; // The character in s being checked
	int length = 0;

	// Check each character in s
	while (c != 0)
	{
		Boolean ok = FALSE;

		// Check against each character in accept
		int i;
		for (i = 0; i < os_strlen(accept); i++)
		{
			if (c == accept[i])
			{
				ok = TRUE;
			}
		}

		if (ok == TRUE)
		{
			// If c matched any character in accept, continue
			length++;
			c = s[length];
		}
		else
		{
			// If did not match any character in accept, we are done
			return length;
		}
	}
	return length;
}

/* Returns the length of the initial segment of s that does not contain
 any characters in string c.
 */
os_size_t os_strcspn(const char *s, const char *reject)
{
	char c = s[0]; // The character in s being checked
	int length = 0;

	// Check each character in s
	while (c != 0)
	{
		// Check against each character in reject
		int i;
		for (i = 0; i < os_strlen(reject); i++)
		{
			if (c == reject[i])
			{
				return length;
			}
		}

		// If c did not match any reject characters, continue
		length++;
		c = s[length];
	}
	return length;
}

int32_t abs(int32_t val)
{
	const int32_t mask = val >> sizeof(int32_t) * (NBBY - 1);
	return (val + mask) ^ mask;
}

void* kmalloc(uint32_t size)
{
	void* block = (void*) allocate(size, heap, heap_size);
	return block;
}

void kfree(void* ptr)
{
	deallocate((uint32_t*) ptr, heap, heap_size);
}
