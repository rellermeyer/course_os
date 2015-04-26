
#include <stdint.h>
#include "../../include/linked_list.h"
#include "../../include/bitvector.h"
#include "../../include/open_table.h"
#include "../../include/mmci.h" 

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
int INDIRECT_BLOCK_TABLE_CACHE_SIZE;

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
bit_vector* inode_bitmap;
bit_vector* data_block_bitmap;

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
	inode_bitmap = (bit_vector*) kmalloc(BLOCKSIZE); 
	sd_receive((void*) inode_bitmap, (FS->inode_bitmap_loc) * BLOCKSIZE); // have a pointer 
	// inode_bitmap = inode_bitmap_temp; // pointer to a bitvector representing iNodes

	data_block_bitmap = (bit_vector*) kmalloc(BLOCKSIZE);
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

	fs_table_init(); //initializes open_table stuff
	return 0;
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

	fs_table_shutdown(); //frees open_table stuff
	return 0;
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
void get_inode(int inum, struct inode* result_inode){
	if(inode_table_cache[inum] != NULL){
		// the inode is in the inode_cache_table, so get it:
		result_inode = (inode_table_cache[inum]);	
	}else{ 
		// inode is not in the cache table, so get it from disk:
		void* inode_spaceholder = (void*) kmalloc(BLOCKSIZE);
		sd_receive(inode_spaceholder, ((inum/INODES_PER_BLOCK)+FS->start_inode_table_loc)*BLOCKSIZE); // the firs
		struct inode *block_of_inodes = (struct inode*) inode_spaceholder;
		*result_inode = block_of_inodes[inum % INODES_PER_BLOCK];
		kfree(inode_spaceholder);
		// need to implement an eviction policy/function to update the inode_table_cache...
		// this will function w/o it, but should be implemented for optimization
	}//end if else
}//end get_inode() helper function

//gets the inum of nextpath (file or dir) looking at the direct data blocks of cur_inode
int get_inum_from_direct_data_block(struct inode* cur_inode, char * next_path){
	int inum = -1;
	int i;
	int file_found = 0; // initialize to false (i.e. file not found)
	void* dir_spaceholder = (void*) kmalloc(BLOCKSIZE);
	for(i = 0; i < cur_inode->direct_blocks_in_file; i++){
		sd_receive(dir_spaceholder, (cur_inode->data_blocks[i])*BLOCKSIZE);
		struct dir_data_block cur_data_block = *(struct dir_data_block*) dir_spaceholder;
		int j;
		for(j = 0; j < (cur_data_block.num_entries); j++){
			struct dir_entry file_dir = cur_data_block.dir_entries[j];  
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
	kfree(dir_spaceholder);
	return inum;
}//end get_inum_from_direct_data_block() helper helper function

//gets the inum of netxpath (file or dir) looking at the indirect data blocks of cur_inode
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

//finds the inode (will be result_inode) following filepath, going dir_levels down the path, starting from starting_inum
int kfind_inode(char* filepath, int starting_inum, int dir_levels, struct inode* result_inode) { //filepath and starting inum must correspond...
	int current_inum = starting_inum;
	int a;
	for(a = 0; a < dir_levels; a++) {
		int k = 1;
		char next_path[MAX_NAME_LENGTH] = {0};

		//get path of next inode
		while ((filepath[k] != '/') && (k <= MAX_NAME_LENGTH) && (filepath[k] != '\0')) {
			next_path[k] = filepath[k];
			k++;
		}//end of litte while to find next_path

		filepath += k;

		// Store inode with current_inum current_inum in result_inode 
		get_inode(current_inum, result_inode);

		//Set new current_inum to the next_path's current_inum
		current_inum = get_inum_from_direct_data_block(result_inode, next_path);

		if(current_inum == -1){
			//current_inum not found in any direct blocks of result_inode
			//look for it in the indirect blocks now
			current_inum = get_inum_from_indirect_data_block(result_inode, next_path);
		}

		if(current_inum == -1){
			//next_path not found in current_inode
			os_printf("404 ERROR! File not found.\nPlease ensure full filepath is specified starting from root (/)\n");
			return -1;
		}
	}//outer most for loop
	//current_inum of target inode found, store that inode in result_inode
	get_inode(current_inum, result_inode);
	return 0;
}//end kfind_inode() helper function

//finds the name of the directory path (result->truncated_path) and the name of the ending part (result->last) and the number of levels (result->levels)
//result has to be kmalloc-ed by and kfree-d by whoever calls this functinos. Also remember to free last and truncated_path. 
void kfind_dir(char* filepath, struct dir_helper* result){
	int dir_levels = 0;
	int total_chars = 0;
	char* iterator = filepath;
	int index = 0;
	while(index < MAX_NAME_LENGTH){
		if(iterator[0] == '\0'){
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

int transmit_data_block_bitmap(){
	int error = 0;
	//TODO: implement this
	return error;
}//end transmit_data_block_bitmap() helper function


/* 	Helper function to add a new dir_entry to a directory file and optinally write it out to disk.
	Updates the last data block of the cur_inode (directory) to add a dir_entry that stores the mapping of the new inode to its inum */
int add_dir_entry(struct inode* cur_inode, int free_inode_loc, struct dir_helper* result){

	int flag_free_cur_indirect_block = 0;

	//first get the appropriate data block, either from the array of direct data blocks from an indirect block:
	struct dir_data_block* dir_block = (struct dir_data_block*) kmalloc(BLOCKSIZE);
	struct indirect_block* cur_indirect_block;
	//if the cur_inode's array of direct data blocks has not reached max capacity, grab the last data block in the array to update:
	if((cur_inode->direct_blocks_in_file <= MAX_DATABLOCKS_PER_INODE)  && (cur_inode->indirect_blocks_in_file == 0)) {
		sd_receive((void*) dir_block, (cur_inode->data_blocks[(cur_inode->direct_blocks_in_file)-1])*BLOCKSIZE);
	}else{
		//all the direct data blocks are full, so grab the last indirect block in the array of indirect blocks:
		cur_indirect_block = (struct indirect_block*) kmalloc(BLOCKSIZE);
		get_indirect_block((cur_inode->indirect_blocks_in_file - 1), cur_indirect_block);
		sd_receive((void*) dir_block, (cur_indirect_block->data_blocks[(cur_indirect_block->blocks_in_file)-1])*BLOCKSIZE);
		flag_free_cur_indirect_block = 1;
		// kfree(cur_indirect_block);
	}
	/* 	dir_block is now a direct data block that holds the content of the last data block in the file
		either from cur_inode's array of direct data blocks or from its last indirect data block */
	
	//create the new dir_entry and populate it's fields:
	struct dir_entry new_dir_entry;
	new_dir_entry.inum = free_inode_loc;
	new_dir_entry.name = result->last;
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
			transmit_data_block_bitmap();//TODO: create this helper function
		
		}else{
			//get the current indirect block and check to see if it has room to add another data block:
			if(!flag_free_cur_indirect_block){
				//if we don't have the indirect block, we need to get it from disk:
				cur_indirect_block = (struct indirect_block*) kmalloc(BLOCKSIZE);
				flag_free_cur_indirect_block = 1;
				get_indirect_block((cur_inode->indirect_blocks_in_file - 1), cur_indirect_block);
				sd_receive((void*) dir_block, (cur_indirect_block->data_blocks[(cur_indirect_block->blocks_in_file)-1])*BLOCKSIZE);
				// kfree(cur_indirect_block);
			}
			if(cur_indirect_block->blocks_in_file < MAX_DATABLOCKS_PER_INDIRECT_BLOCK){
				//Case (2): add a new data block to to the current indirect block, then add the new_dir entry to the new data block:
				cur_indirect_block->data_blocks[cur_indirect_block->blocks_in_file] = new_dir_block->block_num;
				cur_indirect_block->blocks_in_file++;

				new_dir_block->dir_entries[new_dir_block->num_entries] = new_dir_entry;
				new_dir_block->num_entries++;
				cur_inode->size += sizeof(struct dir_entry);

				if(indirect_block_table_cache[cur_indirect_block->block_num] == NULL){
					//TODO: implement eviction policy...add the cur_inode to the cache:
				}else{
					*(indirect_block_table_cache[cur_indirect_block->block_num]) = *(cur_indirect_block);
				}
				sd_transmit((void*) cur_indirect_block, (cur_indirect_block->block_num + FS->start_indirect_block_table_loc) * BLOCKSIZE);
				sd_transmit((void*) new_dir_block, (new_dir_block->block_num + FS->start_data_blocks_loc) * BLOCKSIZE);
				transmit_data_block_bitmap();//TODO: create this helper function

			}else{
				if(cur_inode->indirect_blocks_in_file < MAX_NUM_INDIRECT_BLOCKS){
					/*	Case (3): add a new indirect block to the cur_inode, then add a new data block to the new indirect block, 
						then add the new dir_entry to the new data block: */
					int new_indirect_block_loc = bv_firstFree(data_block_bitmap); //Consult the data_block_bitmap to find a free block to add the new data block at
					struct indirect_block* new_indirect_block = (struct indirect_block*) kmalloc(BLOCKSIZE);

					new_indirect_block->blocks_in_file = 0;
					new_indirect_block->block_num = new_indirect_block_loc;

					new_indirect_block->data_blocks[new_indirect_block->blocks_in_file] = new_dir_block->block_num;
					new_indirect_block->blocks_in_file++;

					new_dir_block->dir_entries[new_dir_block->num_entries] = new_dir_entry;
					new_dir_block->num_entries++;
					cur_inode->size += sizeof(struct dir_entry);

					if(indirect_block_table_cache[new_indirect_block->block_num] == NULL){
						//TODO: implement eviction policy...add the cur_inode to the cache:
					}else{
						*(indirect_block_table_cache[new_indirect_block->block_num]) = *(new_indirect_block);
					}

					sd_transmit((void*) new_indirect_block, (new_indirect_block->block_num + FS->start_indirect_block_table_loc) * BLOCKSIZE);
					sd_transmit((void*) new_dir_block, (new_dir_block->block_num + FS->start_data_blocks_loc) * BLOCKSIZE);
					transmit_data_block_bitmap();//TODO: create this helper function
					kfree(new_indirect_block);

				}else{
					//file has reached max allowable size:
					os_printf("ERROR! Operation failed because file has reached max allowable size\n");
					return -1;
				}
			}
		}
		kfree(new_dir_block);
	}
	kfree(dir_block);
	if(flag_free_cur_indirect_block){
		kfree(cur_indirect_block);
	}
	return 0;
}//end add_dir_entry() helper function

int get_block_address(struct inode *file_inode, int block_num){
	if(block_num < 0){
		os_printf("Invalid block number");
		return -1;
	}
	if(block_num < MAX_DATABLOCKS_PER_INODE){
		return file_inode->data_blocks[block_num];
	}
	//Block must be stored in an indirect block of file_inode

	//Get the indirect block num that contain the target block
	int indirect_block_num = (block_num - MAX_DATABLOCKS_PER_INODE) / MAX_DATABLOCKS_PER_INDIRECT_BLOCK;
	if(indirect_block_num < file_inode->indirect_blocks_in_file || indirect_block_num > (MAX_NUM_INDIRECT_BLOCKS - 1)){
		os_printf("block_num out of range");
		return -1;
	}

	struct indirect_block *current_indirect_block = (struct indirect_block *) &(file_inode->indirect_blocks[indirect_block_num]);
	int indirect_block_direct_block_num = (block_num - MAX_DATABLOCKS_PER_INODE) % MAX_DATABLOCKS_PER_INDIRECT_BLOCK;
	return current_indirect_block->data_blocks[indirect_block_direct_block_num];
}


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
	if (cur_inode->is_dir) {
		os_printf("cannot open a directory, make the path end to a file");
		kfree(cur_inode);
		kfree(result->truncated_path);
		kfree(result->last);
		kfree(result);
		return -1;
	}

	bit_vector *p = cur_inode->perms;
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
	}
	fd = add_to_opentable(cur_inode, mode);
	kfree(result->truncated_path);
	kfree(result->last);
	kfree(result);
	return fd;
}//end kopen()


// Helper function for kread():
int read_partial_block(struct inode *c_inode, int offset, void* buf_offset, int bytes_left, void* transfer_space) {
	int local_offset = offset % BLOCKSIZE; // local_offset is the leftmost point in the block

	// Actually get the data for 1 block (the SD Driver will put it in transferSpace for us)
	int block_num = offset / BLOCKSIZE;
	int block_address = get_block_address(c_inode, block_num);
	int success = sd_receive(transfer_space, block_address);
	if(success < 0){
	 	// failed on a block receive, therefore the whole kread fails; return failure error
	 	// os_printf("failed to receive block number %d\n", numBytes);
	 	return -1;
	}//end if

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
		 // reset transferSpace pointer
		 //transfer_space -= bytes_left; //Purpose of this?
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
	//this should never happen...print for debugging. TODO: remove after debugged
	os_printf("Error! In f1() in kread()...this should never happend!");
	return 0;

}//end of read_partial_block() helper function


// Helper function for kread():
int read_full_block(struct inode *c_inode, int offset, void* buf_offset, int bytesLeft, void* transfer_space) {
	// read BLOCKSIZE
	// Actually get the data for 1 block (the SD Driver will put it in transfer_space for us)
	int block_num = offset / BLOCKSIZE;
	int block_address = get_block_address(c_inode, block_num);
	int success = sd_receive(transfer_space, block_address);
	if(success < 0){
	 	// failed on a block receive, therefore the whole kread fails; return failure error
	 	os_printf("failed to receive block number %d\n", block_num);
	 	return -1;
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
	 //transfer_space -= BLOCKSIZE; //Purpose of this?
	 return BLOCKSIZE; // note, we are returning the number of bytes that were successfully transferred
}//end read_full_block() helper function

int read_inode(struct inode *c_inode, int offset, void* buf, int num_bytes){
	// Allocate space for and create a bitvector to be used repeatedly to transfer the data:
	uint32_t *transfer_space = kmalloc(BLOCKSIZE);
	int bytes_read = 0;

	// start of higher-level algo:
	if(num_bytes < BLOCKSIZE) {
		while(bytes_read < num_bytes) {
			bytes_read += read_partial_block(c_inode, offset + bytes_read, buf, (num_bytes-bytes_read),transfer_space);
		}
	} else if(num_bytes >= BLOCKSIZE) {
		//Read in remainder of current block
		bytes_read += read_partial_block(c_inode, offset + bytes_read, buf, (num_bytes-bytes_read),transfer_space);
		//Read in maximum number of full blocks
		while((num_bytes - bytes_read) > BLOCKSIZE) {
			bytes_read += read_full_block(c_inode, offset + bytes_read, buf, (num_bytes-bytes_read),transfer_space);
		}
		//If not completely read yet, read in remainder
		if(bytes_read < num_bytes) {
			bytes_read += read_partial_block(c_inode, offset + bytes_read, buf, (num_bytes-bytes_read),transfer_space);
		}
	}//end else if
	return bytes_read;
}

/* read from fd, put it in buf, then return the number of bytes read in numBytes */
int kread(int fd_int, void* buf, int num_bytes) {
	int bytes_read = 0;
	uint32_t* buf_offset = buf; //this allows us to move data incrementally to user's buf via buf_offset
	//while retaining the original pointer to return back to the user
	struct file_descriptor* fd = get_descriptor(fd_int); // note, get_file_descriptor() function has not yet been implemented...will be in open_table.c

	if ((fd->permission != 'r' || fd->permission != 'b')) {
		os_printf("no permission\n");
		return -1;
	}

	bytes_read = read_inode(fd->linked_file, fd->offset, buf_offset, num_bytes);
	fd->offset += bytes_read;
	if(bytes_read != num_bytes){
		return bytes_read;
	}else{
		return -1;
	}


} // end kread();



/* write from fd, put it in buf, then return the number of bytes written in numBytes */
int kwrite(int fd_int, void* buf, int num_bytes) {
    struct file_descriptor* fd = get_descriptor(fd_int);

    if ((fd->permission != 'w' || fd->permission != 'b')){
        os_printf("no permission\n");
        return -1;
    }

    int total_bytes_left = num_bytes;
    int bytes_written = 0;

    uint32_t *buf_offset = buf;
    uint32_t *transfer_space = kmalloc(BLOCKSIZE);
    // os_memcpy(transferSpace, buf_offset, (os_size_t) BLOCKSIZE); 

    // have offset in the file already, just need to move it and copy.
    // fd->offset is the offset in the file. 
    while(bytes_written < total_bytes_left) {
		int block_num = fd->offset / BLOCKSIZE;
		// need to put things in transfer_space, move pointer back when done
		int offset_into_current_block = fd->offset % BLOCKSIZE;
		int bytes_left_in_block = BLOCKSIZE - offset_into_current_block;
		int block_address = get_block_address(fd->linked_file, block_num);

		if(total_bytes_left <= bytes_left_in_block){
			/*	--------------- 			-----------------				
				|~~~~~~~|      |	 OR		|     |~~~~~|   |
				----------------			-----------------
			*/ 
			//Remaining bytes to write will fit in the current block
			// write total_bytes_left
			os_memcpy(buf_offset, transfer_space, (os_size_t) total_bytes_left);
			//transfer_space -= total_bytes_left; //Purpose of this?
			// pointer to start, block_num, where we are in file, length of write
			sd_transmit(transfer_space, block_address + offset_into_current_block);

			bytes_written += total_bytes_left;
			total_bytes_left -= total_bytes_left;
			fd->offset += total_bytes_left;
		}
		else if(total_bytes_left > bytes_left_in_block) {
			/* 
				------------
				|	    |~~~|
				------------
				write to the end of the block
			*/
			//Remaining bytes will not fit in current block, fill up remainder of block
			os_memcpy(buf_offset, transfer_space, (os_size_t) bytes_left_in_block);
			//transfer_space -= bytes_left_in_block; //Purpose of this?
			// pointer to start, blockNum, where we are in file, lengh of write
			sd_transmit(transfer_space, block_address + offset_into_current_block);

			bytes_written += bytes_left_in_block;
			total_bytes_left -= bytes_left_in_block;
			fd->offset += bytes_left_in_block;
		} else {
			// Is it possible to even reach here?
			os_memcpy(buf_offset, transfer_space, (os_size_t) BLOCKSIZE);
			transfer_space -= BLOCKSIZE;
			// pointer to start, blockNum, where we are in file, lengh of write
			sd_transmit(transfer_space, block_num);

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
	if(!file_is_open(fd)) { 
		os_printf("file not open"); return -1; 
	}
	error = delete_from_opentable(fd); //this also frees inode
	return error;
} // end kclose();


// seek within the file, return an error if you are outside the boundaries 
int kseek(int fd_int, int num_bytes) {
        struct file_descriptor* fd = get_descriptor(fd_int);
        if (fd->permission != 'r' || fd->permission != 'w') {
            os_printf("no permission \n");
            return -1;
        } else if ((num_bytes > 0) && ((fd->offset + num_bytes) > ((fd->linked_file)->size))){
        	os_printf("Error! file offset exceeds file size \n");
			return -1;
        } else if ((num_bytes < 0) && ((fd->offset + num_bytes) < 0)){
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
	int free_inode_loc = bv_firstFree(inode_bitmap); //Consult the inode_bitmap to find a free space in the inode_table to add the new inode
	if (free_inode_loc == -1) {
		os_printf("Disk has reached max number of files allowed. \n");
		kfree(cur_inode);
		kfree(result->truncated_path);
		kfree(result->last);
		kfree(result);
		return -1;
	}
	bv_set(free_inode_loc, inode_bitmap);
	struct inode * new_inode = (struct inode*) kmalloc(sizeof(struct inode)); // Create the new inode
	//initialize all fields of inode:
	new_inode->inum = free_inode_loc;
	new_inode->fd_refs = 0; //will be incremented in add to opentable
	new_inode->size = 0; 
	new_inode->is_dir = is_this_a_dir; 
	new_inode->usr_id = 0; //or something
	new_inode->direct_blocks_in_file = 0; 
	//new_inode->data_blocks[MAX_DATABLOCKS_PER_INODE] = {0}; 
	new_inode->indirect_blocks_in_file = 0; 
	//new_inode->indirect_blocks[MAX_NUM_INDIRECT_BLOCKS] = {0}; 
	switch (mode){
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
	}
	//UPDATE DISK by writing memory data structures to disk
	int error = add_dir_entry(cur_inode, free_inode_loc, result);
	if(inode_table_cache[cur_inode->inum] == NULL){
			//TODO: implement eviction policy...add the cur_inode to the cache:
		}else{
			*(inode_table_cache[cur_inode->inum]) = *(cur_inode);
	}
	sd_transmit((void*) cur_inode, (cur_inode->inum + FS->start_inode_table_loc) * BLOCKSIZE);
	sd_transmit((void*)new_inode, (FS->start_inode_table_loc + new_inode->inum * INODES_PER_BLOCK)*BLOCKSIZE); //if there are more than 1 inodeperblock need to change
	sd_transmit((void*)inode_bitmap, FS->inode_bitmap_loc);

	kfree(cur_inode);
	if (!is_this_a_dir) {
		fd = add_to_opentable(new_inode, mode);
		return fd;
	}
	else { //directories are not added to open table
		return error;
	}
}//end of kcreate() function


//delete the file or directory at filepath. Return -1 if the file does not exist 
int kdelete(char* filepath) {
	int inum = 0;
	struct inode* cur_inode = (struct inode*) kmalloc(sizeof(struct inode));
	struct dir_helper* result = (struct dir_helper*) kmalloc(sizeof(struct dir_helper));
	kfind_dir(filepath, result);
	int error = kfind_inode(filepath, inum, (result->dir_levels + 1), cur_inode);
	if (error == -1) {
		os_printf("something wrong in kfind_inode \n");
		kfree(cur_inode);
		kfree(result->truncated_path);
		kfree(result->last);
		kfree(result);
		return -1;
	}
	//here we have the file we were looking for! it is cur_inode.

    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    // REAL DELETING HAS TO BE MADE                                                                                                                                    
    // the inode has to be deleted from its directory.                                                                         
    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------  
	
	kfree(cur_inode->data_blocks);
	kfree(cur_inode->indirect_blocks);
	bv_free(cur_inode->perms);
	kfree(cur_inode);
	kfree(result->truncated_path);
	kfree(result->last);
	kfree(result);
	return 0;
} // end kdelete();


//copies contents of file
int kcopy(char* source, char* dest, char mode) {
	int error = 0;
	int inum = 0; //start from root

	//1. find source
	struct dir_helper *source_dir_helper = (struct dir_helper *) kmalloc(sizeof(struct dir_helper));
	kfind_dir(source, source_dir_helper); 
	struct inode *source_inode = (struct inode*) kmalloc(sizeof(struct inode));
	//find the source inode
	error = kfind_inode(source, inum, (source_dir_helper->dir_levels + 1), source_inode);
	if (error == -1) {  //kfind_inode unsuccessful 
		os_printf("kfind_inode unsuccessful \n");
		kfree(source_inode);
		kfree(source_dir_helper->truncated_path);
		kfree(source_dir_helper->last);
		kfree(source_dir_helper);
		return -1;
	}
	//at this point source_inode is the inode of the source
	int copy_directory = source_inode->is_dir; //checks if we are copying a direcory or a file

	//2. cerate destination
	int dest_fd = 0;
	dest_fd = kcreate(dest, mode, copy_directory); //creates the new file or directory
	if (dest_fd == -1) { //some problem occurred in kcreate
		os_printf("kcreate unsuccessful \n");
		kfree(source_inode);
		kfree(source_dir_helper->truncated_path);
		kfree(source_dir_helper->last);
		kfree(source_dir_helper);
		return -1;
	}

	//3. find destination
	struct file_descriptor *dest_fd_struct = get_descriptor(dest_fd);
	if (dest_fd_struct == 0x0) {  //get_descriptor had problems
		os_printf("get_descriptor unsuccessful \n");
		kfree(source_inode);
		kfree(source_dir_helper->truncated_path);
		kfree(source_dir_helper->last);
		kfree(source_dir_helper);
		return -1;
	}
	/* commenting line 925 but will be needed, just want to compile rn */
	//struct inode *dest_inode = dest_fd_struct->linked_file;
	//at this point dest_inode is the inode of the created destination

	//4. copy contents from source_inode to dest_inode (all contents)
	//-------------------------------------------------------------------------------------------------------------------------------------------------------
	// NEED HELPER FUNCTIONS (THAT WILL BE USED ALSO IN READ AND WRITE) THAT READ/WRITE ME STUFF FROM A GIVEN INODE
	// so the whole copying can actually happen!
	//-------------------------------------------------------------------------------------------------------------------------------------------------------

	void *buffer = (void*) kmalloc(source_inode->size);
	read_inode(source_inode, 0, buffer, source_inode->size);
	kwrite(dest_fd, buffer, source_inode->size);

	kfree(buffer);
	kfree(source_inode);
	kfree(source_dir_helper->truncated_path);
	kfree(source_dir_helper->last);
	kfree(source_dir_helper);
	return -1;
}


//prints the contents of a directory
int kls(char* filepath) {
	int error = 0;
	int inum = 0; //starting from root
	struct dir_helper* result = (struct dir_helper *) kmalloc(sizeof(struct dir_helper));
	kfind_dir(filepath, result); 
	struct inode* cur_inode = (struct inode*) kmalloc(sizeof(struct inode));
	error = kfind_inode(filepath, inum, (result->dir_levels + 1), cur_inode);
	if (error == -1 || cur_inode->is_dir == 0) {  //kfind_inode unsuccessful or cannot ls
		if (error == -1) { //kfind
			os_printf("kfind_inode unsuccessful \n"); 
		}
		else { //cannot ls
			os_printf("this is not a directory but a file, cannot ls a file \n"); 
		}
		kfree(cur_inode);
		kfree(result->truncated_path);
		kfree(result->last);
		kfree(result);
		return -1;
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
		get_indirect_block(cur_indirect_block_num, cur_indirect_block);
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
	return 0;
}