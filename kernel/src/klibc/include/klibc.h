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
#ifndef __KLIBC_H__
#define __KLIBC_H__

#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>

#ifndef __NO_WFI
#define SLEEP 	while (1) asm volatile("wfi")
#else
#define SLEEP for(;;)
#endif

typedef unsigned int os_size_t;




//4-17-15: Working assert implementation - Prakash
#define assert(X){\
    if ( (X) || _assert_fail(__FILE__, __LINE__, #X));\
}



char *__strchrnul(char *s, char c);
char *os_strtok(char *s, char *sep);
os_size_t os_strspn(char *s, char *accept);
os_size_t os_strcspn(char *s, char *reject);

void os_memcpy(uint32_t * source, uint32_t * dest, os_size_t size);


uint32_t km_size();
uint32_t kmcheck();


unsigned int rand();

#define UNUSED1(z) (void)(z)
#define UNUSED2(y,z) UNUSED1(y),UNUSED1(z)
#define UNUSED3(x,y,z) UNUSED1(x),UNUSED2(y,z)
#define UNUSED4(b,x,y,z) UNUSED2(b,x),UNUSED2(y,z)
#define UNUSED5(a,b,x,y,z) UNUSED2(a,b),UNUSED3(x,y,z)

#define VA_NUM_ARGS_IMPL(_1,_2,_3,_4,_5, N,...) N
#define VA_NUM_ARGS(...) VA_NUM_ARGS_IMPL(__VA_ARGS__, 5, 4, 3, 2, 1)

#define ALL_UNUSED_IMPL_(nargs) UNUSED ## nargs
#define ALL_UNUSED_IMPL(nargs) ALL_UNUSED_IMPL_(nargs)
#define ALL_UNUSED(...) ALL_UNUSED_IMPL( VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__ )


/// Log level is defined in the Makefile

#if LOG_LEVEL > 3
#define TRACE(format, ...)  kprintf("\e[90m[TRACE] " format "\e[0m\n", ##__VA_ARGS__)
#else
#define TRACE(...) ALL_UNUSED(__VA_ARGS__)
#endif

#if LOG_LEVEL > 2
#define DEBUG(format, ...)  kprintf("[DEBUG] \e[92m%s:%i\e[0m " format "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define DEBUG(...) ALL_UNUSED(__VA_ARGS__)
#endif
#if LOG_LEVEL > 1
#define INFO(format, ...)   kprintf("\e[96m[INFO]\e[0m " format "\n", ##__VA_ARGS__)
#else
#define INFO(...) ALL_UNUSED(__VA_ARGS__)
#endif
#if ENABLE_TESTS
#define WARN(format, ...)  kprintf("\e[38;5;208m[WARN] \e[38;5;208m%s:%i\e[38;5;208m" format "\e[0m\n", __FILE__, __LINE__, ##__VA_ARGS__); panic()
#else
#define WARN(format, ...)  kprintf("\e[38;5;208m[WARN] " format "\e[0m\n", ##__VA_ARGS__)
#endif


#define FATAL(format, ...) kprintf("\e[38;5;160m[FATAL] \e[38;5;208m%s:%i\e[38;5;160m " format "\e[0m\n", __FILE__, __LINE__, ##__VA_ARGS__); panic()

//4-17-15: Initial panic * assert_fail functions added
void panic() __attribute__ ((noreturn));
int _assert_fail(char *_file, unsigned int _line, char *_func);
    //__attribute__ ((__noreturn__));

void splash(void);

#endif
