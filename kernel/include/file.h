#ifndef __FILE_H__
#define __FILE_H__


#include "bitvector.h"
#include "linked_list.h"
#include <stdint.h>

struct file
{
	/* data */
	char* fname;
	list inode;
	uint16_t size;
	bitvector perms; // a bitvector of length three to track: read, write, execute
	time creation_time; // need to look up CourseOS specific data type
};

int kopen(char* filepath, char mode);
int kread(int fd, void* buf, int numBytes);
int kwrite(int fd, void* buf, int num_bytes);
int kclose(int fd);
int kseek(int fd, int num_bytes);
int kdelete(char* filepath);

#endif //__FILE_H__








