/********************************************************************
*	libc.h
*
*	Author: Jared McArthur	// any collaborators, please add name
*
*	Date: 28 March 2014
*
*	Purpose:	Provide basic libc funtionality for CourseOS
*					This header provides function skeletons
					for libc.c
*
*	Usage:	Compile into kernel. Adaptations of normal libc functions
*			can be used by prepending os_ suffix.
********************************************************************/

#ifndef __libc_h
#define __libc_h
typedef unsigned int size_t;
/* string.h type functionality for comparing strings or mem blocks */
int os_memcmp ( const void *left, const void *right, size_t num );
int os_strcmp ( const char *left, const char *right);

int os_printf (const char *str_buf, ...);
/* TODO: create print function for kernel debugging purposes */

#endif
