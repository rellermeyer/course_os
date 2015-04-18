#ifndef __FILE_H__
#define __FILE_H__


#include "bitvector.h"
#include "linked_list.h"
#include <stdint.h>

#define BLOCKSIZE 512
#define MAX_NAME_LENGTH 32
#define MAX_DATABLOCKS_PER_INODE 27
#define DIR_ENTRY_SIZE 40


struct superblock
{
	char* fs_name; // 32 bytes (max length for this field abides by MAX_NAME_LENGTH)
	int fs_version; // 36 bytes
	int magic_num; // 40 bytes
	int sd_card_capacity; // 44 bytes
	int blocksize; // 48 bytes
	int root_inum; // 52 bytes
	int max_inodes; // 56 bytes
	int inode_size;
	int max_data_blocks; // 66 bytes
	int inode_bitmap_loc; // 70 bytes
	int data_bitmap_loc; // 74 bytes
	int start_inode_table_loc; // 78 bytes
	int start_data_blocks_loc; // 82 bytes, start_inode_table_loc + 200 b/c 200 inode bl
	// the rest of the superblock will be empty for now (BLOCKSIZE - 82 = 512 - 82 = 430 free/wasted bytes)
};

struct inode // this is metadata
{
	int size;
	int is_dir;
	int usr_id;
	int blocks_in_file;
	int data_blocks[MAX_DATABLOCKS_PER_INODE]; // how to get this dynamically? defined above as 27 right now
	char mode;
};

struct dir_entry
{
	int inum;
	int name_length; //including null terminating string
	char name[MAX_NAME_LENGTH]; // 32 chars right now
}; // 8 _ MAX_NAME_LENGTH bytes long...40 bytes right now

struct dir_data_block
{
	struct dir_entry dir_entries[(int)(BLOCKSIZE/DIR_ENTRY_SIZE)]
};

/* 
struct directory
{
	struct inode inode;
}; 
*/

struct file
{
	struct inode inode;
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



