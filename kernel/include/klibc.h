/********************************************************************
*	libc.h
*
*       (Any collaborators, please add your name)
*	Author: Jared McArthur, Taylor Smith, Sheldon Sandbekkhaug
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
 */

#ifndef __klibc_h
#define __klibc_h
typedef unsigned int size_t;

/* string.h type functionality for comparing strings or mem blocks */
int os_memcmp ( const void *left, const void *right, size_t num );
int os_strcmp ( const char *left, const char *right);

int os_printf (const char *str_buf, ...);

void *os_memset(void *dest, int c, size_t n);
char *__strchrnul(const char *s, int c);
char *strcpy(char *dest, const char *src);
size_t os_strlen(const char *s);
char *os_strtok(char *s, const char *sep);
size_t os_strspn(const char *s, const char *c);
size_t os_strcspn(const char *s, const char *c);

/* TODO: create print function for kernel debugging purposes */

#endif
