/**
 * @file
 *
 * Course OS FS
 *
 * @author
 *
 * Joel Iventosh, Ginevra Gaudioso, Weston Sellek, Charlie Cox
 * Matt Davidson, Joseph Bourque
 *
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * Course OS FS is a custom file system (started out as FAT 16 but changed into this).
 * The file system uses a superblock to initialize the file system as well as the SD card
 * on boot. Data blocks are stored as array of integers that indexes into the corresponding
 * block number on the SD card (note: some blocks are reserved, data blocks use an offset to
 * access blocks on the SD card) DO NOT FORGET TO ADD OFFSET TO BLOCK NUMBERS!; Each file or
 * directory is stored with an inode which contains metadata about the file as well as the
 * list of addresses on disk where the data the file holds is stored. Directories are indicated
 * with the is_dir flag and data blocks for directories are dir_entry structs;
 * Each file has 100 direct blocks and 20 indirect blocks; no methods are
 * currently implemented to keep data contiguous.
 *
 * CURRENTLY WORKING
 * - Have working directories and directory hierarchy
 * - Standardized error codes; defined in header file
 * - Open file table working
 * - Create working
 * - Open working
 * - Close working
 * - Read working
 * - Write working
 *
 * TO FIX
 * - Max number of files per directory currently limited to 700 due to error finding
 *   and opening files; you can technically create more, but open will not find them
 * - indirect blocks do not seem to work
 * - ls, delete, and copy are untested; could work, but might not; TEST BEFORE USING!
 * - permissions
 *
 */
#include <stdint.h>
#include "klibc.h"
#include "data_structures/bitvector.h"
#include "fs/open_table.h"
#include "fs/file.h"
#include "drivers/mmci.h"

#define TRANSMIT 0
#define RECEIVE 1

//CONSTANTS:
const int SUPERBLOCK = 1;
// const int MAX_NAME_LENGTH = 32; moved this to a define
int MAX_BLOCKS;
// const int INODE_SIZE = 128; shouldn't need this...should be able to do sizeof(Inode)
int MAX_MEMORY;
int INODE_TABLE_CACHE_SIZE;
int NUM_INODE_TABLE_BLOCKS_TO_CACHE;
int INDIRECT_BLOCK_TABLE_CACHE_SIZE;
int NUM_INDIRECT_BLOCK_TABLE_BLOCKS_TO_CACHE;
int INODES_PER_BLOCK = 1;
int DATA_BLOCK_TABLE_CACHE_SIZE;
int NUM_DATA_BLOCK_TABLE_BLOCKS_TO_CACHE;


struct superblock* FS;
bit_vector* inode_bitmap;
bit_vector* data_block_bitmap;

struct inode** inode_table_cache; //is this right? we want an array of inode pointers...
struct indirect_block** indirect_block_table_cache; // an array of pointers to indirect_blocks
struct inode** inode_table_temp;
struct data_block** data_block_table_cache;
// void* data_table; not sure what this is or why we had/needed/wanted it...

/**
 * Format the SD card for Course OS FS
 *
 * Creates the super block & metadata for the file system, creates the
 * root directory of the file system and writes the first block on the
 * SD card as empty.
 *
 */
int kfs_format()
{
	os_printf("In kfs_format\n");
	// Lay down the superblock to block 1
	struct superblock sblock;
	sblock.fs_version = 1;
	sblock.magic_num = 0xDEADBEAF;
	sblock.sd_card_capacity = 128000000;
	sblock.block_size = BLOCKSIZE;
	sblock.root_inum = 0;
	sblock.max_inodes = 4000;
	sblock.inode_size = BLOCKSIZE;
	sblock.max_data_blocks = 200000;
	sblock.inode_bitmap_loc = 10;
	sblock.data_bitmap_loc = 50;
	sblock.start_inode_table_loc = 1000;
	sblock.start_data_blocks_loc = 50000;

	void *block = kmalloc(BLOCKSIZE);
	os_memset(block, 0, BLOCKSIZE);
	os_memcpy((uint32_t*)&sblock, block, sizeof(struct superblock));
	sd_transmit(block, 1*BLOCKSIZE);

	// Lay down the root inode
	struct inode root_inode;
	root_inode.inum = 0; // Isn't this redundant?
	root_inode.fd_refs = 0;
	root_inode.size = 0;
	root_inode.is_dir = 1;
	root_inode.usr_id = 0;
	root_inode.direct_blocks_in_file = 1;
	// root_inode.data_blocks[0] = sblock.start_data_blocks_loc;
	root_inode.data_blocks[0] = 0;
	root_inode.indirect_blocks_in_file = 0;
	os_memset(block, 0, BLOCKSIZE);
	os_memcpy((uint32_t*)&root_inode, block, sizeof(struct inode));
	sd_transmit(block, sblock.start_inode_table_loc*BLOCKSIZE);

	// Lay down the first (empty...) data block for the directory.
	struct dir_data_block ddb;
	ddb.block_num = 0;
	ddb.num_entries = 0;
	os_memset(block, 0, BLOCKSIZE);
	os_memcpy((uint32_t*)&ddb, block, sizeof(struct dir_data_block));
	sd_transmit(block, sblock.start_data_blocks_loc*BLOCKSIZE);

	// Update the inode bitmap
	/*unsigned char i = 0x80;
	os_memset(block, 0, BLOCKSIZE);
	os_memcpy(&i, block, 1);
	sd_transmit(block, sblock.inode_bitmap_loc*BLOCKSIZE);*/
	kfree(block);
	return SUCCESS;
}

/**
 * Load file system into memory and initialize the SD card
 *
 * Initializes the SD card to set it up for data transfer, once the SD card
 * is initialized metadata for the file system is loaded into memory and used
 * to obtain the free list table and data block table.
 *
 * @param
 *
 * int inode_table_cache_size - no longer needed; should be removed
 *
 * @param
 *
 * int data_block_table_cache_size - no longer needed; should be removed
 *
 * @param
 *
 * int reformat - if this is set to a non-zero value; this formats the file
 * system before initialization; if set to 0; initialization occurs with no
 * format
 *
 * @return
 *
 * Returns 0 if initialization was successful; does not do any error checking
 * or reporting at current; will need to be added in at some point in the future
 */
int kfs_init(int inode_table_cache_size, int data_block_table_cache_size, int reformat){

	INODE_TABLE_CACHE_SIZE = inode_table_cache_size;
	DATA_BLOCK_TABLE_CACHE_SIZE = data_block_table_cache_size;
	// TODO: Reading the root inode has weird issues (getting is_dir==0) with inode table block cache.
	NUM_INODE_TABLE_BLOCKS_TO_CACHE = 0;//((int) INODE_TABLE_CACHE_SIZE/BLOCKSIZE) + 1;
	NUM_DATA_BLOCK_TABLE_BLOCKS_TO_CACHE = 0;//((int) DATA_BLOCK_TABLE_CACHE_SIZE/BLOCKSIZE) + 1;

	//initialize the SD Card driver:
	if(!init_sd()){
		//initiallized successfully:
		os_printf("SD Card initiallized successfully!!!\n");
	}else{
		os_printf("FATAL ERROR!!!!! SD Card NOT initialized successfully!\n");
		//handle errors returned by SD Card...will implement this later...
	}//end else

	// Bootstrap the FS...
	if (reformat) {
		kfs_format();
	}

	//read in the super block from disk and store in memory:
	FS =  (struct superblock*) kmalloc(BLOCKSIZE);
	sd_receive((void*)FS, (SUPERBLOCK*BLOCKSIZE)); // make all blocks addresses, like here

	INODES_PER_BLOCK = (FS->block_size/FS->inode_size);

	//initialize the free list by grabbing it from the SD Card:
	inode_bitmap = make_vector(FS->max_inodes);
	data_block_bitmap = make_vector(FS->max_data_blocks);

	//HACK...shouldn't need to do this, but do, so leave for now
	int m; 
	for(m = 0; m < FS->max_inodes; m++){
		bv_lower(m, inode_bitmap);
	} //end HACK...shouldn't need to do this, but do, so leave for now

	bv_set(0, inode_bitmap);
	bv_set(0, data_block_bitmap);


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

	// initilize the data_block_table_cache in memory:
	void* data_block_table_temp = (void*) kmalloc(NUM_DATA_BLOCK_TABLE_BLOCKS_TO_CACHE * BLOCKSIZE);
	data_block_table_cache = (struct data_block**) kmalloc((sizeof(struct data_block*))* FS->max_data_blocks);
	for(i = 0; i < FS->max_data_blocks; i++){
		if(i < NUM_DATA_BLOCK_TABLE_BLOCKS_TO_CACHE){
			sd_receive(data_block_table_temp + (i*BLOCKSIZE), (FS->start_data_blocks_loc) + (i*BLOCKSIZE));
			data_block_table_cache[i] = (struct data_block*)(data_block_table_temp + (i*BLOCKSIZE));
		} else{
			data_block_table_cache[i] = NULL;
		}
	}//end for

	fs_table_init(); //initializes open_table stuff

	os_printf("Finished initializing table...\n");
	return SUCCESS;
}//end fs_init() function

/**
 * Removes the file system from memory
 *
 * Writes the filesystem metadata, the free block table and the data block table to disk
 * for persistant storage and then frees all memory which was being used to allocate those
 * resources
 *
 * @return
 *
 * Returns 0 if shutdown successfully loaded everything out of memory; no error handling or
 * recording is currently implemented; this needs to be implemented in the future so that
 * shutdown does not occur if this is called in the middle of a read or write
 */
int kfs_shutdown(){
	int i;
	//TODO: write inodes pointed to by inode_table_cache back to disk to ensure it's up to date:

	//TODO: write indirect_blocks pointed to by data_block_table_cache back to disk to ensure it's up to date:
	transmit_receive_bitmap(TRANSMIT, inode_bitmap, FS->inode_bitmap_loc, FS->max_inodes, 0, 1); //transmit inode bitmap
	transmit_receive_bitmap(TRANSMIT, data_block_bitmap, FS->data_bitmap_loc, FS->max_data_blocks, 0, 1); //transmit block bitmap

	//free inodes stored in inode_table_cache:
	for(i = 0; i < NUM_INODE_TABLE_BLOCKS_TO_CACHE; i++){
		if(inode_table_cache[i] != NULL){
			kfree(inode_table_cache[i]);
		}//end if
	}//end for

	//free inode_table_cache itself:
	kfree(inode_table_cache);

	//free indirect_blocks stored in data_block_table_cache:
	for(i = 0; i < NUM_DATA_BLOCK_TABLE_BLOCKS_TO_CACHE; i++){
		if(data_block_table_cache[i] != NULL){
			kfree(data_block_table_cache[i]);
		}//end if
	}//end for

	//free data_block_table_cache itself:
	kfree(data_block_table_cache);
	kfree(inode_table_temp);
	//TODO: free anything else that needs to be freed...

	fs_table_shutdown(); //frees open_table stuff
	return SUCCESS;
}//end kfs_shutdown() function


// -------------------------------------------------------------------------------------------------------------------------------------------------------
/* HELPER FUNCTIONS */

/**
 * Retrieves an indirect data block from the SD card
 *
 * Retrieves the specified indirect data block from the SD card using the passed index
 * and then sets it as the current indirect block
 *
 * @param
 *
 * struct inode* cur_inode - points to the file which we want to read the indirect block
 * index from
 *
 * @param
 *
 * int index - holds an index which corresponds to a location in the data block table
 *
 * @param
 *
 * struct indirect_block* cur_indirect_block - Holds the address of the current indirect
 * block
 *
 */
void get_indirect_block(struct inode* cur_inode, int index, struct indirect_block* cur_indirect_block) {
	// indirect_block is not in the cache table, so get it from disk:
	if(cur_inode->indirect_blocks_in_file <= 0){
		transmit_receive_bitmap(RECEIVE, data_block_bitmap, FS->data_bitmap_loc, FS->max_data_blocks, 0, 1);
		int new_indirect_block_loc = bv_firstFree(data_block_bitmap);
		bv_set(new_indirect_block_loc, data_block_bitmap);
		cur_inode->indirect_blocks[0] = new_indirect_block_loc;
		cur_inode->indirect_blocks_in_file++;
		transmit_receive_bitmap(TRANSMIT, data_block_bitmap, FS->data_bitmap_loc, FS->max_data_blocks, new_indirect_block_loc, 0);
		sd_transmit((void*) cur_inode, (cur_inode->inum + FS->start_inode_table_loc) * BLOCKSIZE);
		os_memset(cur_indirect_block, 0, BLOCKSIZE);
		cur_indirect_block->block_num = new_indirect_block_loc;
		cur_indirect_block->blocks_in_file = 0;
	}//end if
	sd_receive((void*) cur_indirect_block, (index + FS->start_data_blocks_loc)*BLOCKSIZE); // the firs
}//end get_indirect_block

/**
 * gets corresponding inode, from the SD card
 *
 * Retrieves an inode from the SD card using specified inum storing
 * it into a specified address.
 *
 * @param
 *
 * int inum - corresponds to a location in the inode table i.e. it is an
 * index to a location in the inode table
 *
 * @param
 *
 * inode* result_inode - holds a specified address to store the retrieved
 * inode at
 *
 */
//from the inum, gets corresponding inode, either from cache or disk
void get_inode(int inum, struct inode* result_inode){
	// if(inode_table_cache[inum] != NULL){
	// 	// the inode is in the inode_cache_table, so get it:
	// 	result_inode = (inode_table_cache[inum]);	
	// }else{ 
		// inode is not in the cache table, so get it from disk:
		// // struct inode* inode_spaceholder = (void*) kmalloc(BLOCKSIZE);
		// sd_receive((void*)inode_spaceholder, ((inum/INODES_PER_BLOCK)+FS->start_inode_table_loc)*BLOCKSIZE); // the firs
		// struct inode *block_of_inodes = inode_spaceholder;
		// result_inode = block_of_inodes[inum % INODES_PER_BLOCK];
		// kfree(inode_spaceholder);
		// need to implement an eviction policy/function to update the inode_table_cache...
		// this will function w/o it, but should be implemented for optimization
	// }//end if else
	sd_receive(result_inode, (inum + FS->start_inode_table_loc)*BLOCKSIZE);
}//end get_inode() helper function

/**
 * gets the inum of nextpath (file or dir) looking at the direct data blocks of cur_inode
 *
 * Uses the current file's  inode in order to retrieve the nested directory's inode
 * and then return it
 *
 * @param
 *
 * struct inode* cur_inode - holds the inode for the current data block
 *
 * @param
 *
 * char * next_path - the path you are searching
 *
 * @return
 *
 * Returns the inode of a specific nested directory; at this time there is no error checking
 * or reporting on this function; needs to be added in at a later data
 */
int get_inum_from_direct_data_block(struct inode* cur_inode, char * next_path){
	int inum = -1;
	int i;
	int file_found = 0; // initialize to false (i.e. file not found)
	struct dir_data_block* cur_data_block = (struct dir_data_block*) kmalloc(BLOCKSIZE);
	
	for(i = 0; i < cur_inode->direct_blocks_in_file; i++){
		sd_receive(cur_data_block, (cur_inode->data_blocks[i] + FS->start_data_blocks_loc)*BLOCKSIZE);

		int j;
		for(j = 0; j < (cur_data_block->num_entries); j++){
			struct dir_entry file_dir = cur_data_block->dir_entries[j];  
			if(!os_strcmp(file_dir.name, next_path)){
				file_found = 1; //we found the file, so break out of loop
				inum = file_dir.inum;
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
	kfree(cur_data_block);
	return inum;
}//end get_inum_from_direct_data_block() helper helper function

/**
 * Returns the index to an inode by searching through an indirect block
 *
 * gets the inum of netxpath (file or dir) looking at the indirect
 * data blocks of cur_inode
 *
 * @param
 *
 * struct inode* cur_inode - holds the inode for the current data block
 *
 * @param
 *
 * char * next_path - the path you are searching
 *
 * @return
 *
 * Returns the inode of a specific nested directory; at this time there is no error checking
 * or reporting on this function; needs to be added in at a later data
*/
int get_inum_from_indirect_data_block(struct inode * cur_inode, char * next_path) {
	int i;
	int inum = -1;
	int cur_indirect_block_num = -1;
	int file_found = 0; // initialize to false (i.e. file not found)
	struct indirect_block cur_indirect_block;
	for(i = 0; i < cur_inode->indirect_blocks_in_file; i++){
		cur_indirect_block_num = cur_inode->indirect_blocks[i];

		get_indirect_block(cur_inode, cur_indirect_block_num, &cur_indirect_block);

		void* dir_spaceholder = (void*) kmalloc(BLOCKSIZE);
		int j;
		int num_indirect_blocks = cur_inode->indirect_blocks_in_file;
		for(j = 0; j < num_indirect_blocks; j++){
			sd_receive(dir_spaceholder, (cur_indirect_block.data_blocks[j])*BLOCKSIZE);
			struct dir_data_block cur_data_block = *((struct dir_data_block*) dir_spaceholder);
			int k;
			for(k = 0; k < (cur_data_block.num_entries); k++){
				struct dir_entry file_dir = cur_data_block.dir_entries[k]; // 
				if(!os_strcmp(file_dir.name, next_path)){
					file_found = 1; //we found the file, so break out of loop
					inum = file_dir.inum;
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

/**
 * locates an inode using the file path and store it in the result_inode
 *
 * finds the inode (will be result_inode) following filepath, going
 * dir_levels down the path, starting from starting_inum
 *
 * @param
 *
 * char* filepath - holds the filepath leading to the inode
 *
 * @param
 *
 * int dir_levels - holds the number of directories the inode is nested
 * under
 *
 * @param
 *
 * struct inode* result_inode - Used to hold the inode once it has been
 * located
 *
 * @return
 *
 * Returns 0 if inode was successfully located and stored; otherwise
 * returns -1
 */
int kfind_inode(char* filepath, int starting_inum, int dir_levels, struct inode* result_inode) { //filepath and starting inum must correspond...
	int current_inum = starting_inum;

	int a = 0;
	for(a = 0; a < dir_levels-1; a++) {
		int k = 1;
		// int k = 0;
		char next_path[MAX_NAME_LENGTH] = {0};

		//get path of next inode
		while ((filepath[k] != '/') && (k <= MAX_NAME_LENGTH) && (filepath[k] != '\0')) {
			next_path[k-1] = filepath[k];
			k++;
		}//end of litte while to find next_path

		filepath += k;

		// Store inode with current_inum current_inum in result_inode 
		get_inode(current_inum, result_inode);

		current_inum = get_inum_from_direct_data_block(result_inode, next_path);

		if(current_inum < 0){
			//current_inum not found in any direct blocks of result_inode
			//look for it in the indirect blocks now
			current_inum = get_inum_from_indirect_data_block(result_inode, next_path);
		}

		if(current_inum < 0){
			//next_path not found in current_inode
			os_printf("404 ERROR! File not found.\nPlease ensure full filepath is specified starting from root (/)\n");
			return ERR_404; //file not found
		}
	}//outer most for loop
	//current_inum of target inode found, store that inode in result_inode

	get_inode(current_inum, result_inode);

	return SUCCESS;
}//end kfind_inode() helper function

/**
 * Uses the file path to obtain information about a directory and stores it in result
 *
 * finds the name of the directory path (result->truncated_path) and the name of the
 * ending part (result->last) and the number of levels (result->levels) result has to
 * be kmalloc-ed by and kfree-d by whoever calls this functinos. Also remember to free
 * last and truncated_path.
 *
 * @param  Description of method's or function's input parameter
 * @param  ...
 * @return Description of the return value
 */
void kfind_dir(char* filepath, struct dir_helper* result){
	int dir_levels = 0;
	int total_chars = 0;
	char* iterator = filepath; //root still level 0, so start from what's next
	int index = 0;
	while(index < MAX_NAME_LENGTH){
		if(iterator[0] == '\0'){
			//index++;
			break;
		}
		else if(iterator[0] == '/'){
			dir_levels++;
			index = 0;
		}else{
			index++;
		}
		iterator++;
		total_chars++;
	}//end while
	total_chars -= index;

	char* truncated_path = (char*)kmalloc(total_chars+1); // do we need to kmalloc this?
	char* last = (char*)kmalloc(index+1);
	int i;
	for(i = 0; i < total_chars; i++){
		truncated_path[i] = filepath[i];
	}
	truncated_path[i] = 0;
	for (i=0; i<index; i++) {
		last[i] = filepath[i+total_chars];
	}
	last[i] = 0;
	result->dir_levels = dir_levels;
	result->truncated_path = truncated_path;
	result->last = last;
	return; //caller of function is responsible for freeing memory for truncated_path and filepath
}//end kfind_dir() function

/**
 * Used to transmit a block to disk or recieve a block from disk
 *
 * transmits or receives the data block bitvector or the inode bitvecotr to and from disk
 *
 * @param
 *
 * int t_or_r - Determines whether operation is a TRANSMIT (0) to disk or a RECIEVE (1)
 * from disk
 *
 * @param
 *
 * bit_vector* vec - Pointer to a bitvector (either a data_block_bitmap for data or an
 * inode_bitmap for inodes)
 *
 * @param
 *
 * int starting_loc - holds the location where the bit vector starts in memory
 *
 * @param
 *
 * int max - holds the number of blocks to write to disk
 *
 * @param
 *
 * int all - Determines whether to only write 1 index (0) or whether to write the entire
 * bit vector (1) to disk
 *
 * @return
 *
 * Return 0 if the transmission/recieval was successful; otherwise returns an error code
 */
int transmit_receive_bitmap(int t_or_r, bit_vector* vec, int starting_loc, int max, int bit_index, int all){
	int error = 0;
	int num_blocks = (max/(8 * BLOCKSIZE)) + 1;
	if (t_or_r != TRANSMIT || t_or_r != RECEIVE) {
		return ERR_INVALID;
	}
	if(all){  //transmit or receive all the bitvector
		int i;
		for(i = 0; i < num_blocks; i++){
			if (t_or_r == TRANSMIT) {
				error = sd_transmit((void*) (vec->vector + (i*BLOCKSIZE)), (starting_loc + i)*BLOCKSIZE);
			}
			else {  //receive
				error = sd_receive((void*) (vec->vector + (i*BLOCKSIZE)), (starting_loc + i)*BLOCKSIZE);
			}
			if(error < 0){
				os_printf("ERROR! Failed to transmit or receive\n");
				return error;
			}
		}	
	}else{  //transmit or reveive only the block corresponding to the index
		int block_to_change = (bit_index/8)/BLOCKSIZE;
		if (t_or_r == TRANSMIT) {
			error = sd_transmit((void*) (vec->vector + (block_to_change*BLOCKSIZE)), (starting_loc + block_to_change)*BLOCKSIZE);
		}
		else { //receive
			error = sd_receive((void*) (vec->vector + (block_to_change*BLOCKSIZE)), (starting_loc + block_to_change)*BLOCKSIZE);
		}
		if(error < 0){
			os_printf("ERROR! Failed to transmit or receive\n");
			return error;
		}
	}
	return error;
}//end transmit_receive_bitmap helper function


/**
 * Adds a new file to an existing directory
 *
 * Helper function to add a new dir_entry to a directory file and
 * optinally write it out to disk. Updates the last data block of
 * the cur_inode (directory) to add a dir_entry that stores the
 * mapping of the new inode to its inum
 *
 * @param
 *
 * struct inode* cur_inode - holds the inode for the current data block
 *
 * @param
 *
 * int free_inode_loc - points to a location in the free inode table
 *
 * @param
 *
 * struct dir_helper* result - holds information about the directory you
 * are trying to add to
 *
 * @return
 *
 * Returns 0 if the file was added successfully; otherwise returns an error
 */
int add_dir_entry(struct inode* cur_inode, int free_inode_loc, struct dir_helper* result){

	int flag_free_cur_indirect_block = 0;

	//first get the appropriate data block, either from the array of direct data blocks from an indirect block:
	struct dir_data_block* dir_block = (struct dir_data_block*) kmalloc(BLOCKSIZE);
	struct indirect_block* cur_indirect_block;
	//if the cur_inode's array of direct data blocks has not reached max capacity, grab the last data block in the array to update:
	if((cur_inode->direct_blocks_in_file <= MAX_DATABLOCKS_PER_INODE)  && (cur_inode->indirect_blocks_in_file == 0)) {
		sd_receive((void*) dir_block, (cur_inode->data_blocks[(cur_inode->direct_blocks_in_file)-1] + FS->start_data_blocks_loc)*BLOCKSIZE);
	}else{
		//all the direct data blocks are full, so grab the last indirect block in the array of indirect blocks:
		cur_indirect_block = (struct indirect_block*) kmalloc(BLOCKSIZE);
		get_indirect_block(cur_inode, (cur_inode->indirect_blocks_in_file - 1), cur_indirect_block);
		sd_receive((void*) dir_block, (cur_indirect_block->data_blocks[(cur_indirect_block->blocks_in_file)-1] + FS->start_data_blocks_loc)*BLOCKSIZE);
		flag_free_cur_indirect_block = 1;
		// kfree(cur_indirect_block);
	}
	/* 	dir_block is now a direct data block that holds the content of the last data block in the file
		either from cur_inode's array of direct data blocks or from its last indirect data block */
	
	//create the new dir_entry and populate it's fields:
	struct dir_entry new_dir_entry;
	new_dir_entry.inum = free_inode_loc;
	os_strcpy(new_dir_entry.name, result->last);
	new_dir_entry.name_length = os_strlen(result->last);

	//check to see if the data block we recieved above has room to add a new dir_entry to it; if not, create a new data block, if possible:
	if(dir_block->num_entries < MAX_DIR_ENTRIES_PER_DATA_BLOCK){
		//the data block has room to add the new dir_entry, so we add it:
		dir_block->dir_entries[dir_block->num_entries] = new_dir_entry;
		dir_block->num_entries++;
		cur_inode->size += sizeof(struct dir_entry);

		sd_transmit((void*) dir_block, (dir_block->block_num + FS->start_data_blocks_loc) * BLOCKSIZE);
	}else{
		/* 	the data block doesn't have room to add the new dir_entry, so we need to add a new data block....
			either by adding:		(1) as a direct data block, 
									(2) to the current indirect block or 
									(3) add a new indirect block then add a new data block to that */
		//first kmalloc space for a new_dir_block:
		int new_data_block_loc = bv_firstFree(data_block_bitmap); //Consult the data_block_bitmap to find a free block to add the new data block at
		if(new_data_block_loc < 0){//disk is full
			os_printf("ERROR! Disk full\n");
			kfree(dir_block);
			if (cur_indirect_block != NULL) {
				kfree(cur_indirect_block);
			}
			return ERR_FULL;
		}
		bv_set(new_data_block_loc, data_block_bitmap);
		struct dir_data_block* new_dir_block = (struct dir_data_block*) kmalloc(BLOCKSIZE);

		new_dir_block->num_entries = 0;
		new_dir_block->block_num = new_data_block_loc;

		if(cur_inode->direct_blocks_in_file < MAX_DATABLOCKS_PER_INODE){
			//Case (1): add a direct data block to cur_inode:
			cur_inode->data_blocks[cur_inode->direct_blocks_in_file] = new_dir_block->block_num;
			cur_inode->direct_blocks_in_file++;

			new_dir_block->dir_entries[new_dir_block->num_entries] = new_dir_entry;
			new_dir_block->num_entries++;
			cur_inode->size += sizeof(struct dir_entry);
			sd_transmit((void*) new_dir_block, (new_dir_block->block_num + FS->start_data_blocks_loc) * BLOCKSIZE);
			transmit_receive_bitmap(TRANSMIT, data_block_bitmap, FS->data_bitmap_loc, FS->max_data_blocks, new_dir_block->block_num, 0);
		}else{
			//get the current indirect block and check to see if it has room to add another data block:
			if(!flag_free_cur_indirect_block){
				//if we don't have the indirect block, we need to get it from disk:
				cur_indirect_block = (struct indirect_block*) kmalloc(BLOCKSIZE);
				flag_free_cur_indirect_block = 1;
				get_indirect_block(cur_inode, (cur_inode->indirect_blocks_in_file - 1), cur_indirect_block);
				sd_receive((void*) dir_block, (cur_indirect_block->data_blocks[(cur_indirect_block->blocks_in_file)-1] + FS->start_data_blocks_loc)*BLOCKSIZE);
				// kfree(cur_indirect_block);
			}	
			if(cur_indirect_block->blocks_in_file < MAX_DATABLOCKS_PER_INDIRECT_BLOCK){
				//Case (2): add a new data block to to the current indirect block, then add the new_dir entry to the new data block:
				cur_indirect_block->data_blocks[cur_indirect_block->blocks_in_file] = new_dir_block->block_num;
				cur_indirect_block->blocks_in_file++;

				new_dir_block->dir_entries[new_dir_block->num_entries] = new_dir_entry;
				new_dir_block->num_entries++;
				cur_inode->size += sizeof(struct dir_entry);

				sd_transmit((void*) cur_indirect_block, (cur_indirect_block->block_num + FS->start_data_blocks_loc) * BLOCKSIZE);
				sd_transmit((void*) new_dir_block, (new_dir_block->block_num + FS->start_data_blocks_loc) * BLOCKSIZE);
				transmit_receive_bitmap(TRANSMIT, data_block_bitmap, FS->data_bitmap_loc, FS->max_data_blocks, new_dir_block->block_num, 0);
			}else{
				if(cur_inode->indirect_blocks_in_file < MAX_NUM_INDIRECT_BLOCKS){
					/*	Case (3): add a new indirect block to the cur_inode, then add a new data block to the new indirect block, 
						then add the new dir_entry to the new data block: */
					int new_indirect_block_loc = bv_firstFree(data_block_bitmap); //Consult the data_block_bitmap to find a free block to add the new data block at
					if (new_indirect_block_loc < 0) { //no free blocks
						os_printf("disk space over \n");
						kfree(dir_block);
						if (cur_indirect_block != NULL) {
							kfree(cur_indirect_block);
						}
						return ERR_FULL;
					}
					struct indirect_block* new_indirect_block = (struct indirect_block*) kmalloc(BLOCKSIZE);

					new_indirect_block->blocks_in_file = 0;
					new_indirect_block->block_num = new_indirect_block_loc;

					new_indirect_block->data_blocks[new_indirect_block->blocks_in_file] = new_dir_block->block_num;
					new_indirect_block->blocks_in_file++;
					cur_inode->indirect_blocks[cur_inode->indirect_blocks_in_file] = new_indirect_block->block_num;
					cur_inode->indirect_blocks_in_file++;

					new_dir_block->dir_entries[new_dir_block->num_entries] = new_dir_entry;
					new_dir_block->num_entries++;
					cur_inode->size += sizeof(struct dir_entry);

					if(indirect_block_table_cache[new_indirect_block->block_num] == NULL){
						//TODO: implement eviction policy...add the cur_inode to the cache:
					}else{
						*(indirect_block_table_cache[new_indirect_block->block_num]) = *(new_indirect_block);
					}
					bv_set(new_indirect_block_loc, data_block_bitmap); //taken
					sd_transmit((void*) new_indirect_block, (new_indirect_block->block_num + FS->start_data_blocks_loc) * BLOCKSIZE);
					sd_transmit((void*) new_dir_block, (new_dir_block->block_num + FS->start_data_blocks_loc) * BLOCKSIZE);
					transmit_receive_bitmap(TRANSMIT, data_block_bitmap, FS->data_bitmap_loc, FS->max_data_blocks, new_dir_block->block_num, 0);
					kfree(new_indirect_block);

				}else{
					//file has reached max allowable size:
					os_printf("ERROR! Operation failed because file has reached max allowable size\n");
					return ERR_FULL;
				}
			}
		}
		kfree(new_dir_block);
	}
	kfree(dir_block);
	if(flag_free_cur_indirect_block){
		kfree(cur_indirect_block);
	}
	return SUCCESS;
}//end add_dir_entry() helper function

/**
 * Obtains address on disk  of a specific data block within a file
 *
 * Uses the passed block number to search through the file's metadata and
 * obtain a block address to read off the SD card
 *
 * @param
 *
 * inode *file_inode - points to the specific file in which the data block
 * resides
 *
 * @param
 *
 * int file_block_num - holds the specific data block of the file to
 * obtain the address from
 *
 * @return
 *
 * Returns 0 if address was successfully retrieved; otherwise returns -1
 */
int get_block_address(struct inode *file_inode, int file_block_num){
	if(file_block_num < 0){
		os_printf("Invalid block number");
		return ERR_INVALID;
	}
	if(file_block_num < MAX_DATABLOCKS_PER_INODE){
			if (file_block_num >= file_inode->direct_blocks_in_file) {
			return ERR_GEN;
		}
		return (file_inode->data_blocks[file_block_num] + FS->start_data_blocks_loc) * BLOCKSIZE;
	}
	//Block must be stored in an indirect block of file_inode

	//Get the indirect block num that contain the target block
	int indirect_block_num = (file_block_num - MAX_DATABLOCKS_PER_INODE) / MAX_DATABLOCKS_PER_INDIRECT_BLOCK;
	if(indirect_block_num >= file_inode->indirect_blocks_in_file || indirect_block_num > (MAX_NUM_INDIRECT_BLOCKS - 1)){
		return ERR_GEN;
	}

	struct indirect_block* cur_indirect_block = (struct indirect_block*) kmalloc(sizeof(struct indirect_block));
	get_indirect_block(file_inode, file_inode->indirect_blocks[indirect_block_num], (struct indirect_block*) cur_indirect_block);

	int indirect_block_direct_block_num = (file_block_num - MAX_DATABLOCKS_PER_INODE) % MAX_DATABLOCKS_PER_INDIRECT_BLOCK;
	if (cur_indirect_block->blocks_in_file <= indirect_block_direct_block_num) {
		return ERR_GEN;
	}
	int block_address = cur_indirect_block->data_blocks[indirect_block_direct_block_num];
	block_address = (block_address + FS->start_data_blocks_loc) * BLOCKSIZE;
	kfree(cur_indirect_block);
	return block_address;
}

/**
 * Reads a partial portion of a block using the specified buffer size
 *
 * Reads a specified portion of a block from the passed file's inode
 * starting at the offset point and then store the results of that in
 * buf_offset
 *
 * @param
 *
 * struct inode *c_inode - The inode of the file we want to read
 *
 * @param
 *
 * int offset - starting point in the file to read from
 *
 * @param
 *
 * void* buf_offset - Holds the information read from the file
 *
 * @param
 *
 * int bytes_left - Holds the number of bytes to read from the file
 *
 * @param
 *
 * void* transfer_space - Used to hold data gotten from SD card block
 * however I don't think we need this
 * TODO: (maybe) replace transfer_space with buff_offset
 *
 * @return
 *
 * Returns the number of bytes which were transferred if successful;
 * otherwise returns an error code
 */
int read_partial_block(struct inode *c_inode, int offset, void* buf_offset, int bytes_left, void* transfer_space) {
	int local_offset = offset % BLOCKSIZE; // local_offset is the leftmost point in the block

	// Actually get the data for 1 block (the SD Driver will put it in transferSpace for us)
	int file_block_num = offset / BLOCKSIZE;
	int block_address = get_block_address(c_inode, file_block_num);
	if (block_address < 0) {
		return block_address; //return same error
	}

	int success = sd_receive(transfer_space, block_address);
	if(success < 0){
	 	// failed on a block receive, therefore the whole kread fails; return failure error
	 	return ERR_SD;
	}//end if block_num

	if((local_offset == 0) && (bytes_left < BLOCKSIZE)) { 
	/*	 ___________________
		|~~~~~~~~~|			|
		--------------------- */
		// Actually move the data to the user's specified buffer...must first cast void pointers to uint32_t* pointers:
		// source is transfer_space
		// dest is users buffer

		// note, this updates the buf_offset pointer as it transfer the data
		// os_memcpy takes uint32_t* as arguments
		os_memcpy(transfer_space, buf_offset, (os_size_t) bytes_left); 	
		return bytes_left; // note, we are returning the number of bytes that were successfully transferred

	} else if((local_offset > 0) && (bytes_left >= (BLOCKSIZE - local_offset))) {
	/*	_____________________
		|           |~~~~~~~~|
		---------------------- */
		// Actually move the data to the user's specified buffer...must first cast void pointers to uint32_t* pointers:
		// source is transferSpace
		// dest is users buffer

		 os_memcpy((transfer_space + local_offset), buf_offset, (os_size_t) (BLOCKSIZE - local_offset)); 	// note, this updates the buf_offset pointer as it transfer the data
		 																// os_memcpy takes uint32_t* as arguments
		 // reset transferSpace pointer
		// transfer_space -= BLOCKSIZE;

		 return (BLOCKSIZE - local_offset); // note, we are returning the number of bytes that were successfully transferred

	} else if((local_offset > 0) && (bytes_left < (BLOCKSIZE - local_offset))){
	/*	______________________
		|      |~~~~|         |
		----------------------- */
		// Actually move the data to the user's specified buffer...must first cast void pointers to uint32_t* pointers:
		// source is transferSpace
		// dest is users buffer

		 os_memcpy((transfer_space + local_offset), buf_offset, (os_size_t) bytes_left); 	// note, this updates the buf_offset pointer as it transfer the data
		 																// os_memcpy takes uint32_t* as arguments
		 // reset transferSpace pointer
		 transfer_space -= (local_offset + bytes_left);

		 return bytes_left; // note, we are returning the number of bytes that were successfully transferred

	}
	return ERR_GEN;
}//end of read_partial_block() helper function


/**
 * Reads a full block using the specified buffer size
 *
 * Reads a specified  block from the passed file's inode
 * starting at the offset point and then store the results of that in
 * buf_offset
 *
 * @param
 *
 * struct inode *c_inode - The inode of the file we want to read
 *
 * @param
 *
 * int offset - starting point in the file to read from
 *
 * @param
 *
 * void* buf_offset - Holds the information read from the file
 *
 * @param
 *
 * int bytes_left - Holds the number of bytes to read from the file
 *
 * @param
 *
 * void* transfer_space - Used to hold data gotten from SD card block
 * however I don't think we need this
 * TODO: (maybe) replace transfer_space with buff_offset
 *
 * @return
 *
 * Returns the number of bytes which were transferred if successful;
 * otherwise returns an error code
 */
int read_full_block(struct inode *c_inode, int offset, void* buf_offset, int bytesLeft, void* transfer_space) {
	// read BLOCKSIZE
	// Actually get the data for 1 block (the SD Driver will put it in transfer_space for us)
	int file_block_num = offset / BLOCKSIZE;
	int block_address = get_block_address(c_inode, file_block_num);
	if (block_address < 0) {
		return block_address; //return same error
	}
	int success = sd_receive(transfer_space, block_address);
	if(success < 0){
	 	// failed on a block receive, therefore the whole kread fails; return failure error
	 	os_printf("failed to receive block number %d\n", file_block_num);
	 	return ERR_SD;
	}//end if
	/*	______________________
		|~~~~~~~~~~~~~~~~~~~~~|
		----------------------- */
	// Actually move the data to the user's specified buffer...must first cast void pointers to uint32_t* pointers:
	// source is transferSpace
	// dest is users buffer
	 os_memcpy(transfer_space, buf_offset, (os_size_t) BLOCKSIZE); 	// note, this updates the buf_offset pointer as it transfer the data
	 // os_memcpy takes uint32_t* as arguments
	 // reset transferSpace pointer
	 return BLOCKSIZE; // note, we are returning the number of bytes that were successfully transferred
}//end read_full_block() helper function

/**
 *  Reads the number of bytes specified from the target inode
 *
 * Calls either read_partial_block or read_full_block based on the number
 * of bytes to read; will continue to call these functions until number of bytes
 * specified has been met
 *
 * @param  Description of method's or function's input parameter
 * @param  ...
 * @return Description of the return value
 */
int read_inode(struct inode *c_inode, int offset, void* buf, int num_bytes){
	// Allocate space for and create a bitvector to be used repeatedly to transfer the data:
	uint32_t *transfer_space = kmalloc(BLOCKSIZE);
	int bytes_read = 0;

	if (num_bytes+offset > c_inode->size) {
		num_bytes = c_inode->size - offset;
	}

	// start of higher-level algo:
	if(num_bytes < BLOCKSIZE) {
		while(bytes_read < num_bytes) {
			int x = read_partial_block(c_inode, offset + bytes_read, buf, (num_bytes-bytes_read),transfer_space);
			bytes_read += x;
		}

	} else if(num_bytes >= BLOCKSIZE) {
		//Read in remainder of current block
		bytes_read += read_partial_block(c_inode, offset + bytes_read, buf, (num_bytes-bytes_read),transfer_space);
		//Read in maximum number of full blocks
		while((num_bytes - bytes_read) >= BLOCKSIZE) {
			bytes_read += read_full_block(c_inode, offset + bytes_read, buf+bytes_read, (num_bytes-bytes_read),transfer_space);
		}
		//If not completely read yet, read in remainder
		if(bytes_read < num_bytes) {
			bytes_read += read_partial_block(c_inode, offset + bytes_read, buf, (num_bytes-bytes_read),transfer_space);
		}
	}//end else if

	kfree(transfer_space);
	return bytes_read;
}


//end of helper functions
// -----------------------------------------------------------------------------------------------------------------------------------------------------------

/**
 * Kernel level open file; adds a new entry to the open file table
 *
 * Opens a file using the specified file path and sets it in either
 * read or write mode.
 *
 * @param
 *
 * char* filepath - holds the pathname to the file to be opened
 *
 * @param
 *
 * char mode - holds either r or w indicating whether the file
 * is to be opened in read or write mode
 *
 * @return
 *
 * Returns 0 if the file was opened correctly; otherwise returns
 * -1
 *
 */
int kopen(char* filepath, char mode){
	if (filepath == NULL) {
		os_printf("no directory specified \n");
		return ERR_INVALID;
	}
	if (mode == 0) {
		os_printf("no mode specified \n");
		return ERR_INVALID;
	}
	int fd;
	int inum = 0;
	struct inode* cur_inode = (struct inode*) kmalloc(sizeof(struct inode));
	struct dir_helper* result = (struct dir_helper*) kmalloc(sizeof(struct dir_helper));
	kfind_dir(filepath, result);
	int error = kfind_inode(filepath, inum, (result->dir_levels + 1), cur_inode);
	
	//here we have the file we were looking for! it is cur_inode.
	if (error < 0 || cur_inode->is_dir) {
		if (error < 0) {
			os_printf("file not found, exiting kopen\n");
		}
		else {
			os_printf("cannot open a directory, make the path end to a file\n");
			error = ERR_INVALID;
		}
		kfree(cur_inode);
		kfree(result->truncated_path);
		kfree(result->last);
		kfree(result);

		return error; //return same error
	}

	// TODO: Permissions need some work, because the bitvector cannot be pulled out like this. :(
	/*bit_vector *p = cur_inode->perms;
	switch (mode){
		case 'r':
			if(bv_get(0, p) == 0){
				os_printf("File Cannot Be Read\n");
				return -1;
			}
			break;
		case 'w':
			if(bv_get(1, p) == 0){
				os_printf("File Cannot Be Written\n");
				return -1;
			}
			break;
		case 'a':
			if(bv_get(1, p) == 0){
				os_printf("File Cannot Be Appeneded To\n");
				return -1;
			}	
			break;
		default:
			os_printf("Please specify permission as r to read, w to write and a to append\n");
			}*/
	fd = add_to_opentable(cur_inode, mode);
	kfree(result->truncated_path);
	kfree(result->last);
	kfree(result);
	return fd;
}//end kopen()

/**
 * Reads a specified number of bytes from the SD card
 *
 * read from fd, put it in buf, then return the number of bytes read
 * in numBytes
 *
 * @param
 *
 * int fd_int - index to a file descriptor in the open file table
 * @param
 *
 * void* buf - buffer to hold the data read from the SD card
 *
 * @param
 *
 * int num_bytes - holds the number of bytes to store in the buffer
 *
 * @return
 *
 * Returns the number of bytes read from the SD card if successful,
 * otherwise returns an error code
 */
int kread(int fd_int, void* buf, int num_bytes) {
	if (fd_int < 0 || fd_int >= SYSTEM_SIZE) {
		os_printf("kread, fd not valid \n");
		return ERR_INVALID;
	}
	if (buf == NULL) {
		os_printf("no buffer \n");
		return ERR_INVALID;
	}
	if (num_bytes <= 0) {
		os_printf("invalid number of bytes \n");
		return ERR_INVALID;
	}
	int bytes_read = 0;
	uint32_t* buf_offset = buf; //this allows us to move data incrementally to user's buf via buf_offset
	//while retaining the original pointer to return back to the user
	struct file_descriptor* fd = get_descriptor(fd_int); // note, get_file_descriptor() function has not yet been implemented...will be in open_table.c

	if ((fd->permission != 'r') && (fd->permission != 'b')) {
		os_printf("no permission\n");
		return ERR_PERM;
	}
	bytes_read = read_inode(fd->linked_file, fd->offset, buf_offset, num_bytes);

	fd->offset += bytes_read;
	return bytes_read;
} // end kread();


/**
 * Write a specified number of bytes to the SD card
 *
 * write from fd, put it in buf, then return the number of bytes written in
 * numBytes
 *
 * @param
 *
 * int fd_int - holds the index of a file descriptor in the open file table
 *
 * @param
 *
 * void* buf - holds the data to be written out to disk
 *
 * @param
 *
 * int num_bytes - holds the number of bytes to write to disk
 *
 * @return
 * Returns the number of bytes written to disk if successful, otherwise
 * returns an error code
 */
int kwrite(int fd_int, void* buf, int num_bytes) {
	if (fd_int < 0 || fd_int >= SYSTEM_SIZE) {
		os_printf("kwrite, fd not valid \n");
		return ERR_INVALID;
	}
	if (buf == NULL) {
		os_printf("no buffer \n");
		return ERR_INVALID;
	}
	if (num_bytes <= 0) {
		os_printf("invalid number of bytes \n");
		return ERR_INVALID;
	}
    struct file_descriptor* fd = get_descriptor(fd_int);

    if ((fd->permission != 'w' && fd->permission != 'a')){
        os_printf("ERROR! File was opened without write permissions.\nYou may or may not have permission to write to this file.\nTry opneing with 'w' permission\n");
        return ERR_PERM;
    }

    int total_bytes_left = num_bytes;
    int bytes_written = 0;

    struct indirect_block* cur_indirect_block; 
    void* buf_offset = buf;
    void* transfer_space = kmalloc(BLOCKSIZE);
    // os_memcpy(transferSpace, buf_offset, (os_size_t) BLOCKSIZE); 
    struct inode* cur_inode = fd->linked_file;
    // have offset in the file already, just need to move it and copy.
    // fd->offset is the offset in the file. 
    //while(bytes_written < total_bytes_left){
    while (total_bytes_left > 0) {
		int file_block_num = fd->offset / BLOCKSIZE;
		// need to put things in transfer_space, move pointer back when done
		int offset_into_current_block = fd->offset % BLOCKSIZE;
		int bytes_left_in_block = BLOCKSIZE - offset_into_current_block;
		int block_address = get_block_address(cur_inode, file_block_num);

		//if get_block_address == -1, we know we need to allocate more space for the file
		if(block_address < 0){
			int new_data_block_loc = bv_firstFree(data_block_bitmap);
			if(new_data_block_loc < 0){//disk is completley full
				os_printf("ERROR! disk full\n");
				return ERR_FULL;
			}
			block_address = (new_data_block_loc + FS->start_data_blocks_loc) * BLOCKSIZE;
			bv_set(new_data_block_loc, data_block_bitmap);

			int flag_free_cur_indirect_block = 0;
			if(cur_inode->direct_blocks_in_file < MAX_DATABLOCKS_PER_INODE){
				//Case (1): add a direct data block to cur_inode:
				cur_inode->data_blocks[cur_inode->direct_blocks_in_file] = new_data_block_loc;
				cur_inode->direct_blocks_in_file++;
				transmit_receive_bitmap(TRANSMIT, data_block_bitmap, FS->data_bitmap_loc, FS->max_data_blocks, new_data_block_loc, 0);
			}else{//(cur_inode->direct_blocks_in_file >= MAX_DATABLOCKS_PER_INODE
				//get the current indirect block and check to see if it has room to add another data block:
				cur_indirect_block = (struct indirect_block*) kmalloc(BLOCKSIZE);
				flag_free_cur_indirect_block = 1;
				get_indirect_block(cur_inode, cur_inode->indirect_blocks[cur_inode->indirect_blocks_in_file-1], cur_indirect_block);

				if(cur_indirect_block->blocks_in_file < MAX_DATABLOCKS_PER_INDIRECT_BLOCK && cur_inode->indirect_blocks_in_file != 0){
					//Case (2): add a new data block to to the current indirect block:
					cur_indirect_block->data_blocks[cur_indirect_block->blocks_in_file] = new_data_block_loc;
					cur_indirect_block->blocks_in_file++;

					// Causes data abort errors for some reason.
					/*if(indirect_block_table_cache[cur_indirect_block->block_num] == NULL){
						//TODO: implement eviction policy...add the cur_inode to the cache:
					}else{
						*(indirect_block_table_cache[cur_indirect_block->block_num]) = *(cur_indirect_block);
						}*/

					sd_transmit((void*) cur_indirect_block, (cur_indirect_block->block_num + FS->start_data_blocks_loc) * BLOCKSIZE);
					transmit_receive_bitmap(TRANSMIT, data_block_bitmap, FS->data_bitmap_loc, FS->max_data_blocks, new_data_block_loc, 0);
				}else{ //last indirect block full
					if(cur_inode->indirect_blocks_in_file < MAX_NUM_INDIRECT_BLOCKS){
						/*	Case (3): add a new indirect block to the cur_inode, then add a new data block to the new indirect block */
						int new_indirect_block_loc = bv_firstFree(data_block_bitmap); //Consult the data_block_bitmap to find a free block to add the new data block at
						if(new_indirect_block_loc < 0){
							os_printf("ERROR! Disk full\n");
							return ERR_FULL;
						}
						bv_set(new_indirect_block_loc, data_block_bitmap);

						struct indirect_block* new_indirect_block = (struct indirect_block*) kmalloc(BLOCKSIZE);
						new_indirect_block->blocks_in_file = 0;
						new_indirect_block->block_num = new_indirect_block_loc;

						new_indirect_block->data_blocks[new_indirect_block->blocks_in_file] = new_data_block_loc;
						new_indirect_block->blocks_in_file++;
						cur_inode->indirect_blocks[cur_inode->indirect_blocks_in_file] = new_indirect_block_loc;
						cur_inode->indirect_blocks_in_file++;

						/*if(indirect_block_table_cache[new_indirect_block->block_num] == NULL){
							//TODO: implement eviction policy...add the cur_inode to the cache:
						}else{
							*(indirect_block_table_cache[new_indirect_block->block_num]) = *(new_indirect_block);
							}*/

						void *buf = kmalloc(BLOCKSIZE);
						os_memcpy((uint32_t *) new_indirect_block, buf, sizeof(struct indirect_block));
						sd_transmit(buf, (new_indirect_block->block_num + FS->start_data_blocks_loc) * BLOCKSIZE);
						kfree(buf);
						transmit_receive_bitmap(TRANSMIT, data_block_bitmap, FS->data_bitmap_loc, FS->max_data_blocks, new_indirect_block_loc, 0);
						kfree(new_indirect_block);

					}else{
						//file has reached max allowable size:
						os_printf("ERROR! Operation failed because file has reached max allowable size\n");
						return ERR_FULL;
					}
				}
			}
			if(flag_free_cur_indirect_block){
				kfree(cur_indirect_block);
			}
		}//end if(block_address == -1)

		//now we have added new blocks to the file if necessary (and possible), so actually execute the write:	
		if(total_bytes_left <= bytes_left_in_block){
			//note, that here we are overwriting, but will never have to allocate new data blocks for the file
			/*	--------------- 			-----------------				
				|~~~~~~~|      |	 OR		|     |~~~~~|   |                           
				----------------			-----------------
			*/ 
			//Remaining bytes to write will fit in the current block
			//first read the block in, so that the first part of it doesn't get ovewritten:
			sd_receive(transfer_space, block_address);
			// write total_bytes_left
			os_memcpy(buf_offset, transfer_space + offset_into_current_block, (os_size_t) total_bytes_left);
			//transfer_space -= total_bytes_left; //Purpose of this?
			// pointer to start, block_num, where we are in file, length of write
			sd_transmit(transfer_space, block_address);

			bytes_written += total_bytes_left;
			fd->offset += total_bytes_left;
			cur_inode->size += total_bytes_left;
			total_bytes_left = 0; //at this point there is nothing left to write, so we're done
		}
		else{//(total_bytes_left > bytes_left_in_block) so...we might need to allocate new data blocks for the file...
			/* 
				------------
				|	    |~~~|
				------------
				write to the end of the block
			*/

			//Remaining bytes will not fit in current block, fill up remainder of block
			//first read the block in, so that the first part of it doesn't get ovewritten:
			sd_receive(transfer_space, block_address);
			// write total_bytes_left
			os_memcpy(buf_offset, transfer_space + offset_into_current_block, (os_size_t) bytes_left_in_block);

			//transfer_space -= bytes_left_in_block; //Purpose of this?
			// pointer to start, blockNum, where we are in file, lengh of write
			sd_transmit(transfer_space, block_address);

			bytes_written += bytes_left_in_block;
			total_bytes_left -= bytes_left_in_block;
			fd->offset += bytes_left_in_block;
			buf_offset += bytes_left_in_block;
			cur_inode->size += bytes_left_in_block;
		}

	}//end while
	//update the the cur_inode on disk:
	sd_transmit(cur_inode, (cur_inode->inum + FS->start_inode_table_loc) * BLOCKSIZE);
	return bytes_written;
} // end kwrite();

/**
 * Close the file
 *
 * Remove the file from the open file table and free the file's inode
 *
 * @param
 *
 * int fd - holds an index of a file descriptor in the open file table
 *
 * @return
 *
 * Returns 1 if the file was successfully closed, otherwise returns an error
 */
int kclose(int fd) {
	if (fd < 0 || fd >= SYSTEM_SIZE) {
		os_printf("kclose, fd not valid \n");
		return ERR_INVALID;
	}
	int error;
	if(!file_is_open(fd)) { 
		os_printf("file not open"); 
		return ERR_GEN; 
	}
	error = delete_from_opentable(fd); //this also frees inode
	return error;
} // end kclose();

/**
 * Increment  seek position within a file
 *
 * Increments the seek pointer of a specific file by a number of
 * specified bytes
 *
 * @param
 *
 * int fd_int - holds the index of a file descriptor in the open file table
 *
 * @param
 *
 * int num_bytes - holds the number of bytes to move the seek pointer by
 *
 * @return
 *
 * Returns 0 if the seek pointer was successfully moved; otherwise returns
 * an error code
 */
int kseek(int fd_int, int num_bytes) {
	if (fd_int < 0 || fd_int >= SYSTEM_SIZE) {
		os_printf("kseek, fd not valid \n");
		return ERR_INVALID;
	}
	if (num_bytes <= 0) {
		os_printf("num of bytes not valid \n");
		return ERR_INVALID;
	}
    struct file_descriptor* fd = get_descriptor(fd_int);
    if (fd->permission != 'r' || fd->permission != 'w') {
        os_printf("no permission \n");
        return ERR_PERM;
    } else if ((num_bytes > 0) && ((fd->offset + num_bytes) > ((fd->linked_file)->size))){
    	os_printf("Error! file offset exceeds file size \n");
		return ERR_GEN;
    } else if ((num_bytes < 0) && ((fd->offset + num_bytes) < 0)){
		os_printf("Error! file offset exceeds beginning of file \n");
		return ERR_GEN;
	}//end if else  */
	fd->offset += num_bytes;	
	return SUCCESS;
} // end kseek();


/**
 * Creates a new file or directory in the file system
 *
 * Creates a new file or directory and all metadata attached to that
 * file or directory and adds it to the open file table
 *
 * NOTE: File/Directory is not persistenly stored at this point!
 *
 * @param
 *
 * char* filepath - holds the path to the directory in which the
 * new file/directory is to be added
 *
 * @param
 *
 * char mode - not used at present, will eventually be used to
 * set file permissions
 *
 * @param
 *
 * int is_this_a_dir - set to 0 if the object to be created is
 * a file; set to 1 if the object to be created is a directory
 *
 * @return
 *
 * Returns 0 if the file/directory was created successfully
 * otherwise returns -1
 */
int kcreate(char* filepath, char mode, int is_this_a_dir) {
	if (filepath == NULL) {
		os_printf("filepath not valid \n");
		return ERR_INVALID;	
	}
	if (mode == 0) {
		os_printf("filepath not valid \n");
		return ERR_INVALID;	
	}
	int fd;
	int inum = 0;
	struct inode* cur_inode = (struct inode*) kmalloc(sizeof(struct inode));
	struct dir_helper* result = (struct dir_helper*) kmalloc(sizeof(struct dir_helper));
	kfind_dir(filepath, result);
	kfind_inode(result->truncated_path, inum, result->dir_levels, cur_inode);
	
	// at this point, the name of the file or dir to be created is “result->last” and it has to be added to cur_inode
	int free_inode_loc = bv_firstFree(inode_bitmap); //Consult the inode_bitmap to find a free space in the inode_table to add the new inode

	if (free_inode_loc < 0) {
		os_printf("Disk has reached max number of files allowed. \n");
		kfree(cur_inode);
		kfree(result->truncated_path);
		kfree(result->last);
		kfree(result);
		return ERR_FULL;
	}
	bv_set(free_inode_loc, inode_bitmap);
	struct inode * new_inode = (struct inode*) kmalloc(sizeof(struct inode)); // Create the new inode
	
	/*	initialize all fields of inode...note, these next 5 fields are for
		all files, whether they are a dir or leaf */
	new_inode->inum = free_inode_loc;
	new_inode->fd_refs = 0; //will be incremented in add to opentable
	new_inode->size = 0; 
	new_inode->usr_id = 0; //or something
	new_inode->indirect_blocks_in_file = 0; 

	/*	initialize the fields for new_indoe that are different for dirs and leaves */
	if(is_this_a_dir){
		new_inode->is_dir = 1; 
		new_inode->direct_blocks_in_file = 1;
		
		// Lay down the first (empty...) data block for the new directory (NOT root).
		
		transmit_receive_bitmap(RECEIVE, data_block_bitmap, FS->data_bitmap_loc, FS->max_data_blocks, 0, 1);
		int new_data_block_loc = bv_firstFree(data_block_bitmap);
		if(new_data_block_loc < 0){//disk is completley full
				os_printf("ERROR! disk full\n");
				return ERR_FULL;
		}//end if
		int block_address = (new_data_block_loc + FS->start_data_blocks_loc) * BLOCKSIZE;
		bv_set(new_data_block_loc, data_block_bitmap);
		
		//WILL THIS FIX IT??????????		
		transmit_receive_bitmap(TRANSMIT, data_block_bitmap, FS->data_bitmap_loc, FS->max_data_blocks, 0, 1);

		void *block = kmalloc(BLOCKSIZE);
		os_memset(block, 0, BLOCKSIZE);
		struct dir_data_block ddb;
		ddb.block_num = new_data_block_loc;
		ddb.num_entries = 0;
		os_memset(block, 0, BLOCKSIZE);
		os_memcpy((uint32_t*)&ddb, block, sizeof(struct dir_data_block));
		sd_transmit((void*)block, block_address);
		new_inode->data_blocks[0] = new_data_block_loc;
		kfree(block);
	}else{ 	/*	initialize the fields for new_inode that are different for dirs and leaves */
		new_inode->is_dir = 0; 
		new_inode->direct_blocks_in_file = 0; 
	}//end if else

	//new_inode->data_blocks[MAX_DATABLOCKS_PER_INODE] = {0}; 

	//new_inode->indirect_blocks[MAX_NUM_INDIRECT_BLOCKS] = {0}; 
	// TODO: Investigate permissions, and bitvectors/FS in general.
	/*switch (mode){
		case 'r':
			bv_set(0, new_inode->perms);
			bv_lower(1, new_inode->perms);
			break;
		case 'w':
			bv_set(0, new_inode->perms);
			bv_set(1, new_inode->perms);
			break;
		default:
			os_printf("Wrong permission. Please insert r for read and w for write\n");
			return -1;
			}*/
	//UPDATE DISK by writing memory data structures to disk

	int error = add_dir_entry(cur_inode, new_inode->inum, result);
	if (error != 0) {
		return error;
	}
	void *block = kmalloc(BLOCKSIZE);
	os_memset(block, 0, BLOCKSIZE);
	os_memcpy((uint32_t*)cur_inode, block, sizeof(struct inode));
	sd_transmit(block, (cur_inode->inum + FS->start_inode_table_loc) * BLOCKSIZE);

	os_memset(block, 0, BLOCKSIZE);
	os_memcpy((uint32_t*)new_inode, block, sizeof(struct inode));
	sd_transmit(block, (FS->start_inode_table_loc + new_inode->inum * INODES_PER_BLOCK)*BLOCKSIZE); //if there are more than 1 inodeperblock need to change
	kfree(block);
	// See above... We can't just do this.
	//sd_transmit((void*)inode_bitmap, FS->inode_bitmap_loc);
	kfree(cur_inode);
	if (!is_this_a_dir) {
		fd = add_to_opentable(new_inode, mode);
		return fd;
	}else { //directories are not added to open table
		os_printf("Directory Successfully added\n");
		kfree(new_inode);
		return SUCCESS;
	}

}//end of kcreate() function

/**
 * Determines whether a directory is empty or not
 *
 * Determines whether a directory is empty, can also be used to
 * determine whether the object is a file instead of a directory as
 * well as if the file is not a valid Course OS FS file
 *
 * @param
 *
 * struct inode* cur_inode - holds the file/directory to check
 *
 * @return
 *
 * Return 1 if it is a directory, return 0 if it not a directory,
 * return an error code if something went wrong
 */
int dir_empty(struct inode* cur_inode){
	if(!cur_inode->is_dir){
		os_printf("This file is not a directory\n");
		return FALSE;//return 0, because it's not a dir
	}//end else
	if((cur_inode->direct_blocks_in_file == 0) && (cur_inode->indirect_blocks_in_file == 0)){
        if(cur_inode->size == 0){
            return TRUE; //return true because the dir is empty
        } else {
            /* TODO: Remove after debugging */
            os_printf("LOGIC ERROR: File size does not sync with file properties");
            return ERR_GEN;//error bc we should never get here
        }
	}
	else {
		os_printf("Your current directory still has files in it\n");
		return FALSE;//return false, bc the dir is not empty
	}
}

/**
 * Helper function for kremove_dir_entry actually deletes file/directory
 *
 * Runs after entry for the file/directory has been removed from the parent
 * directory and physically deletes the file/directory
 *
 * @param
 *
 * struct inode* cur_inode - inode of the file to check
 *
 * @return
 *
 * Returns 0 if the file is OK to remove, otherwise returns an error code
 */
int kdelete_single_helper(struct inode * cur_inode){
    if (cur_inode->is_dir){
        //we know it's a directory
        if(dir_empty(cur_inode)) {
           
            bv_lower(cur_inode->inum, inode_bitmap);
            return SUCCESS;//no error
        }
        else {
            os_printf("This file is a non-empty directory, so cannot be deleted");
            return ERR_GEN;//error
        }
    }
    //we know it is a file
    else{
        int d;
        for(d=0; d<cur_inode->direct_blocks_in_file; d++){
            
            int index = cur_inode->data_blocks[d];
            bv_lower(index, data_block_bitmap);
        }       
        struct indirect_block* cur_inder_block = (struct indirect_block*)kmalloc(sizeof(struct indirect_block));
        int i;       
        for(i=0; i<cur_inode->indirect_blocks_in_file; i++){          
            int z;
            int indy_index_add = (cur_inode->indirect_blocks[i] + FS->start_data_blocks_loc) * BLOCKSIZE;
            sd_receive((void*)cur_inder_block, indy_index_add);            
            for(z=0; z<cur_inder_block->blocks_in_file; z++){
                
                int index = cur_inder_block->data_blocks[z];
                bv_lower(index, data_block_bitmap);
            }          
            bv_lower(cur_inder_block->block_num, data_block_bitmap);
        }      
        kfree(cur_inder_block);
        bv_lower(cur_inode->inum, inode_bitmap);
    }
    return SUCCESS;
} // end kdelete_single_helper();

/**
 * Deletes a single entry in a directory
 *
 * Deletes a file or directory's entry from a specified directory's inode list
 * NOTE: May/May not use recursive delete; this is untested so proceed
 * with caution and test whether a recursive delete is done before
 * using this function for deleting sub-directories.
 *
 * @param
 *
 * struct inode* cur_inode - holds a pointer to the folder which contains the file
 * reference to remove
 *
 * @param
 *
 * int tgt_inum - Used to hold the index of the inode to be removed
 *
 * @return
 *
 * Returns 0 if the entry was successfully removed from the directory; otherwise
 * it returns an error code.
 */
int kremove_dir_entry (struct inode* cur_inode, int tgt_inum) {//free_inode_loc = tgt_inum
	//first get the appropriate data block, either from the array of direct data blocks from an indirect block:
	struct dir_data_block* dir_block = (struct dir_data_block*) kmalloc(BLOCKSIZE);
	struct indirect_block* cur_indirect_block;
	//if the cur_inode's array of direct data blocks has not reached max capacity, grab the last data block in the array to update:
	int i, j, k;
	//search through direct data blocks:
	for(i = 0; i < cur_inode->direct_blocks_in_file; i++){
		sd_receive((void*) dir_block, (cur_inode->data_blocks[i] + FS->start_data_blocks_loc)*BLOCKSIZE);
		for(j = 0; j < dir_block->num_entries; j++){
			struct dir_entry dir_ent = (struct dir_entry) dir_block->dir_entries[j];
			if(dir_ent.inum == tgt_inum){
				/*	remove dir_ent from dir_block, by moving the last dir_entry to this
					index in the array and decrementing dir_block->num_entries */
				dir_block->dir_entries[j] = dir_block->dir_entries[dir_block->num_entries-1];
				dir_block->num_entries--;
				if(dir_block->num_entries == 0){
					/*	then this dir_data_block is empty, so we need to:
							remove it from the cur_inode->data_blocks[] at index i
							remove it from the data_blocks_bitmap */
					bv_lower(cur_inode->data_blocks[i], data_block_bitmap);
					cur_inode->data_blocks[i] = cur_inode->data_blocks[cur_inode->direct_blocks_in_file-1];
					cur_inode->direct_blocks_in_file--;
				}//end if
				cur_inode->size -= sizeof(struct dir_entry);
				sd_transmit((void*) dir_block, (cur_inode->data_blocks[i] + FS->start_data_blocks_loc)*BLOCKSIZE);
				kfree(dir_block);
				return SUCCESS;
			}//end if
		}//end for
	}//end outer for
	//search through indirect data blocks:
	cur_indirect_block = (struct indirect_block*) kmalloc(BLOCKSIZE);
	for(k = 0; k < cur_inode->indirect_blocks_in_file; k++){
		get_indirect_block(cur_inode, k, cur_indirect_block);
		for(i = 0; i < cur_indirect_block->blocks_in_file; i++){
			sd_receive((void*) dir_block, (cur_indirect_block->data_blocks[i] + FS->start_data_blocks_loc)*BLOCKSIZE);
			for(j = 0; j < dir_block->num_entries; j++){
				struct dir_entry dir_ent = (struct dir_entry) dir_block->dir_entries[j];
				if(dir_ent.inum == tgt_inum){
					/*	remove dir_ent from dir_block, by moving the last dir_entry to this
						index in the array and decrementing dir_block->num_entries */
					dir_block->dir_entries[j] = dir_block->dir_entries[dir_block->num_entries-1];
					dir_block->num_entries--;
					if(dir_block->num_entries == 0){
						/*	then this dir_data_block is empty, so we need to:
								remove it from the cur_inode->data_blocks[] at index i
								remove it from the data_blocks_bitmap */
						bv_lower(cur_indirect_block->data_blocks[i], data_block_bitmap);
						cur_indirect_block->data_blocks[i] = cur_indirect_block->data_blocks[cur_inode->direct_blocks_in_file-1];
						cur_indirect_block->blocks_in_file--;
						
						if(cur_indirect_block->blocks_in_file == 0){
							/*	then this indirect_block is empty, so we need to:
									remove it from the cur_inode->indirect_blocks[] at index k
									remove it from the data_blocks_bitmap */
							bv_lower(cur_inode->indirect_blocks[k],data_block_bitmap);
							cur_inode->indirect_blocks[k] = cur_inode->indirect_blocks[cur_inode->indirect_blocks_in_file-1];
							cur_inode->indirect_blocks_in_file--;
						}//end if
					}//end if
					cur_inode->size -= sizeof(struct dir_entry);
					sd_transmit((void*) dir_block, (cur_indirect_block->data_blocks[i] + FS->start_data_blocks_loc)*BLOCKSIZE);
					kfree(dir_block);
					kfree(cur_indirect_block);
					return SUCCESS;	
				}//end if
			}//end for
		}//end outer for
	}//end outer outer for
	kfree(dir_block);
	kfree(cur_indirect_block);
	return ERR_404;
}//end kremove_dir_entry() function


/**
 * Deletes a single file or from a directory
 *
 * call delete_single_helper, deletes the lowest level (ie target) file and
 * updates all bitmaps, but DOES NOT remove the dir_entry in levelup dir
 *
 * @param
 *
 * struct inode* cur_inode - Points to a specific file/directory to remove
 *
 * @param
 *
 * struct inode* level_up_inode - points to the directory in which the specifiied
 * file/directory resides
 *
 * @return
 *
 * Returns 0 if the the file was successfully deleted; returns -1 if the file
 * doesn't exist
 */
int kdelete_single(struct inode* cur_inode, struct inode* level_up_inode) {
    /* call delete_single_helper, deletes the lowest level (ie target) file and
        updates all bitmaps, but DOES NOT remove the dir_entry in levelup dir */
    int error = kdelete_single_helper(cur_inode);
    if(!error){ //if kdeletesinglehelper was successful
    	sd_transmit((void*) cur_inode, (cur_inode->inum + FS->start_inode_table_loc) * BLOCKSIZE);
    	/* delete dir_entry in levelup dir) */
    	error = kremove_dir_entry(level_up_inode, cur_inode->inum);
   		if(error < 0){
			os_printf("ERROR! Could not find dir_entry in level_up_inode...BIG PROBLEM!!!\n");
			kfree(level_up_inode);
			return error;
		}//end inner if
    	sd_transmit((void*) cur_inode, (level_up_inode->inum + FS->start_inode_table_loc) * BLOCKSIZE);
    }else{
		kfree(level_up_inode);
		return error; //return same error
	}
	kfree(level_up_inode);
  	return SUCCESS;
} // end kdelete_single()


/**
 * Recursively deletes an entire directory
 *
 * Deletes a directory and any sub-directories and files contained within
 * freeing all blocks and metadata for each entry and then updates the
 * metadata for the directory which contained it
 *
 * The logic on this should be good, but still need to...
 * TODO: on each sd_recieve() check cache first AND figure out kmalloc
 * situation...will this cause heap overflow??? should we limit the
 * depth of filepaths to avoid this???
 *
 * @param
 *
 * struct inode* cur_inode - Points to a specific file/directory to remove
 *
 * @param
 *
 * struct inode* level_up_inode - points to the directory in which the specifiied
 * file/directory resides
 *
 * @return
 *
 * Returns 0 if the the directory was successfully deleted, otherwise returns
 * an error code
 */
int krec_delete(struct inode * level_up_inode, struct inode * cur_inode){
	//base case
	int error;
	int status = dir_empty(cur_inode);
	if (status == TRUE){ //dir is empty
		error = kdelete_single(cur_inode, level_up_inode);
	//SHOULDN'T FREE IT	kfree(level_up_inode);
	}else if(status < 0){ //WTF?
		return ERR_GEN; 
	}

	if (error) {
		return error;
	}

	//recursive step
	else{ //status == 0, meaning is not empty
		int i;
		int direct_data_block_address;
		struct dir_data_block* dir_block = (struct dir_data_block*) kmalloc(sizeof(struct dir_data_block));
		for(i=0; i< cur_inode->direct_blocks_in_file; i++){
			direct_data_block_address = (cur_inode->data_blocks[i]+FS->start_data_blocks_loc) * BLOCKSIZE;
			sd_receive(dir_block, direct_data_block_address);
			int j;
			int inum;
			struct dir_entry cur_dir_entry;
			for(j = 0; j < dir_block->num_entries; j++){
				cur_dir_entry = dir_block->dir_entries[j];
				inum = cur_dir_entry.inum;
				struct inode* next_inode = (struct inode*) kmalloc(sizeof(struct inode));
				// struct inode* next_inode;
				get_inode(inum, next_inode);
				krec_delete(cur_inode,next_inode);
				kfree(next_inode);
			}//end
		}//end for
		int k;
		int indirect_data_block_address;
		struct indirect_block* cur_indirect_block = (struct indirect_block*) kmalloc(sizeof(struct indirect_block));
		for(k = 0; k < cur_inode->indirect_blocks_in_file; k++){
			indirect_data_block_address = (cur_inode->indirect_blocks[k]+FS->start_data_blocks_loc) * BLOCKSIZE;
			sd_receive(cur_indirect_block, indirect_data_block_address);
			for(i=0; i < cur_indirect_block->blocks_in_file; i++){
				direct_data_block_address = (cur_indirect_block->data_blocks[i]+FS->start_data_blocks_loc) * BLOCKSIZE;
				sd_receive(dir_block, direct_data_block_address);
				int j;
				int inum;
				struct dir_entry cur_dir_entry;
				for(j = 0; j < dir_block->num_entries; j++){
					cur_dir_entry = dir_block->dir_entries[j];
					inum = cur_dir_entry.inum;
					struct inode* next_inode = (struct inode*) kmalloc(sizeof(struct inode));
					// struct inode* next_inode;
					get_inode(inum, next_inode);
					krec_delete(cur_inode,next_inode);
					kfree(next_inode);
				}//end
			}//end
		}//end outer for
		kfree(cur_indirect_block);
		kfree(dir_block);
	}//end if else	
	return SUCCESS; 
}//end krec_delete()

//---------------------------------------------------

/**
 * Deletes a file or directory at the specified filepath
 *
 * Can be used to delete a single file or directory; can delete
 * directories recursively
 *
 * NOTE: Recursive delete is untested; please test this function
 * to ensure it is working properly before using it
 *
 * @param
 *
 * char* filepath - Holds the path to the specified file/directory to delete
 *
 * @param
 *
 * int recursive - Holds 0 if the target is a single file or empty directory
 * holds 1 if the target is a full directory which needs to be deleted recursively
 *
 * @return
 *
 * Returns 0 if the file/directory was deleted successfully; otherwise returns an
 * error code
 */
//delete the file or directory at filepath. Return -1 if the file does not exist 
int kdelete(char* filepath, int recursive) {
    int error;
    int inum = 0;
    
    /* spaceholder for lowest inode */
	struct inode* cur_inode = (struct inode*) kmalloc(sizeof(struct inode));
	struct inode* level_up_inode = (struct inode*) kmalloc(sizeof(struct inode));
    /* spaceholder for helper struct */
    struct dir_helper* result = (struct dir_helper*) kmalloc(sizeof(struct dir_helper));
	
    /* find the helper struct */
    kfind_dir(filepath, result);
    
    /* find the lowest inode */
	error = kfind_inode(filepath, inum, (result->dir_levels + 1), cur_inode);
	if(error < 0){
		kfree(cur_inode);
		kfree(level_up_inode);
		kfree(result->truncated_path);
		kfree(result->last);
		kfree(result);
		return error;
	}
    /* Check to ensure the file is not currently open...if it is return an error and notify user */
	if(inode_is_open(cur_inode)){
		os_printf("File %d cannot be deleted, because it is currently open\n", cur_inode->inum);
		kfree(cur_inode);
		kfree(level_up_inode);
		kfree(result->truncated_path);
		kfree(result->last);
		kfree(result);
		return ERR_GEN;
	}
	error = kfind_inode(filepath, inum, (result->dir_levels), level_up_inode);
	if(recursive){
		error =  krec_delete(level_up_inode,cur_inode);
		kfree(cur_inode);
		kfree(level_up_inode);
		kfree(result->truncated_path);
		kfree(result->last);
		kfree(result);
		transmit_receive_bitmap(TRANSMIT, data_block_bitmap, FS->data_bitmap_loc, FS->max_data_blocks, 0, 1);
		transmit_receive_bitmap(TRANSMIT, inode_bitmap, FS->inode_bitmap_loc, FS->max_inodes, 0, 1);
		return error;
	}else{
		error = kdelete_single(cur_inode, level_up_inode);
		kfree(cur_inode);
		kfree(level_up_inode);
		kfree(result->truncated_path);
		kfree(result->last);
		kfree(result);
		transmit_receive_bitmap(TRANSMIT, data_block_bitmap, FS->data_bitmap_loc, FS->max_data_blocks, 0, 1);
		transmit_receive_bitmap(TRANSMIT, inode_bitmap, FS->inode_bitmap_loc, FS->max_inodes, 0, 1);
		return error;
	}//end if else
	os_printf("we should never reach this!!!\n");
    return ERR_GEN;
} // end kdelete()


//---------------------------------------------------

/**
 * Copies contents of a file into a new file
 *
 * Creates a new file at the destination path and then copies
 * all data from the file pointed to by the source path
 *
 * @param
 *
 * char* source - holds the file path of the file to copy
 *
 * @param
 *
 * char* dest - holds the destination file path to copy to
 *
 * @param
 *
 * char mode - Mainly used for permissions; not implemented right
 * now however
 *
 * @return
 *
 * Retun 0 if the file was copied successfully; otherwise return
 * an error code
 */
int kcopy(char* source, char* dest, char mode) {
	int error = 0;
	int inum = 0; //start from root

	//1. find source
	struct dir_helper *source_dir_helper = (struct dir_helper *) kmalloc(sizeof(struct dir_helper));
	kfind_dir(source, source_dir_helper); 
	struct inode *source_inode = (struct inode*) kmalloc(sizeof(struct inode));
	//find the source inode
	error = kfind_inode(source, inum, (source_dir_helper->dir_levels + 1), source_inode);
	if (error < 0) {  //kfind_inode unsuccessful 
		os_printf("kfind_inode unsuccessful \n");
		kfree(source_inode);
		kfree(source_dir_helper->truncated_path);
		kfree(source_dir_helper->last);
		kfree(source_dir_helper);
		return ERR_404;
	}
	//at this point source_inode is the inode of the source
	int copy_directory = source_inode->is_dir; //checks if we are copying a direcory or a file

	//2. cerate destination
	int dest_fd = 0;
	dest_fd = kcreate(dest, mode, copy_directory); //creates the new file or directory
	if (dest_fd < 0) { //some problem occurred in kcreate
		os_printf("kcreate unsuccessful \n");
		kfree(source_inode);
		kfree(source_dir_helper->truncated_path);
		kfree(source_dir_helper->last);
		kfree(source_dir_helper);
		return ERR_GEN;
	}

	//3. find destination
	struct file_descriptor *dest_fd_struct = get_descriptor(dest_fd);
	if (dest_fd_struct == NULL) {  //get_descriptor had problems
		os_printf("get_descriptor unsuccessful \n");
		kfree(source_inode);
		kfree(source_dir_helper->truncated_path);
		kfree(source_dir_helper->last);
		kfree(source_dir_helper);
		return ERR_GEN;
	}
	//at this point dest_inode is the inode of the created destination
	
	void *buffer = (void*) kmalloc(source_inode->size);
	error = read_inode(source_inode, 0, buffer, source_inode->size);
	if (!error) {
		error = kwrite(dest_fd, buffer, source_inode->size);
	}
	kfree(buffer);
	kfree(source_inode);
	kfree(source_dir_helper->truncated_path);
	kfree(source_dir_helper->last);
	kfree(source_dir_helper);
	return error;
}//end kcopy function


/**
 * Prints the contents of a directory to the screen
 *
 * @param
 *
 * char* filepath - holds the filepath to the directory to display
 * the contents of
 *
 * @return
 * Returns 0 if the function was executed correctly, otherwise returns
 * an error code
 */
int kls(char* filepath) {
	int error = 0;
	int inum = 0; //starting from root
	struct dir_helper* result = (struct dir_helper *) kmalloc(sizeof(struct dir_helper));
	kfind_dir(filepath, result); 
	struct inode* cur_inode = (struct inode*) kmalloc(sizeof(struct inode));
	error = kfind_inode(filepath, inum, (result->dir_levels + 1), cur_inode);
	if (error < 0 || cur_inode->is_dir == 0) {  //kfind_inode unsuccessful or cannot ls
		if (error < 0) { //kfind
			os_printf("kfind_inode unsuccessful \n"); 
		}
		else { //cannot ls
			os_printf("this is not a directory but a file, cannot ls a file \n"); 
			error = ERR_INVALID;
		}
		kfree(cur_inode);
		kfree(result->truncated_path);
		kfree(result->last);
		kfree(result);
		return error; //return same error
	}
	//at this point, cur_inode is a directory and we need to print all the names of its contents. 
	//1. print from direct blocks
	int i;
	void* dir_spaceholder = (void*) kmalloc(BLOCKSIZE);
	for(i = 0; i < cur_inode->direct_blocks_in_file; i++){
		sd_receive(dir_spaceholder, (cur_inode->data_blocks[i])*BLOCKSIZE);
		struct dir_data_block cur_data_block = *((struct dir_data_block*) dir_spaceholder);
		int j;
		for(j = 0; j < (cur_data_block.num_entries); j++){
			struct dir_entry file_dir = cur_data_block.dir_entries[j];  
			os_printf("entry: %s \n", file_dir.name);
		}//inner for
	}//outer for
	//2. print from indirect blocks
	struct indirect_block *cur_indirect_block = (struct indirect_block *) kmalloc(sizeof(struct indirect_block));
	int cur_indirect_block_num;
	for(i = 0; i < cur_inode->indirect_blocks_in_file; i++){
		cur_indirect_block_num = cur_inode->indirect_blocks[i];
		get_indirect_block(cur_inode, cur_indirect_block_num, cur_indirect_block);
		int j;
		for(j = 0; j < cur_indirect_block->blocks_in_file; j++){
			sd_receive(dir_spaceholder, (cur_indirect_block->data_blocks[j])*BLOCKSIZE);
			struct dir_data_block cur_data_block = *((struct dir_data_block*) dir_spaceholder);
			int k;
			for(k = 0; k < (cur_data_block.num_entries); k++){
				struct dir_entry file_dir = cur_data_block.dir_entries[k];  
				os_printf("entry: %s \n", file_dir.name);
			}//inner for
		}//outer for
	}//end for
	kfree(cur_indirect_block);
	kfree(dir_spaceholder);
	kfree(cur_inode);
	kfree(result->truncated_path);
	kfree(result->last);
	kfree(result);
	return SUCCESS;
}

/**
 * Used to obtain information about a specified file or directory
 *
 * Obtains various information about the file at the specified file
 * path including: size of the file, how many open copies of the
 * file there are, and whether the file is a directory
 *
 * @param
 *
 * char* filepath - The path to the file to obtain information from
 *
 * @param
 *
 * struct stats* result - Pointer to a location to store information
 * about the specified file
 *
 * @return
 * Returns 0 if information was successfully retrieved; otherwise
 * returns an error code
 */
int get_stats(char * filepath, struct stats * result) {
	int inum = 0;
	struct inode* cur_inode = (struct inode*) kmalloc(sizeof(struct inode));
	struct dir_helper* help_result = (struct dir_helper*) kmalloc(sizeof(struct dir_helper));
	kfind_dir(filepath, help_result);
	int error = kfind_inode(filepath, inum, (help_result->dir_levels + 1), cur_inode);
	if (error < 0) {
		os_printf("file not found, exiting kopen\n");
		kfree(cur_inode);
		kfree(help_result->truncated_path);
		kfree(help_result->last);
		kfree(help_result);
		return error; //return same error
	}
	result->size = cur_inode->size;
	result->fd_refs = cur_inode->fd_refs;
	result->is_dir = cur_inode->is_dir;
	kfree(cur_inode);
	kfree(help_result->truncated_path);
	kfree(help_result->last);
	kfree(help_result);
	return SUCCESS;
}//end get_stats function
