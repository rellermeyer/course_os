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
#ifndef __klibc_h
#define __klibc_h
typedef unsigned int os_size_t;


/* string.h type functionality for comparing strings or mem blocks */
int os_memcmp ( const void *left, const void *right, os_size_t num );
int os_strcmp ( const char *left, const char *right);

int os_printf (const char *str_buf, ...);

void *os_memset(void *dest, int c, os_size_t n);
char *__strchrnul(const char *s, int c);
char *os_strcpy(char *dest, const char *src);
os_size_t os_strlen(const char *s);
char *os_strtok(char *s, const char *sep);
os_size_t os_strspn(const char *s, const char *accept);
os_size_t os_strcspn(const char *s, const char *reject);

void os_memcpy(uint32_t * source, uint32_t * dest, os_size_t size);
/* TODO: create print function for kernel debugging purposes */

void* kmalloc(uint32_t);
void kfree(void*);

#endif

