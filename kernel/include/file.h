#ifndef __FILE_H__
#define __FILE_H__


#include "bitvector.h"
#include "linked_list.h"
#include <stdint.h>

struct superblock
{
	char* fs_name; // 32 bytes (max length for this field abides by MAX_NAME_LENGTH)
	int fs_version; // 36 bytes
	int magic_num; // 40 bytes
	int sd_card_capacity; // 44 bytes
	int blocksize; // 48 bytes
	int root_inum; // 52 bytes
	int max_inodes; // 56 bytes
	int max_data_blocks; // 66 bytes
	int inode_bitmap_loc; // 70 bytes
	int data_bitmap_loc; // 74 bytes
	int start_inode_table_loc; // 78 bytes
	int start_data_blocks_loc; // 82 bytes
	// the rest of the superblock will be empty for now (BLOCKSIZE - 82 = 512 - 82 = 430 free/wasted bytes)
};

struct Inode // this is metadata
{
	char mode;
	int size;
	int* data_blocks[];
	int blocks_in_file;
	int usr_id;
	int is_dir;
};

/* 
struct directory
{
	struct inode inode;
}; 
*/

struct file
{
	struct Inode inode;
	// what to put here to avoid level of indirection?
};

int kopen(char* filepath, char mode);
int kread(int fd, void* buf, int numBytes);
int kwrite(int fd, void* buf, int num_bytes);
int kclose(int fd);
int kseek(int fd, int num_bytes);
int kdelete(char* filepath);

#endif //__FILE_H__




// in the header file write it with extern. And in one of the c files declare it without extern.



