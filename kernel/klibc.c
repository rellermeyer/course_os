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

/*4-17-15: - Prakash 
 * panic() added
 - Currrently states the panic and stalls the machine
 */
void panic()
{
	os_printf("Kernel panic!\n");
	asm("wfi");
}

/*4-17-15: - Prakash
 _assert_fail() added
 - This is a helper function for the assert() macro
 */
int _assert_fail(char *_file, unsigned int _line, char *_func)
{
	os_printf("ASSERT FAILURE: %s:%u: %s\n", _file, _line, _func);
	panic();
	return 1;
}

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
	int limit = size / sizeof(uint32_t);

	int i = 0;
	for (; i < limit; i += 1)
	{
		*(dest + i) = *(source + i);
	}

	char * d = (char*) (dest + i);
	char * s = (char*) (source + i);

	i *= sizeof(uint32_t);

	for (; i < size; i++)
	{
		*(d++) = *(s++);
	}
	source -= size;
	dest -= size;
}

// base is between 2 and 16, inclusive
int print_int(char *buf, int buflen, int val, int base, int is_unsigned,
		int padding, char pad_char, int is_uppercase)
{
	int max_len = buflen;
	int orig_max_len = max_len;
	int negate = 0;
	if (val < 0 && !is_unsigned)
	{
		val = -val;
		negate = 1;
	}
	unsigned int temp = val;

	if (max_len == 0)
		return orig_max_len - max_len;
	if (negate)
	{
		*buf = '-';
		buf++;
		max_len--;
		if (max_len == 0)
			return orig_max_len - max_len;
	}

	char tmp_buf[64];
	int ndigits = 0;
	while (temp != 0)
	{
		if (is_uppercase)
		{
			tmp_buf[ndigits] = upper_case_digits[temp % base];
		}
		else
		{
			tmp_buf[ndigits] = lower_case_digits[temp % base];
		}
		temp = temp / base;
		ndigits++;
	}

	// Zero-pad the output
	int i;
	if (padding > 0)
	{
		for (i = 0; i < padding - ndigits - negate; i++)
		{
			*buf = pad_char;
			buf++;
			max_len--;
			if (max_len == 0)
				return orig_max_len;
		}
	}

	// Output the digits
	for (i = ndigits - 1; i >= 0; i--)
	{
		*buf = tmp_buf[i];
		buf++;
		max_len--;
		if (max_len == 0)
			return orig_max_len;
	}
	if (ndigits == 0 && padding <= 0)
	{
		*buf = '0';
		buf++;
		max_len--;
		if (max_len == 0)
			return orig_max_len;
	}

	return orig_max_len - max_len;
}

// args must already have been started
int os_vsnprintf(char *buf, int buflen, const char *str_buf, va_list args)
{
	if (buflen == 0)
		return 0;
	buflen--;
	if (buflen == 0)
	{
		buf[0] = 0;
		return 1;
	}
	int nwritten = 0;
	int t_arg;
	char* str_arg;
	int padding = -1;
	char pad_char = 0;
	while (*str_buf != '\0')
	{
		int n;
		if (*str_buf == '%')
		{
			str_buf++;
			// This label is where we go after we've read an option.
			reread_switch: ;
			switch (*str_buf)
			{
			case '0':
				// Zero-padding... Read all the numbers.
				// Then restart the switch statement.
				padding = 0;
				pad_char = '0';
				while (*str_buf <= '9' && *str_buf >= '0')
				{
					padding *= 10;
					padding += *str_buf - '0';
					str_buf++;
				}
				goto reread_switch;
				break;
			case 'X':
				t_arg = va_arg(args, int);
				n = print_int(buf, buflen, t_arg, 16, 1, padding, pad_char, 1);
				break;
			case 'x':
				t_arg = va_arg(args, int);
				n = print_int(buf, buflen, t_arg, 16, 1, padding, pad_char, 0);
				break;
			case 'd':
				t_arg = va_arg(args, int);
				n = print_int(buf, buflen, t_arg, 10, 0, padding, pad_char, 0);
				break;
			case 'u':
				t_arg = va_arg(args, int);
				n = print_int(buf, buflen, t_arg, 10, 1, padding, pad_char, 0);
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
				if (n > buflen)
				{
					n = buflen;
				}
				break;
			case '%':
				*buf = '%';
				n = 1;
				break;
			}
			// Reset all the options
			padding = -1;
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
void *os_memset(void *dest, char c, os_size_t n)
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
char *__strchrnul(const char *s, char c)
{
	os_size_t *w, k;

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
	void* block = (void*) allocate(size, 0 /* unused */, 0 /* unused */);
	return block;
}

uint32_t kmcheck()
{
	return mem_check();
}

// NOTE potentially expand these features. offer more
// memory stats
uint32_t km_size()
{
	return mem_get_heap_size();
}

void* kmalloc_aligned(uint32_t size, uint32_t alignment)
{
	void* block;
	void* ptr;

	switch (alignment)
	{
	case 4:
		block = kmalloc(size + 4);
		ptr = (void*) (((uint32_t) block + 4) & ~0x3);
		return ptr;
	case 1024:
		block = kmalloc(size + 1024);
		ptr = (void*) (((uint32_t) block + 1024) & ~0x1ff);
		return ptr;
	case 4096:
		block = kmalloc(size + 4096);
		ptr = (void*) (((uint32_t) block + 4096) & ~0x7ff);
		return ptr;
	case 16 * 1024:
		block = kmalloc(size + 16 * 1024);
		ptr = (void*) (((uint32_t) block + 16 * 1024) & ~0x1fff);
		return ptr;
	default:
		return kmalloc(size);
	}
}

void kfree(void* ptr)
{
	deallocate((uint32_t*) ptr, 0 /* unused */, 0 /* unused */);
}

unsigned int rand()
{
	static unsigned int z1 = 12345, z2 = 67891, z3 = 11121, z4 = 31415;
	unsigned int b;
	b = ((z1 << 6) ^ z1) >> 13;
	z1 = ((z1 & 4294967294U) << 18) ^ b;
	b = ((z2 << 2) ^ z2) >> 27;
	z2 = ((z2 & 4294967288U) << 2) ^ b;
	b = ((z3 << 13) ^ z3) >> 21;
	z3 = ((z3 & 4294967280U) << 7) ^ b;
	b = ((z4 << 3) ^ z4) >> 12;
	z4 = ((z4 & 4294967168U) << 13) ^ b;
	return (z1 ^ z2 ^ z3 ^ z4);
}

/**
 * umalloc allocates memory on the user heap
 *
 * @param  size of the block of memory allocated
 * @param  uint32_t size
 * @return returns a pointer to the allocated block of memory
 */
void* umalloc(uint32_t size)
{
	void* block = (void*) proc_allocate(size);
	return block;
}

/**
 * ualigned alloc allocates memory on the user heap
 * according to a specified alignemnt
 *
 * @param  size of the block of memory allocated, and alignment desired
 * @param  uint32_t size, uint32_alignment
 * @return returns a pointer to the allocated block of memory
 * 		   that is a multiple of the specified allignement 
 */

void* ualigned_alloc(uint32_t size, uint32_t alignment)
{
	void* block;
	void* ptr;

	switch (alignment)
	{
	case 4:
		block = umalloc(size + 4);
		ptr = (void*) (((uint32_t) block + 4) & ~0x3);
		return ptr;
	case 1024:
		block = umalloc(size + 1024);
		ptr = (void*) (((uint32_t) block + 1024) & ~0x1ff);
		return ptr;
	case 4096:
		block = umalloc(size + 4096);
		ptr = (void*) (((uint32_t) block + 4096) & ~0x7ff);
		return ptr;
	case 16 * 1024:
		block = umalloc(size + 16 * 1024);
		ptr = (void*) (((uint32_t) block + 16 * 1024) & ~0x1fff);
		return ptr;
	default:
		return umalloc(size);
	}
}

/**
 * free's an allocated block of memory on the heap
 *
 * @param  pointer to a block of memeory on the heap
 * @param  void* ptr
 * @return nothing returned
 */
void ufree(void* ptr)
{
	proc_deallocate((uint32_t*) ptr);
}
