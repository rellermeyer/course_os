/********************************************************************
*	klibc.c
*
*	(Any collaborators, please add your name)
*	Author: Jared McArthur, Taylor Smith, Sheldon Sandbekkhaug, Kaelen Haag
*
*	Last edited: 21 April 2014
*
*	Purpose:	Provide basic libc funtionality for CourseOS
*			This file provides function implementations
*			for skels in libc.h
*
*	Usage:	Compile into kernel. Adaptations of normal libc functions
*		can be used by prepending os_ suffix.
*
*	Notes:	The following were adapted directly from musl-libc:
*               memcmp, memset, strcmp, strchrnul, strcpy, strlen, strtok,
*               strspn, and strcspn
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
#define ALIGN (sizeof(os_size_t))
#define ONES ((os_size_t)-1/UCHAR_MAX)
#define HIGHS (ONES * (UCHAR_MAX/2+1))
#define HASZERO(x) ((x)-ONES & ~(x) & HIGHS)


static char lower_case_digits[16] = "0123456789abcdef";
static char upper_case_digits[16] = "0123456789ABCDEF";

uint32_t* heap;
uint32_t heap_size;
/* string.h type functionality for comparing strings or mem blocks */
int os_memcmp ( const void *left, const void *right, os_size_t num )
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

//memory copy
//Responsibility is on the programmer to copy safely 
void os_memcpy(uint32_t * source, uint32_t * dest, os_size_t size)
{
	int i = 0;
	for(; i < size; i++)
	{
		*(dest + i) = *(source + i);
	}
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
void *os_memset(void *dest, int c, os_size_t n)
{
	unsigned char *s = dest;
	os_size_t k;

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


/* Returns a pointer to the first instance of c in s, like indexOf().
   If c is not found, then return a pointer to the NULL character at
   the end of String s.
*/
char *__strchrnul(const char *s, int c)
{
	os_size_t *w, k;

	c = (unsigned char)c;
	if (!c) return (char *)s + os_strlen(s);

	for (; (uintptr_t)s % ALIGN; s++)
		if (!*s || *(unsigned char *)s == c) return (char *)s;
	k = ONES * c;
	for (w = (void *)s; !HASZERO(*w) && !HASZERO(*w^k); w++);
	for (s = (void *)w; *s && *(unsigned char *)s != c; s++);
	return (char *)s;
}


/* Copies the String src to dest */
char *os_strcpy(char *dest, const char *src)
{
        const unsigned char *s = src;
        unsigned char *d = dest;
        while ((*d++ = *s++));
        return dest;
}


/* Return the length of s */
os_size_t os_strlen(const char *s)
{
	const char *a = s;
	const os_size_t *w;
	for (; (uintptr_t)s % ALIGN; s++) if (!*s) return s-a;
	for (w = (const void *)s; !HASZERO(*w); w++);
	for (s = (const void *)w; *s; s++);
	return s-a;
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

int32_t abs(int32_t val){
  if(val<0)
    return val*(-1);
  else
    return val;
}

void* init_heap(uint32_t size){
  heap = (char*)mem_alloc(size);
  //os_printf("&heap=%x\n", heap);
  heap_size = size;

  uint32_t* heap_header = heap;
  uint32_t* heap_footer = heap+heap_size-sizeof(int);

  *heap_header = heap_size;
  *heap_footer = heap_size;

  //os_printf("heap_header=%x\n", heap_header);
  //os_printf("heap_footer=%x\n", heap_footer);
  return heap;
}

char* allocate(uint32_t size, char* heap, int32_t heap_size){
  if(size > (heap_size + 2*sizeof(int32_t)))
    return 0;
  int32_t i, ret_ptr;
  for(i = 0; i < heap_size; i+=0){

    //os_printf("byte=%d\n",i);

    uint32_t* header_addr = heap+i;
    int32_t header = *header_addr;

    //os_printf("&header=%x\n",header_addr);

    uint32_t* footer_addr = heap+i+sizeof(int32_t)+size;

    //free and >= request
    if(header>0 && header>=size){

      //cannot split this block
      if( (header-size) < (2*sizeof(int32_t)+sizeof(char)) ){

        ret_ptr = i + sizeof(int32_t);
        //mark header as used
        *header_addr = header*(-1);
        //insert a footer at end of block
        *footer_addr = header*(-1);
        return heap+ret_ptr;

      }

      //can split this block
      else{
        ret_ptr = i + sizeof(int32_t);

        int32_t old_space = header;
        int32_t occ_space = size+2*sizeof(int32_t);
        //mark header as used
        *header_addr = size*(-1);
        //insert footer
        *footer_addr = size*(-1);

        //insert new free block header
        uint32_t* new_header = heap+i+2*sizeof(int32_t)+size;
        *new_header = old_space - occ_space;
        //insert new free block footer
        uint32_t* new_footer = heap+i+sizeof(int32_t)+old_space;
        *new_footer = old_space - occ_space;

        return heap+ret_ptr;
      }
    }
    //jump to the next block
    else{
      i = i + abs(header) + 2*sizeof(int32_t);
    }
  }

  return 0;

}

void deallocate(void* ptr, char* heap, int32_t heap_size){
  uint32_t first_block = 0;
  uint32_t last_block = 0;

  uint32_t* header_addr = (char*)ptr-sizeof(int);
  uint32_t size = abs(*header_addr);

  uint32_t* footer_addr = (char*)ptr+size;

  if(header_addr == heap){
    first_block = 1;
  }

  if(footer_addr+sizeof(int) == (uint32_t*)heap+heap_size){
    last_block = 1;
  }

  //only check and coalesce right block
  if(first_block){
    uint32_t* right_header_addr = (char*)footer_addr+sizeof(int32_t);
    int32_t right_block_size = *right_header_addr;

    //free right block
    if(right_block_size>0){
      //set new header at freed blocks header
      *header_addr = size+right_block_size+2*sizeof(int32_t);
      //set new footer at right blocks footer
      uint32_t* right_footer_addr = (char*)footer_addr+2*sizeof(int32_t)+right_block_size;
      *right_footer_addr = size+right_block_size+2*sizeof(int32_t);
    }
    else{
      //make freed blocks header and footer positive
      *header_addr = size;
      *footer_addr = size;
    }
  }

  //only check and coalesce left block
  if(last_block){
    uint32_t* left_block_header = (char*)header_addr-sizeof(int32_t);
    int32_t left_block_size = *left_block_header;

    //free left block
    if(left_block_size>0){
      //set new header at left blocks header
      uint32_t* left_header_addr = (char*)header_addr-2*sizeof(int32_t)-left_block_size;
      *left_header_addr = size+left_block_size+2*sizeof(int32_t);
      //set new footer at freed blocks footer
      *footer_addr = size+left_block_size+2*sizeof(int32_t);
    }
    else{
      *header_addr = size;
      *footer_addr = size;
    }
  }

  //check and coalesce both adjacent blocks
  if(!first_block && !last_block){
    uint32_t* right_block_header = (char*)footer_addr+sizeof(int32_t);
    int32_t right_block_size = *right_block_header;

    uint32_t* left_block_header = (char*)header_addr-sizeof(int32_t);
    int32_t left_block_size = *left_block_header;

    //both adjacent blocks are free
    if(right_block_size>0 && left_block_size>0){
      int32_t new_size = size + right_block_size + left_block_size + 4*sizeof(int32_t);

      //set new header at left blocks header
      uint32_t* left_header_addr = (char*)header_addr-2*sizeof(int32_t)-left_block_size;
      *left_header_addr = new_size;
      //set new footer at right blocks footer 
      uint32_t* right_footer_addr = (char*)footer_addr+2*sizeof(int32_t)+right_block_size;
      *right_footer_addr = new_size;          
    }

    //only right free block
    else if(right_block_size>0 && left_block_size<0){
      //set new header at freed blocks header
      *header_addr = size+right_block_size+2*sizeof(int32_t);
      //set new footer at right blocks footer
      uint32_t* right_footer_addr = (char*)footer_addr+2*sizeof(int32_t)+right_block_size;
      *right_footer_addr = size+right_block_size+2*sizeof(int32_t);     
    }
    //only left free block
    else if(left_block_size>0 && right_block_size<0){
      //set new header at left blocks header
      uint32_t* left_header_addr = (char*)header_addr-2*sizeof(int32_t)-left_block_size;
      *left_header_addr = size+left_block_size+2*sizeof(int32_t);
      //set new footer at freed blocks footer
      *footer_addr = size+left_block_size+2*sizeof(int32_t);      
    }
    else{
      *header_addr = size;
      *footer_addr = size;
    }   

  }


}


void* kmalloc(uint32_t size){
  void* block = (void*)allocate(size, heap, heap_size);
  return block;
}

void kfree(void* ptr){
  deallocate((uint32_t*)ptr, heap, heap_size);
}


void mcheck(){

  char* ptr = (char*)heap;
  uint32_t* end_ptr = (char*)heap + 2*sizeof(int32_t) + heap_size;
  int i, block=0;
  for(i = 0; i < heap_size; i+=0){
    uint32_t* block_addr = ptr+sizeof(int32_t);

    uint32_t* header_addr = ptr;
    int32_t block_header = *header_addr;
    int32_t block_size = abs(block_header);
    
    uint32_t* footer_addr = ptr+sizeof(int32_t)+block_size;
    int32_t block_footer = *footer_addr;

    if(block_header == block_footer && block_header<0){
      os_printf("Block %d Allocated:", block);
      os_printf("\tsize = %d, address = %x\n", block_size, block_addr);
    }
    else if(block_header == block_footer && block_header>0){
      os_printf("Block %d Free:", block);
      os_printf("\tsize = %d, address = %x\n", block_size, block_addr);
    }
    else{
      os_printf("INCONSISTENT HEAP");
      return;
    }

    ptr = ptr + block_size + 2*sizeof(int32_t);
    block++;
    if(ptr == end_ptr)
      return;
  }
}