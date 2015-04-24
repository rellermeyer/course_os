#include <stdint>
#include "../../include/linked_list.h"
//NOTES:
//free list = keeps track of which blocks on the storage device are free (i.e. available)...must be stored persistently


//CONSTANTS:
const uint16_t BLOCKSIZE = 4096;


//STRUCTS:
typedef struct file
{
	/* data */
	char* fname;
	llist_handle inode;
	uint16_t size;
	bitvector perms; // a bitvector of length three to track: read, write, execute
	time creation_time; // need to look up CourseOS specific data type
	uint16_t offset;
} file;

// typedef struct inode
// {
// 	 data 

// } inode;