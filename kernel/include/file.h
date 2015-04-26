#ifndef __FILE_H__
#define __FILE_H__


#include "bitvector.h"
#include "linked_list.h"
#include <stdint.h>

#define BLOCKSIZE 512
#define MAX_NAME_LENGTH 32
#define MAX_DATABLOCKS_PER_INODE 68
#define DIR_ENTRY_SIZE 40
#define MAX_NUM_INDIRECT_BLOCKS 50
#define MAX_DATABLOCKS_PER_INDIRECT_BLOCK (BLOCKSIZE/4)-2
#define MAX_DIR_ENTRIES_PER_DATA_BLOCK (int)((BLOCKSIZE-4)/DIR_ENTRY_SIZE)-2

//IMPORTANT!! ---------------------------------------------------------------------------------------------------
//all constants are in units of block NUMBER																	|
//the indexes of arrays are also in units of block NUMBER relative to the starting offset of their cathegory.   |
//---------------------------------------------------------------------------------------------------------------

//constants of file system, that wil be filled at boot time
struct superblock 
{
	char* fs_name; // 32 bytes (max length for this field abides by MAX_NAME_LENGTH)
	int fs_version; // 36 bytes
	int magic_num; // 40 bytes
	int sd_card_capacity; // 44 bytes
	int block_size; // 48 bytes
	int root_inum; // 52 bytes
	int max_inodes; // 56 bytes
	int inode_size;
	int max_data_blocks; // 66 bytes
	int inode_bitmap_loc; // 70 bytes
	int data_bitmap_loc; // 74 bytes
	int indirect_blocks_bitmap_loc; //
	int start_inode_table_loc; // 78 bytes  
	int start_data_blocks_loc; // 82 bytes, start_inode_table_loc + 200 b/c 200 inode bl
	int start_indirect_block_table_loc; //
	int max_indirect_blocks;
	// the rest of the superblock will be empty for now (BLOCKSIZE - 82 = 512 - 82 = 430 free/wasted bytes)
};

//metadata of each file or directory
struct inode {
	int inum; //inum of the file (4bytes)
	int fd_refs; //how many times the file is referenced (=appears in the opentable) (4bytes)
	int size; // size of the whole file (4 bytes)
	int is_dir; // 1 if this is a directory, 0 if this is a file (4 bytes)
	int usr_id; // id of the user who created the file (4 bytes)     ...not yet used!
	int direct_blocks_in_file; // how many direct block are being used  (4 bytes)
	int data_blocks[MAX_DATABLOCKS_PER_INODE]; // array of data (now long 68)
	int indirect_blocks_in_file; // how many indirect block are being used  (4 bytes)
	int indirect_blocks[MAX_NUM_INDIRECT_BLOCKS]; // 50*4 = 200 bytes ....50 indirect blocks right now 
	bit_vector* perms; // permissions of the file (12 bytes)
};

struct indirect_block // total size is 1 block
{
	int block_num;
	int blocks_in_file; //blocks actually used
	int data_blocks[MAX_DATABLOCKS_PER_INDIRECT_BLOCK]; // because this is just an array of ints, so it's BLOCKSIZE/4 bytes bc each int is 4 bytes 
};

struct dir_entry 
{
	int inum;
	int name_length; //including null terminating string
	char *name; // 32 chars right now
}; // 8 _ MAX_NAME_LENGTH bytes long...40 bytes right now

struct dir_data_block
{
	int block_num;
	int num_entries;
	struct dir_entry dir_entries[MAX_DIR_ENTRIES_PER_DATA_BLOCK];
};

struct dir_helper //used by helper functions in file.c
{
	int dir_levels;
	char* truncated_path;
	char* last;
};

/* 
struct directory
{
	struct inode inode;
}; 
*/

// struct file
// {
// 	struct inode* inode;
// 	// what to put here to avoid level of indirection?
// };

int kopen(char* filepath, char mode); //opens the file of filepath with permissions mode
int kread(int fd, void* buf, int numBytes); //reads the open file corresponding to fd
int kwrite(int fd, void* buf, int num_bytes); //writes the open file corresponding to fd
int kclose(int fd); //closes the cpen file corresponding to fd
int kseek(int fd, int num_bytes); //moves the offset of the open file fd
int kdelete(char* filepath); //deletes the file or directory following filepath
int kcreate(char* filepath, char mode, int is_this_a_dir); //creates and opens a file or directory with permissions mode in fielpath
int kcopy(char* source, char* dest, char mode); //copies the contents of a file 
int kls(char* filepath); //shows contents of one directory


// in the header file write it with extern. And in one of the c files declare it without extern.



#endif 

