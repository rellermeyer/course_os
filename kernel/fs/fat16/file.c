
#include <stdint.h>
#include "../../include/linked_list.h"
#include "../../include/bitvector.h"
#include "../../include/open_table.h"
#include "../../include/mmci.h" 
#include "../../include/klibc.h"

#define NULL 0x0

//CONSTANTS:
const int SUPERBLOCK = 1;
// const int MAX_NAME_LENGTH = 32; moved this to a define
int MAX_BLOCKS;
// const int BLOCKSIZE = 512; moved this to a define...should be dynamic though...how?
// const int INODE_SIZE = 128; shouldn't need this...should be able to do sizeof(Inode)
int MAX_MEMORY;
int INODE_TABLE_CACHE_SIZE;
int NUM_INODE_TABLE_BLOCKS_TO_CACHE;
int INDIRECT_BLOCK_TABLE_CACHE_SIZE;
int NUM_INDIRECT_BLOCK_TABLE_BLOCKS_TO_CACHE;
int INODES_PER_BLOCK;

// FAKE CONSTANTS:
/* These should be read in the first time the FS is loaded */
int sd_card_capacity = 128000000; // 128 MB
int root_inum = 0;
int max_inodes = 4000; // will eventually be in superblock create
int inode_size = 512; // inodes will now be a full block size
int max_data_blocks = 200000;
int max_indirect_blocks = 25000;
int inode_bitmap_loc = 10 * BLOCKSIZE; // since we now have 250,000 blocks, we have plenty of space, so moving these to even start locations
int data_bitmap_loc = 50 * BLOCKSIZE;
int indirect_blocks_bitmap_loc = 100 * BLOCKSIZE; // note the indirect_blocks_bitmap will take 7 blocks bc 25000 max indirect blocks/512 block size ~ 6.1
int start_inode_table_loc = 1000;
int start_indirect_blocks_table_loc = 25000;
int start_data_blocks_loc = 50000;

struct superblock* FS;
bitVector* inode_bitmap;
bitVector* data_block_bitmap;
/* Need to as Luke about this */
struct inode** inode_table_cache; //is this right? we want an array of inode pointers...
struct indirect_block** indirect_block_table_cache; // an array of pointers to indirect_blocks
struct inode** inode_table_temp;
// void* data_table; not sure what this is or why we had/needed/wanted it...


/* make the global varrible */
// initialize the filesystem:
int kfs_init(int inode_table_cache_size, int indirect_block_table_cache_size){

	INODE_TABLE_CACHE_SIZE = inode_table_cache_size;
	INDIRECT_BLOCK_TABLE_CACHE_SIZE = indirect_block_table_cache_size;
	NUM_INODE_TABLE_BLOCKS_TO_CACHE = ((int) INODE_TABLE_CACHE_SIZE/BLOCKSIZE) + 1;
	NUM_INDIRECT_BLOCK_TABLE_BLOCKS_TO_CACHE = ((int) INDIRECT_BLOCK_TABLE_CACHE_SIZE/BLOCKSIZE) + 1;

	//initialize the SD Card driver:
	init_sd();
	//read in the super block from disk and store in memory:
	FS =  (struct superblock*) kmalloc(BLOCKSIZE);
	sd_receive((void*)FS, (SUPERBLOCK*BLOCKSIZE)); // make all blocks addresses, like here

	INODES_PER_BLOCK = (FS->block_size/FS->inode_size);

	//initialize the free list by grabbing it from the SD Card:
	inode_bitmap = (bitVector*) kmalloc(BLOCKSIZE); 
	sd_receive((void*) inode_bitmap, (FS->inode_bitmap_loc) * BLOCKSIZE); // have a pointer 
	// inode_bitmap = inode_bitmap_temp; // pointer to a bitvector representing iNodes

	data_block_bitmap = (bitVector*) kmalloc(BLOCKSIZE);
	sd_receive((void*) data_block_bitmap, (FS->data_bitmap_loc) * BLOCKSIZE);
	// data_block_bitmap = data_block_bitmap_temp; // pointer to bitvector representing free data blocks

	
	// initilize the inode_table_cache in memory:
	inode_table_temp = (struct inode**)kmalloc(NUM_INODE_TABLE_BLOCKS_TO_CACHE * BLOCKSIZE);
	inode_table_cache = (struct inode**) kmalloc((sizeof(struct inode*))* FS->max_inodes);
	int i;
	for(i = 0; i < FS->max_inodes; i++){
		if(i < NUM_INODE_TABLE_BLOCKS_TO_CACHE * INODES_PER_BLOCK){
			if(i % INODES_PER_BLOCK == 0){
				sd_receive((((void*)inode_table_temp) + ((i/INODES_PER_BLOCK)*BLOCKSIZE)), (FS->start_inode_table_loc + (i/INODES_PER_BLOCK)) * BLOCKSIZE);
			}
			inode_table_cache[i] = (struct inode*)((inode_table_temp + (((int)(i/INODES_PER_BLOCK))*BLOCKSIZE)) + ((i % INODES_PER_BLOCK)*FS->inode_size));
		}
		//each iteration through the loop will grab 1 inodes, since we can fit 1 inodes per block
		else{
			inode_table_cache[i] = NULL;
		}
	}//end for
	// inode_table_cache = (inode*) inode_table_temp;  // cast the void pointer to an Inode pointer


	// initilize the indirect_block_table_cache in memory:
	void* indirect_block_table_temp = (void*) kmalloc(NUM_INDIRECT_BLOCK_TABLE_BLOCKS_TO_CACHE * BLOCKSIZE);
	indirect_block_table_cache = (struct indirect_block**) kmalloc((sizeof(struct indirect_block*))* FS->max_indirect_blocks);
	for(i = 0; i < FS->max_indirect_blocks; i++){
		if(i < NUM_INDIRECT_BLOCK_TABLE_BLOCKS_TO_CACHE){
			sd_receive(indirect_block_table_temp + (i*BLOCKSIZE), (FS->start_indirect_block_table_loc) + (i*BLOCKSIZE));
			indirect_block_table_cache[i] = (struct indirect_block*)(indirect_block_table_temp + (i*BLOCKSIZE));
		} else{
			indirect_block_table_cache[i] = NULL;
		}
	}//end for

	fs_table_init(); //initializes the opentable 

	//what was this for? don't think we need it...
	// data_table = kmalloc(BLOCKSIZE); // pointer to the start of data table
	// receive(data_table, (FS->start_data_blocks_loc) * BLOCKSIZE); 
}//end fs_init() function

int kfs_shutdown(){
	int i;
	//TODO: write inodes pointed to by inode_table_cache back to disk to ensure it's up to date:

	//TODO: write indirect_blocks pointed to by indirect_block_table_cache back to disk to ensure it's up to date:


	//free inodes stored in inode_table_cache:
	for(i = 0; i < NUM_INODE_TABLE_BLOCKS_TO_CACHE; i++){
		if(inode_table_cache[i] != NULL){
			kfree(inode_table_cache[i]);
		}//end if
	}//end for

	//free inode_table_cache itself:
	kfree(inode_table_cache);

	//free indirect_blocks stored in indirect_block_table_cache:
	for(i = 0; i < NUM_INDIRECT_BLOCK_TABLE_BLOCKS_TO_CACHE; i++){
		if(indirect_block_table_cache[i] != NULL){
			kfree(indirect_block_table_cache[i]);
		}//end if
	}//end for

	//free indirect_block_table_cache itself:
	kfree(indirect_block_table_cache);
	kfree(inode_table_temp);
	//TODO: free anything else that needs to be freed...

	fs_table_shutdown(); //frees the free list in open_table.c

}//end kfs_shutdown() function


// -------------------------------------------------------------------------------------------------------------------------------------------------------
/* HELPER FUNCTIONS */

//from the index, gets the corresponding indirect block, either from cache or from disk
void get_indirect_block(int index, struct indirect_block* cur_indirect_block) {
	if(indirect_block_table_cache[index] != NULL){
		// the indirect_block is in the indirect_block_table_cache, so get it:
		*cur_indirect_block = *(indirect_block_table_cache[index]);	
	}else{ //RIGHT HERE
		// indirect_block is not in the cache table, so get it from disk:
		void* indirect_block_spaceholder = kmalloc(BLOCKSIZE);
		sd_receive(indirect_block_spaceholder, (index + FS->start_indirect_block_table_loc)*BLOCKSIZE); // the firs
		*cur_indirect_block = *((struct indirect_block*) indirect_block_spaceholder);
		kfree(indirect_block_spaceholder);
		// need to implement an eviction policy/function to update the indirect_block_table_cache...
	}//end if else
}//end get_indirect_block helper

//from the inum, gets corresponding inode, either from cache or disk
void get_inode(int inum, struct inode* cur_inode){
	if(inode_table_cache[inum] != NULL){
		// the inode is in the inode_cache_table, so get it:
		cur_inode = (inode_table_cache[inum]);	
	}else{ 
		// inode is not in the cache table, so get it from disk:
		void* inode_spaceholder = (void*) kmalloc(BLOCKSIZE);
		sd_receive(inode_spaceholder, ((inum/INODES_PER_BLOCK)+FS->start_inode_table_loc)*BLOCKSIZE); // the firs
		struct inode* block_of_inodes = (struct inode*) inode_spaceholder;
		*cur_inode = block_of_inodes[inum % INODES_PER_BLOCK];
		kfree(inode_spaceholder);
		// need to implement an eviction policy/function to update the inode_table_cache...
		// this will function w/o it, but should be implemented for optimization
	}//end if else
}//end get_indoe() helper function


int get_inum_from_direct_data_block(struct inode* cur_inode, char * next_path){
	int inum = -1;
	int i;
	int file_found = 0; // initialize to false (i.e. file not found)
	void* dir_spaceholder = (void*) kmalloc(BLOCKSIZE);
	for(i = 0; i < cur_inode->blocks_in_file; i++){
		sd_receive(dir_spaceholder, (cur_inode->data_blocks[i])*BLOCKSIZE);
		struct dir_data_block* cur_data_block = (struct dir_data_block*) dir_spaceholder;
		int j;
		for(j = 0; j < (cur_data_block->num_entries); j++){
			struct dir_entry* file_dir = cur_data_block[j];  
			if(!os_strcmp(file_dir->name, next_path)){
				file_found = 1; //we found the file, so break out of loop
				inum = file_dir->inum;
				break;
			}
			if(file_found){
				break;
			}
		}//inner for
		if(file_found){
			break;
		}
	}//outer for
	kfree(dir_spaceholder);
	return inum;
}//end get_inum_from_direct_data_block() helper helper function


int get_inum_from_indirect_data_block(struct inode * cur_inode, char * next_path) {
	int i;
	int inum = -1;
	int cur_indirect_block_num = -1;
	int file_found = 0; // initialize to false (i.e. file not found)
	struct indirect_block cur_indirect_block;
	for(i = 0; i < cur_inode->indirect_blocks_in_file; i++){
		cur_indirect_block_num = cur_inode->indirect_blocks[i];

		get_indirect_block(cur_indirect_block_num, &cur_indirect_block);

		void* dir_spaceholder = (void*) kmalloc(BLOCKSIZE);
		int j;
		for(j = 0; j < cur_indirect_block.blocks_in_file; j++){
			sd_receive(dir_spaceholder, (cur_indirect_block.data_blocks[j])*BLOCKSIZE);
			struct dir_data_block cur_data_block = *((struct dir_data_block*) dir_spaceholder);
			int k;
			for(k = 0; k < (cur_data_block->num_entries); k++){
				struct dir_entry* file_dir = cur_data_block[k]; // 
				if(!os_strcmp(file_dir->name, next_path)){
					file_found = 1; //we found the file, so break out of loop
					inum = file_dir->inum;
					break;
				}
			}//inner for
			if(file_found){
				break;
			}
		}//outer for
		kfree(dir_spaceholder);
		if(file_found){
			break;
		}
	}//end for
	return inum;
}//end of get_inum_from_indirect_data_block

//finds the inode (will be cur_inode) following filepath, going dir_levels down the path, starting from starting_inum
int kfind_inode(char* filepath, int starting_inum, int dir_levels, struct inode* cur_inode) { //filepath and starting inum must correspond...
	int inum = starting_inum;
	int a;
	for(a = 0; a < dir_levels; a++) {
		int k = 1;
		char next_path[MAX_NAME_LENGTH] = {0};
		while ((filepath[k] != '/') && (k <= MAX_NAME_LENGTH) && (filepath[k] != '\0')) {
			next_path[k] = filepath[k];
			k++;
		}//end of litte while to find next_path
		filepath += k;
		// Look up cur_inode inode in cached inode table
		get_inode(inum, cur_inode);
		inum = get_inum_from_direct_data_block(cur_inode, next_path);

		if(inum == -1){
			inum = get_inum_from_indirect_data_block(cur_inode, next_path);
		}

		if(inum == -1){//throw an error
			os_printf("404 ERROR! File not found.\nPlease ensure full filepath is specified starting from root (/)\n");
			return -1;
		}
	}//outer most for loop
	get_inode(inum, cur_inode);
	return 0;
}//end kfind_inode() helper function

//finds the name of the directory path (result->truncated_path) and the name of the ending part (result->last) and the number of levels (result->levels)
void kfind_dir(char* filepath, struct dir_helper* result){
	// struct dir_helper* result = (struct dir_helper*) kmalloc(sizeof(struct dir_helper));
	int dir_levels = 0;
	int total_chars = 0;
	char* iterator = filepath;
	int index = 0;
	while(index < MAX_NAME_LENGTH){
		if(iterator[0] == '/'){
			dir_levels++;
			index = 0;
		}else{
			if(iterator[0] == '\0'){
				break;
			}
			index++;
		}
		iterator++;
		total_chars++;
	}//end while
	total_chars -= index;

	char* truncated_path = (char*)kmalloc(total_chars); // do we need to kmalloc this?
	char* last = (char*)kmalloc(index);
	int i;
	for(i = 0; i < total_chars; i++){
		truncated_path[i] = filepath[i];
	}
	for(; i < index; i++){
		last[i-total_chars] = filepath[i];
	}
	result->dir_levels = dir_levels;
	result->truncated_path = truncated_path;
	result->last = last;
	return; //caller of function is responsible for freeing memory for truncated_path and filepath
}//end kfind_dir() function

//end of helper functions
// -----------------------------------------------------------------------------------------------------------------------------------------------------------


int kopen(char* filepath, char mode){
	int fd;
	int inum = 0;
	struct inode* cur_inode = (struct inode*) kmalloc(sizeof(struct inode));
	struct dir_helper* result = (struct dir_helper*) kmalloc(sizeof(struct dir_helper));
	kfind_dir(filepath, result);
	kfind_inode(filepath, inum, (result->dir_levels + 1), cur_inode);
	//here we have the file we were looking for! it is cur_inode.

	bitVector *p = cur_inode->perms;
	switch (mode){
		case 'r':
			if(get(0, p) == 0){
				os_printf("File Cannot Be Read\n");
				return -1;
			}
			break;
		case 'w':
			if(get(1, p) == 0){
				os_printf("File Cannot Be Written\n");
				return -1;
			}
			break;
		case 'a':
			if(get(1, p) == 0){
				os_printf("File Cannot Be Appeneded To\n");
				return -1;
			}	
			break;
		default:
			os_printf("Please specify permission as r to read, w to write and a to append\n");
	}
	fd = add_to_opentable(cur_inode, mode);
	kfree(result->truncated_path);
	kfree(result->last);
	kfree(result);
	return fd;
}//end kopen()


// Helper function for kread():
int read_partial_block(int bytesLeft, void* buf_offset, struct file_descriptor* fd, void* transferSpace) {
	int local_offset = fd->offset % BLOCKSIZE; // local_offset is the leftmost point in the block

	// Actually get the data for 1 block (the SD Driver will put it in transferSpace for us)
	int blockNum = fd->offset / BLOCKSIZE;
	int success = sd_receive(transferSpace, blockNum);
	if(success < 0){
	 	// failed on a block receive, therefore the whole kread fails; return failure error
	 	// os_printf("failed to receive block number %d\n", numBytes);
	 	return -1;
	}//end if

	if((local_offset == 0) && (bytesLeft < BLOCKSIZE)) { 
	/*	 ___________________
		|~~~~~~~~~|			|
		--------------------- */
		// Actually move the data to the user's specified buffer...must first cast void pointers to uint32_t* pointers:
		// source is transferSpace
		// dest is users buffer

		 os_memcpy(transferSpace, buf_offset, (os_size_t) bytesLeft); 	// note, this updates the buf_offset pointer as it transfer the data
		 																// os_memcpy takes uint32_t* as arguments
		 fd->offset += bytesLeft; // update the file descriptors file offset
		 // reset transferSpace pointer
		 transferSpace -= bytesLeft;
		 return bytesLeft; // note, we are returning the number of bytes that were successfully transferred

	} else if((local_offset > 0) && (bytesLeft >= (BLOCKSIZE - local_offset))) {
	/*	_____________________
		|           |~~~~~~~~|
		---------------------- */
		// Actually move the data to the user's specified buffer...must first cast void pointers to uint32_t* pointers:
		// source is transferSpace
		// dest is users buffer

		 os_memcpy((transferSpace + local_offset), buf_offset, (os_size_t) (BLOCKSIZE - local_offset)); 	// note, this updates the buf_offset pointer as it transfer the data
		 																// os_memcpy takes uint32_t* as arguments
		 fd->offset += (BLOCKSIZE - local_offset); // update the file descriptors file offset
		 // reset transferSpace pointer
		 transferSpace -= BLOCKSIZE;
		 return (BLOCKSIZE - local_offset); // note, we are returning the number of bytes that were successfully transferred

	} else if((local_offset > 0) && (bytesLeft < (BLOCKSIZE - local_offset))){
	/*	______________________
		|      |~~~~|         |
		----------------------- */
		// Actually move the data to the user's specified buffer...must first cast void pointers to uint32_t* pointers:
		// source is transferSpace
		// dest is users buffer

		 os_memcpy((transferSpace + local_offset), buf_offset, (os_size_t) bytesLeft); 	// note, this updates the buf_offset pointer as it transfer the data
		 																// os_memcpy takes uint32_t* as arguments
		 fd->offset += bytesLeft; // update the file descriptors file offset
		 // reset transferSpace pointer
		 transferSpace -= (local_offset + bytesLeft);
		 return bytesLeft; // note, we are returning the number of bytes that were successfully transferred

	} else{
		//this should never happen...print for debugging. TODO: remove after debugged
		os_printf("Error! In f1() in kread()...this should never happend!");
		return 0;
	}//end if else block
}//end of read_partial_block() helper function


// Helper function for kread():
int read_full_block(int bytesLeft, void* buf_offset, struct file_descriptor* fd, void* transferSpace) {
	// read BLOCKSIZE
	// Actually get the data for 1 block (the SD Driver will put it in transferSpace for us)
	int blockNum = fd->offset / BLOCKSIZE;
	int success = sd_receive(transferSpace, blockNum);
	if(success < 0){
	 	// failed on a block receive, therefore the whole kread fails; return failure error
	 	os_printf("failed to receive block number %d\n", blockNum);
	 	return -1;
	}//end if
	/*	______________________
		|~~~~~~~~~~~~~~~~~~~~~|
		----------------------- */
	// Actually move the data to the user's specified buffer...must first cast void pointers to uint32_t* pointers:
	// source is transferSpace
	// dest is users buffer
	 os_memcpy(transferSpace, buf_offset, (os_size_t) BLOCKSIZE); 	// note, this updates the buf_offset pointer as it transfer the data
	 																// os_memcpy takes uint32_t* as arguments
	 fd->offset += BLOCKSIZE; // update the file descriptors file offset
	 // reset transferSpace pointer
	 transferSpace -= BLOCKSIZE;
	 return BLOCKSIZE; // note, we are returning the number of bytes that were successfully transferred
}//end read_full_block() helper function


/* read from fd, put it in buf, then return the number of bytes read in numBytes */
int kread(int fd_int, void* buf, int numBytes) {
	int bytes_read = 0;
	uint32_t* buf_offset = buf; //this allows us to move data incrementally to user's buf via buf_offset
	//while retaining the original pointer to return back to the user
	struct file_descriptor* fd = get_descriptor(fd_int); // note, get_file_descriptor() function has not yet been implemented...will be in open_table.c

	if ((fd->permission != 'r' || fd->permission != 'b')) {
		os_printf("no permission\n");
		return -1;
	}

	// Allocate space for and create a bitvector to be used repeatedly to transfer the data:
	uint32_t* transferSpace = kmalloc(BLOCKSIZE);

	// start of higher-level algo:
	if(numBytes < BLOCKSIZE) {
		while(bytes_read < numBytes) {
			bytes_read += read_partial_block((numBytes-bytes_read), buf_offset, fd, transferSpace);
		}
	} else if(numBytes >= BLOCKSIZE) {
		bytes_read += read_partial_block((numBytes-bytes_read), buf_offset, fd, transferSpace);
		while((numBytes - bytes_read) > BLOCKSIZE) {
			bytes_read += read_full_block((numBytes-bytes_read), buf_offset, fd, transferSpace);
		}
		if(bytes_read < numBytes) {
			bytes_read += read_partial_block((numBytes-bytes_read), buf_offset, fd, transferSpace);
		}
	}//end else if
	if(bytes_read != numBytes){
		return bytes_read;
	}else{
		return -1;
	}	
} // end kread();



/* write from fd, put it in buf, then return the number of bytes written in numBytes */
int kwrite(int fd_int, void* buf, int num_bytes) {
        struct file_descriptor* fd = get_descriptor(fd_int);

        if ((fd->permission != 'w' || fd->permission != 'b')) {
            os_printf("no permission\n");
            return -1;
        }

    int total_bytes_left = num_bytes;
    int bytes_written = 0;

    uint32_t* buf_offset = buf;
    uint32_t* transferSpace = kmalloc(BLOCKSIZE);
    // os_memcpy(transferSpace, buf_offset, (os_size_t) BLOCKSIZE); 

    // have offset in the file already, just need to move it and copy.
    // fd->offset is the offset in the file. 
    int blockNum;
    int bytes_left_in_block;
    while(bytes_written < total_bytes_left) {
		blockNum = fd->offset / BLOCKSIZE;
		// need to put things in transferSpace, move pointer back when done
		bytes_left_in_block = BLOCKSIZE - (fd->offset % BLOCKSIZE);
		if(total_bytes_left <= bytes_left_in_block){
			/*	--------------- 			-----------------				
				|~~~~~~~|      |	 OR		|     |~~~~~|   |
				----------------			-----------------
			*/ 
			// write total_bytes_left
			os_memcpy(buf_offset, transferSpace, (os_size_t) total_bytes_left);
			transferSpace -= total_bytes_left;
			// pointer to start, blockNum, where we are in file, length of write
			sd_transmit(transferSpace, blockNum);

			bytes_written += total_bytes_left;
			total_bytes_left -= total_bytes_left;
			fd->offset += total_bytes_left;
		}
		else if(bytes_left_in_block <= total_bytes_left) {
			/* 
				------------
				|	    |~~~|
				------------
				read to the end of the block
			*/
			os_memcpy(buf_offset, transferSpace, (os_size_t) bytes_left_in_block);
			transferSpace -= bytes_left_in_block;
			// pointer to start, blockNum, where we are in file, lengh of write
			sd_transmit(transferSpace, blockNum);

			bytes_written += bytes_left_in_block;
			total_bytes_left -= bytes_left_in_block;
			fd->offset += bytes_left_in_block;
		} else {
			os_memcpy(buf_offset, transferSpace, (os_size_t) BLOCKSIZE);
			transferSpace -= BLOCKSIZE;
			// pointer to start, blockNum, where we are in file, lengh of write
			sd_transmit(transferSpace, blockNum);

			bytes_written += BLOCKSIZE;
			total_bytes_left -= BLOCKSIZE;
			fd->offset += BLOCKSIZE;
		}
	}
	return bytes_written;
} // end kwrite();


// close the file fd, return 1 if the close was successful 
int kclose(int fd) {
	int error;
	if(!file_is_open(fd)) { os_printf("file not open"); return -1; }
	error = delete_from_opentable(fd);
	return error;
} // end kclose();


// seek within the file, return an error if you are outside the boundaries 
int kseek(int fd_int, int num_bytes) {
        struct file_descriptor* fd = get_descriptor(fd_int);
        if (fd->permission != 'r' || fd->permission != 'w') {
            os_printf("no permission \n");
            return -1;
        } else if ((num_Bytes > 0) && ((fd->offset + num_Bytes) > ((fd->linked_file)->size))){
        	os_printf("Error! file offset exceeds file size \n");
			return -1;
        } else if ((num_Bytes < 0) && ((fd->offset + num_Bytes) < 0)){
			os_printf("Error! file offset exceeds beginning of file \n");
			return -1;
		}//end if else  */
	fd->offset += num_bytes;	
	return 0;
} // end kseek();



/* create a new file, if we are unsuccessful return -1 */
int kcreate(char* filepath, char mode, int is_this_a_dir) {
	int fd;
	int inum = 0;
	struct inode* cur_inode = (struct inode*) kmalloc(sizeof(struct inode));
	struct dir_helper* result = (struct dir_helper*) kmalloc(sizeof(struct dir_helper));
	kfind_dir(filepath, result);
	kfind_inode(result->truncated_path, inum, result->dir_levels, cur_inode);

	// at this point, the name of the file or dir to be created is “result->last” and it has to be added to cur_inode
	int free_inode_loc = firstFree(inode_bitmap); //Consult the inode_bitmap to find a free space in the inode_table to add the new inode
	if (free_inode_loc == -1) {
		os_printf("Disk has reached max number of files allowed. \n");
		return -1;
	}
	set(free_inode_loc, inode_bitmap);
	struct inode * new_inode = (struct inode*) kmalloc(sizeof(struct inode)); // Create the new inode
	//initialize all fields of inode:
	new_inode->inum = free_inode_loc;
	new_inode->fd_refs = 0; //will be incremented in add to opentable
	new_inode->size = 0; 
	new_inode->is_dir = is_this_a_dir; 
	new_inode->usr_id = 0; //or something
	new_inode->blocks_in_file = 0; 
	new_inode->data_blocks[MAX_DATABLOCKS_PER_INODE] = NULL; 
	new_inode->indirect_blocks_in_file = 0; 
	new_inode->indirect_blocks[NUM_INDIRECT_BLOCKS] = NULL; 
	switch (mode){
		case 'r':
			set(0, new_inode->perms);
			lower(1, new_inode->perms);
			break;
		case 'w':
			set(0, new_inode->perms);
			set(1, new_inode->perms);
			break;
		default:
			os_printf("Wrong permission. Please insert r for read and w for write\n");
			return -1;
	}
	//UPDATE DISK by writing memory data structures to disk
	sd_transmit((void*)inode_bitmap, FS->inode_bitmap_loc);
	sd_transmit((void*)new_inode, (FS->start_inode_table_loc + new_inode->inum * INODES_PER_BLOCK)*BLOCKSIZE); //if there are more than 1 inodeperblock need to change
	
	//update cur_inode directory to make it have the new inode 
	struct dir_data_block* dir_block = (struct dir_data_block*) kmalloc(BLOCKSIZE);
	if(cur_inode->blocks_in_file < MAX_DATABLOCKS_PER_INODE){
		sd_receive((void*) dir_block, (cur_inode->data_blocks[(cur_inode->blocks_in_file)-1])*BLOCKSIZE);
	}else{
		struct indirect_block* cur_indirect_block = (struct indirect_block*) kmalloc(BLOCKSIZE);
		get_indirect_block((cur_inode->indirect_blocks_in_file - 1), indirect_block); // do you want cur_indirect_block?
		sd_receive((void*) dir_block, (cur_indirect_block->data_blocks[(cur_indirect_block->blocks_in_file)-1])*BLOCKSIZE);
		kfree(cur_indirect_block);
	}
	struct dir_entry new_dir_entry;
	new_dir_entry.inum = free_inode_loc;
	new_dir_entry.name = result->last;
	// new_dir_entry.name_length = ??? // how to get this???
	if(dir_block->num_entries < MAX_DIR_ENTRIES_PER_DATA_BLOCK){
		dir_block->dir_entries[dir_block->num_entries] = new_dir_entry;
	}else{

		//RIGHT HERE...still need to:
		//add new data block to indirect block array if dir_block->num_entries >= MAX_DIR_ENTRIES_PER_DATA_BLOCK
		//add new indirect block AND new data block if cur_indirect_block->blocks_in_file >= MAX_DATABLOCKS_PER_INDIRECT_BLOCK
		//then update memory/caches
		//then write back out to disk
		//should move all this stuff out to helper function...
	}
	
	fd = add_to_opentable(new_inode, mode);
	return 0;
}


//delete the file or directory at filepath. Return -1 if the file does not exist 
int kdelete(char* filepath) {
	int inum = 0;
	struct inode* cur_inode = (struct inode*) kmalloc(sizeof(struct inode));
	struct dir_helper* result = (struct dir_helper*) kmalloc(sizeof(struct dir_helper));
	kfind_dir(filepath, result);
	kfind_inode(filepath, inum, (result->dir_levels + 1), cur_inode);
	//here we have the file we were looking for! it is cur_inode.

    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    // REAL DELETING HAS TO BE MADE                                                                                                                                    |
    // struct of inode has to be deleted from disk and removed from parents directory inode, and space has to be marked as free                                                                           |
    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------  
	
	//also free direct and indirect data blocks
	kfree(cur_inode);
	kfree(dir_helper);
	return 0;

} // end kdelete();
