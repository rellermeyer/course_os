/********************************************************************
*	klibc.c
*
*	(Any collaborators, please add your name)
*	Author: Jared McArthur, Taylor Smith, Sheldon Sandbekkhaug
*
*	Last edited: 20 April 2014
*
*	Purpose:	Provide basic libc funtionality for CourseOS
*			This file provides function implementations
*			for skels in libc.h
*
*	Usage:	Compile into kernel. Adaptations of normal libc functions
*		can be used by prepending os_ suffix.
*
*	Notes:	The following were adapted directly from musl-libc:
*               memcmp, memset, strcmp, strchrnul, strlen, strtok, strspn,
*               strcspn
********************************************************************/

#include "include/klibc.h"
#include "include/global_defs.h"
#include <stdarg.h>
#include <stdint.h>

#define LOWER_CASE 0
#define UPPER_CASE 1
#define NO_CASE 2

// Helpers for MUSL String functions
#define UCHAR_MAX 0xFF
#define ALIGN (sizeof(size_t))
#define ONES ((size_t)-1/UCHAR_MAX)
#define HIGHS (ONES * (UCHAR_MAX/2+1))
#define HASZERO(x) ((x)-ONES & ~(x) & HIGHS)


static char lower_case_digits[16] = "0123456789abcdef";
static char upper_case_digits[16] = "0123456789ABCDEF";

/* string.h type functionality for comparing strings or mem blocks */
int os_memcmp ( const void *left, const void *right, size_t num )
{
	const unsigned char *l = left, *r = right;
	for ( ; num && *l == *r; num--, l++, r++ );
	return num ? *l - *r : 0;
}

int os_strcmp ( const char *left, const char *right)
{
	for (; *left == *right && *left; left++, right++);
	return *(unsigned char *)left - *(unsigned char *)right;
}

void print_hex(int val, int CASE)
{
  int temp = val;
  int count_digits = 0;
  char buf[100];
  int CHAR_MASK = 0xF;
  if (temp == 0){
    //printf("0");
    print_char_uart0('0');
  } 
  while((temp != 0) && (count_digits < 8))
  {
    int index = temp & CHAR_MASK;
    if(CASE == UPPER_CASE){
      buf[count_digits] = upper_case_digits[index];
    } else {
      buf[count_digits] = lower_case_digits[index];
    }
    temp = temp >> 4;
    count_digits += 1;
  }
  while(count_digits > 0){
    //printf("%c", buf[count_digits-1]);
    print_char_uart0(buf[count_digits - 1]);
    count_digits--;
  }
}

print_dec(int val){
  int temp = val;
  int count_digits = 0;
  char buf[100];
  if (temp == 0){
    //printf("0");
    print_uart0("0");
  } else if (temp < 0) {
    //printf("-");
    print_uart0("-");
    temp = -temp;
  }
  while(temp != 0){
    int index = temp % 10;
    buf[count_digits] = upper_case_digits[index];
    temp = temp / 10;
    count_digits += 1;
  }
  while(count_digits > 0){
    //printf("%c", buf[count_digits-1]);
    print_char_uart0(buf[count_digits - 1]);
    count_digits--;
  }
}

int os_printf(const char *str_buf, ...) {
  va_list args;
  int t_arg;
  va_start(args, str_buf);
  int count_args_found = 0;
  while(*str_buf != '\0') {
    if(*str_buf == '%') {
      str_buf++;
      switch (*str_buf) {
        case 'X':
          t_arg = va_arg(args,int);
          print_hex(t_arg, UPPER_CASE);
          break;
        case 'x':
          t_arg = va_arg(args,int);
          print_hex(t_arg, LOWER_CASE);
          break;
        case 'd':
          t_arg = va_arg(args,int);
          print_dec(t_arg);
          break;
        case 'c':
          t_arg = va_arg(args,int);
          print_char_uart0(t_arg);
          //printf("%c", t_arg);
          break;
        case '%':
          print_uart0("%");
          //printf("%%");
          break;
      }
    } else {
      print_char_uart0(*str_buf);
      //printf("%c", *str_buf);
    }
    str_buf++;
  }
  va_end(args);
}

/* Set the first n bytes of dest to be the value c.*/
void *os_memset(void *dest, int c, size_t n)
{
	unsigned char *s = dest;
	size_t k;

	/* Fill head and tail with minimal branching. Each
	 * conditional ensures that all the subsequently used
	 * offsets are well-defined and in the dest region. */

	if (!n) return dest;
	s[0] = s[n-1] = c;
	if (n <= 2) return dest;
	s[1] = s[n-2] = c;
	s[2] = s[n-3] = c;
	if (n <= 6) return dest;
	s[3] = s[n-4] = c;
	if (n <= 8) return dest;

	/* Advance pointer to align it at a 4-byte boundary,
	 * and truncate n to a multiple of 4. The previous code
	 * already took care of any head/tail that get cut off
	 * by the alignment. */

	k = -(uintptr_t)s & 3;
	s += k;
	n -= k;
	n &= -4;

#ifdef __GNUC__
	typedef uint32_t __attribute__((__may_alias__)) u32;
	typedef uint64_t __attribute__((__may_alias__)) u64;

	u32 c32 = ((u32)-1)/255 * (unsigned char)c;

	/* In preparation to copy 32 bytes at a time, aligned on
	 * an 8-byte bounary, fill head/tail up to 28 bytes each.
	 * As in the initial byte-based head/tail fill, each
	 * conditional below ensures that the subsequent offsets
	 * are valid (e.g. !(n<=24) implies n>=28). */

	*(u32 *)(s+0) = c32;
	*(u32 *)(s+n-4) = c32;
	if (n <= 8) return dest;
	*(u32 *)(s+4) = c32;
	*(u32 *)(s+8) = c32;
	*(u32 *)(s+n-12) = c32;
	*(u32 *)(s+n-8) = c32;
	if (n <= 24) return dest;
	*(u32 *)(s+12) = c32;
	*(u32 *)(s+16) = c32;
	*(u32 *)(s+20) = c32;
	*(u32 *)(s+24) = c32;
	*(u32 *)(s+n-28) = c32;
	*(u32 *)(s+n-24) = c32;
	*(u32 *)(s+n-20) = c32;
	*(u32 *)(s+n-16) = c32;

	/* Align to a multiple of 8 so we can fill 64 bits at a time,
	 * and avoid writing the same bytes twice as much as is
	 * practical without introducing additional branching. */

	k = 24 + ((uintptr_t)s & 4);
	s += k;
	n -= k;

	/* If this loop is reached, 28 tail bytes have already been
	 * filled, so any remainder when n drops below 32 can be
	 * safely ignored. */

	u64 c64 = c32 | ((u64)c32 << 32);
	for (; n >= 32; n-=32, s+=32) {
		*(u64 *)(s+0) = c64;
		*(u64 *)(s+8) = c64;
		*(u64 *)(s+16) = c64;
		*(u64 *)(s+24) = c64;
	}
#else
	/* Pure C fallback with no aliasing violations. */
	for (; n; n--, s++) *s = c;
#endif

	return dest;
}


/* Return the length of s */
size_t os_strlen(const char *s)
{
	const char *a = s;
	const size_t *w;
	for (; (uintptr_t)s % ALIGN; s++) if (!*s) return s-a;
	for (w = (const void *)s; !HASZERO(*w); w++);
	for (s = (const void *)w; *s; s++);
	return s-a;
}


/* Returns a pointer to the first instance of c in s, like indexOf().
   If c is not found, then return a pointer to the NULL character at
   the end of String s.
*/
char *__strchrnul(const char *s, int c)
{
	size_t *w, k;

	c = (unsigned char)c;
	if (!c) return (char *)s + os_strlen(s);

	for (; (uintptr_t)s % ALIGN; s++)
		if (!*s || *(unsigned char *)s == c) return (char *)s;
	k = ONES * c;
	for (w = (void *)s; !HASZERO(*w) && !HASZERO(*w^k); w++);
	for (s = (void *)w; *s && *(unsigned char *)s != c; s++);
	return (char *)s;
}


/* A re-entrant function that returns a substring of s. The substring starts
   at the beginning of the string or the previous index of one of the
   delimiter characters (indicated by sep). The substring ends at the next
   delimeter character (indicated by sep).
*/
char *os_strtok(char *s, const char *sep)
{
	static char *p;
	if (!s && !(s = p)) return NULL;
	s += os_strspn(s, sep);
	if (!*s) return p = 0;
	p = s + os_strcspn(s, sep);
	if (*p) *p++ = 0;
	else p = 0;
	return s;
}

// Helper for os_strspn and os_strcpsn
#define BITOP(a,b,op) \
 ((a)[(size_t)(b)/(8*sizeof *(a))] op (size_t)1<<((size_t)(b)%(8*sizeof *(a))))


/* Returns the length of the initial segment of s that only includes
   the characters in c.
*/
size_t os_strspn(const char *s, const char *c)
{
	const char *a = s;
	size_t byteset[32/sizeof(size_t)] = { 0 };

	if (!c[0]) return 0;
	if (!c[1]) {
		for (; *s == *c; s++);
		return s-a;
	}

	for (; *c && BITOP(byteset, *(unsigned char *)c, |=); c++);
	for (; *s && BITOP(byteset, *(unsigned char *)s, &); s++);
	return s-a;
}


/* Returns the length of the initial segment of s that does not contain
   any characters in string c.
*/
size_t os_strcspn(const char *s, const char *c)
{
	const char *a = s;
	size_t byteset[32/sizeof(size_t)];

	if (!c[0] || !c[1]) return __strchrnul(s, *c)-a;

	os_memset(byteset, 0, sizeof byteset);
	for (; *c && BITOP(byteset, *(unsigned char *)c, |=); c++);
	for (; *s && !BITOP(byteset, *(unsigned char *)s, &); s++);
	return s-a;
}

int main()
{ return 0; }

