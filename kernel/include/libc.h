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
*	Usage:	Compile into kernel
********************************************************************/

#ifndef __libc_h
#define __libc_h

/* string.h type functionality for comparing strings or mem blocks */
int memcmp ( const void *left, const void *right, size_t num );
int strcmp ( const void *left, const void *right);

/* TODO: create print function for kernel debugging purposes */
