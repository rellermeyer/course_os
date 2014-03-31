/********************************************************************
*	libc.c
*
*	Author: Jared McArthur	// any collaborators, please add name
*
*	Date last edit: 28 March 2014
*
*	Purpose:	Provide basic libc funtionality for CourseOS
<<<<<<< HEAD
*					This header provides function skeletons
					for libc.c
=======
*				This file provides function implementations
*				for skels in libc.h
>>>>>>> 29237d187daf60f494a9a916bf8df368ddd6c51a
*
*	Usage:	Compile into kernel. Adaptations of normal libc functions
*			can be used by prepending os_ suffix.
*
*	Notes:	memcmp and strcmp adapted directly from musl-libc
********************************************************************/

#include "include/libc.h"

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

/* TODO: create print function for kernel debugging purposes */

int main()
{ return 0; }
