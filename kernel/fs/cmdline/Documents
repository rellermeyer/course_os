#ifndef __FILE_H__
#define __FILE_H__


#include "bitvector.h"
#include <stdint.h>

#define BLOCKSIZE 512
#define MAX_NAME_LENGTH 32
#define MAX_DATABLOCKS_PER_INODE 68
#define DIR_ENTRY_SIZE 40
#define MAX_NUM_INDIRECT_BLOCKS 50
#define MAX_DATABLOCKS_PER_INDIRECT_BLOCK ((BLOCKSIZE/4)-2)
#define MAX_DIR_ENTRIES_PER_DATA_BLOCK ((int)((BLOCKSIZE-4)/DIR_ENTRY_SIZE)-2)

//IMPORTANT!! ---------------------------------------------------------------------------------------------------
//all constants are in units of block NUMBER																	|
//the indexes of arrays are also in units of block NUMBER relative to the starting offset of their cathegory.   |
//---------------------------------------------------------------------------------------------------------------

//constants of file system, that wil be filled at boot time
struct superblock 
{
	// char* fs_name; // 32 bytes (max length for this field abides by MAX_NAME_LENGTH)
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
	// int indirect_blocks_bitmap_loc; //
	int start_inode_table_loc; // 78 bytes  
	int start_data_blocks_loc; // 82 bytes, start_inode_table_loc + 200 b/c 200 inode bl
	// int start_indirect_block_table_loc; //
	// int max_indirect_blocks;
	// char spaceholder[???]; Might need this to make the cast from memory to superblock work...not sure??? Don't think we need this, but not sure
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
	char name[MAX_NAME_LENGTH]; // 32 chars right now
}; // 8 _ MAX_NAME_LENGTH bytes long...40 bytes right now

struct dir_data_block
{
	int block_num;
	int num_entries;
	struct dir_entry dir_entries[MAX_DIR_ENTRIES_PER_DATA_BLOCK];
};

struct data_block
{
	char data[BLOCKSIZE];
};

struct dir_helper //used by helper functions in file.c
{
	int dir_levels;
	char* truncated_path;
	char* last;
};

struct stats //statistics about the file
{
	int size; //size of the file
	int fd_refs; //how many times it is open now
	int is_dir; //is this a directory
};

int kopen(char* filepath, char mode); //opens the file of filepath with permissions mode
int kread(int fd, void* buf, int numBytes); //reads the open file corresponding to fd
int kwrite(int fd, void* buf, int num_bytes); //writes the open file corresponding to fd
int kclose(int fd); //closes the cpen file corresponding to fd
int kseek(int fd, int num_bytes); //moves the offset of the open file fd
int kdelete(char* filepath); //deletes the file or directory following filepath
int kcreate(char* filepath, char mode, int is_this_a_dir); //creates and opens a file or directory with permissions mode in fielpath
int kcopy(char* source, char* dest, char mode); //copies the contents of a file 
int kls(char* filepath); //shows contents of one directory
int kfs_init(int inode_table_cache_size, int data_block_table_cache_size, int reformat);
int kfs_shutdown();

// // -------------------------------------------------------------------------------------------------------------------------------------------------------
// /* HELPER FUNCTIONS */

//from the index, gets the corresponding indirect block, either from cache or from disk
void get_indirect_block(int index, struct indirect_block* cur_indirect_block);

//from the inum, gets corresponding inode, either from cache or disk
void get_inode(int inum, struct inode* result_inode);

//gets the inum of nextpath (file or dir) looking at the direct data blocks of cur_inode
int get_inum_from_direct_data_block(struct inode* cur_inode, char * next_path);

//gets the inum of netxpath (file or dir) looking at the indirect data blocks of cur_inode
int get_inum_from_indirect_data_block(struct inode * cur_inode, char * next_path);

//finds the inode (will be result_inode) following filepath, going dir_levels down the path, starting from starting_inum
int kfind_inode(char* filepath, int starting_inum, int dir_levels, struct inode* result_inode);

//finds the name of the directory path (result->truncated_path) and the name of the ending part (result->last) and the number of levels (result->levels)
//result has to be kmalloc-ed by and kfree-d by whoever calls this functinos. Also remember to free last and truncated_path. 
void kfind_dir(char* filepath, struct dir_helper* result);

//transmits or receives the data block bitvector or the inode bitvecotr to and from disk
// First parameter: TRANSMIT or RECEIVE (defined)
// Second paramter: put pointer to bitvector (example: data_block_bitmap for data, inode_bitmap for inodes)
// Third parameter: put where that bitvecotr starts in memory (example: FS->data_bitmap_loc for data, FS->inode_bitmap_loc for inode)
// Fourth parameter: how many there are (example: FS->max_data_blocks for data, FS->max_inodes for inodes)
// index = index you would put in the bitvector
// all = 0 for only one index, 1 for all the bitvector
int transmit_receive_bitmap(int t_or_r, bit_vector* vec, int starting_loc, int max, int bit_index, int all);

/* 	Helper function to add a new dir_entry to a directory file and optinally write it out to disk.
	Updates the last data block of the cur_inode (directory) to add a dir_entry that stores the mapping of the new inode to its inum */
int add_dir_entry(struct inode* cur_inode, int free_inode_loc, struct dir_helper* result);

int get_block_address(struct inode *file_inode, int block_num);

// Helper function for kread():
int read_partial_block(struct inode *c_inode, int offset, void* buf_offset, int bytes_left, void* transfer_space);

// Helper function for kread():
int read_full_block(struct inode *c_inode, int offset, void* buf_offset, int bytesLeft, void* transfer_space);;

int read_inode(struct inode *c_inode, int offset, void* buf, int num_bytes);

struct stats * get_stats(char * filepath, struct stats * result);


#endif 

