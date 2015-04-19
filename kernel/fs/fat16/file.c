#include <stdint>
#include "../../include/linked_list.h"
#include "../../include/bitvector.h"
#include "../../include/open_table.h"
#include "../../include/mmci.h" 
#include "../../include/klibc.h"

//CONSTANTS:
const int SUPERBLOCK = 1;
// const int MAX_NAME_LENGTH = 32; moved this to a define
const int MAX_BLOCKS;
// const int BLOCKSIZE = 512; moved this to a define...should be dynamic though...how?
// const int INODE_SIZE = 128; shouldn't need this...should be able to do sizeof(Inode)
const int MAX_MEMORY;
const int INODE_TABLE_CACHE_SIZE;
const int NUM_INODE_TABLE_BLOCKS_TO_CACHE;
const int INODES_PER_BLOCK;

// FAKE CONSTANTS:
const int sd_card_capacity = 512000;
const int root_inum = 0;
const int max_inodes = 200; // will eventually be in superblock create
const int inode_size = 128;
const int max_data_blocks = 796;
const int inode_bitmap_loc = 2 * BLOCKSIZE;
const int data_bitmap_loc = 3 * BLOCKSIZE;
const int start_inode_table_loc = 4;
const int start_data_blocks_loc = 204;


struct *superblock FS;
bitvector* inode_bitmap;
bitvector* data_block_bitmap;
inode** inode_table_cache; //is this right? we want an array of inode pointers...
void* data_table;

// initialize the filesystem:
int kfs_init(int max_memory){
	MAX_MEMORY = max_memory;
	INODE_TABLE_CACHE_SIZE = (int) MAX_MEMORY/20; //does this seem reasonable? using 1/20 of total system memory for caching indode table?
	NUM_INODE_TABLE_BLOCKS_TO_CACHE = ((int) INODE_TABLE_CACHE_SIZE/BLOCKSIZE) + 1;

	//initialize the SD Card driver:
	init_sd();
	//read in the super block from disk and store in memory:
	void* superblock_spaceholder = kmalloc(BLOCKSIZE);
	receive(superblock_spaceholder, (SUPERBLOCK*BLOCKSIZE)); // make all blocks addresses, like here

	FS = (struct superblock*) superblock_spaceholder; // super block is smaller than block size
	INODES_PER_BLOCK = (int)(FS->block_size/FS->inode_size);

	//initialize the free list by grabbing it from the SD Card:
	void* inode_bitmap_temp = kmalloc(BLOCKSIZE); 
	receive(inode_bitmap_temp, (FS->inode_bitmap_loc) * BLOCKSIZE); // have a pointer 
	inode_bitmap = inode_bitmap_temp; // pointer to a bitvector representing iNodes

	void* data_block_bitmap_temp = kmalloc(BLOCKSIZE);
	receive(data_block_bitmap_temp, (FS->data_bitmap_loc) * BLOCKSIZE);
	data_block_bitmap = data_block_bitmap_temp; // pointer to bitvector representing free data blocks

	
	//NEED TO LOOK THROUGH THIS AGAIN:
	void* inode_table_temp = kmalloc(NUM_INODE_TABLE_BLOCKS_TO_CACHE * BLOCKSIZE);
	struct inode** inode_table_cache = kmalloc((sizeof(struct inode*))* FS->max_inodes);
	int i;
	for(i = 0; i < FS->max_inodes; i++){
		if(i < NUM_INODE_TABLE_BLOCKS_TO_CACHE * INODES_PER_BLOCK){
			if(i % INODES_PER_BLOCK == 0){
				receive((inode_table_temp + ((i/INODES_PER_BLOCK)*BLOCKSIZE)), ((FS->start_inode_table_loc) + (i/INODES_PER_BLOCK)) * BLOCKSIZE);
			}
			inode_table_cache[i] = &((inode_table_temp + (((int)(i/INODES_PER_BLOCK))*BLOCKSIZE)) + ((i % INODES_PER_BLOCK)*FS->inode_size));
		}
		//each iteration through the loop will grab 4 inodes, since we can fit 4 inodes per block
		inode_table_cache[i] = NULL;
	}
	// inode_table_cache = (inode*) inode_table_temp;  // cast the void pointer to an Inode pointer

	//what was this for? don't think we need it...
	// data_table = kmalloc(BLOCKSIZE); // pointer to the start of data table
	// receive(data_table, (FS->start_data_blocks_loc) * BLOCKSIZE); 
}

int kopen(char* filepath, char mode){
	int fd;
	int inum = 0;
	struct inode cur_inode;
	int exit_flag = 1;
	while(exit_flag){
		int k = 1;
		char next_path[MAX_NAME_LENGTH] = {0};
		while((filepath[k] != '/') && (k <= MAX_NAME_LENGTH)){
			next_path[k] = filepath[k];
			k++;
		}
		filepath += (k);
		// Look up cur_inode inode in cached inode table
		if(inode_table_cache[inum] != NULL){
			// the inode is in the inode_cache_table, so get it:
			cur_inode = *(inode_table_cache[inum]);	
		}else{ 
			// inode is not in the cache table, so get it from disk:
			void* inode_spaceholder = kmalloc(BLOCKSIZE);
			receive(inode_spaceholder, ((inum/INODES_PER_BLOCK)+FS->start_inode_table_loc)*BLOCKSIZE); // the firs
			struct inode* block_of_inodes = (struct inode*) inode_spaceholder;
			cur_inode = block_of_inodes[inum % INODES_PER_BLOCK];
			// need to implement an eviction policy/function to update the inode_table_cache...
			// this will function w/o it, but should be implemented for optimization
		}
		int i;
		int file_found = 0; // initialize to false (i.e. file not found)
		for(i = 0; i < cur_inode.blocks_in_file; i++){
			void* dir_spaceholder = kmalloc(BLOCKSIZE);
			receive(dir_spaceholder, (root.data_blocks[i])*BLOCKSIZE);
			struct dir_data_block = *((struct dir_data_block*) dir_spaceholder);
			int j;
			for(j = 0; j < (BLOCKSIZE/DIR_ENTRY_SIZE); j++){
				struct dir_entry file_dir = dir_data_block[j]; // 
				int x;
				int is_equal = 1; // initialize to true
				for(x = 0; x < MAX_NAME_LENGTH; x++){
					if(file_dir.name[x]  != next_path[x]){
						is_equal = 0;
						break;
					}//end if
				}//end for
				if(is_equal){
					file_found = 1; //we found the file, so break out of loop
					break;
				}
			}//inner for
			if(file_found){
				break;
			}
		}//outer for

		if(!file_found){//throw an error
			os_printf("404 ERROR! File not found.\nPlease ensure full filepath is specified starting from root (/)\n");
			return -1;
		}
		if(!(cur_inode->is_dir)){
			/*	when we reach the portion of the filepath that is not a dir, we set the exit_flag
			 	to 0 so that we exit the outermost while loop */
			exit_flag = 0;
		}//end if
	}//outer most while loop
	
	//here we have the file we were looking for! it is cur_inode.

	bitvector *p = cur_inode->perms;
	switch mode{
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
			if(get(2, p) == 0){
				os_printf("File Cannot Be Appeneded To\n");
				return -1;
			}	
			break;
		case 'b':
			if(get(3, p) == 0){
				os_printf("File Cannot Be Read and Written\n");
				return -1;
			}	
			break;
		default:
			os_printf("File permission passed\n");
	}

	fd = add_to_opentable(cur_inode, mode);
	return fd;
}//end kopen()


// Helper function for kread():
int read_partial_block(int bytesLeft, void* buf_offset, file_descriptor* fd, void* transferSpace) {
	int local_offset = fd->offset % BLOCKSIZE; // local_offset is the leftmost point in the block

	// Actually get the data for 1 block (the SD Driver will put it in transferSpace for us)
	int blockNum = fd->offset / BLOCKSIZE;
	int success = recieve(transferSpace, blockNum);
	if(success < 0){
	 	// failed on a block receive, therefore the whole kread fails; return failure error
	 	os_printf("failed to receive block number %d\n", numBytes);
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
int read_full_block(int bytesLeft, void* buf_offset, file_descriptor* fd, void* transferSpace) {
	// read BLOCKSIZE
	// Actually get the data for 1 block (the SD Driver will put it in transferSpace for us)
	int blockNum = fd->offset / BLOCKSIZE;
	int success = recieve(transferSpace, blockNum);
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
	file_descriptor* fd = get_file_descriptor(fd_int); // note, get_file_descriptor() function has not yet been implemented...will be in open_table.c

	if (fd->permission != 'r' || fd->permission != 'b') {
		os_printf("no permission \n");
		return -1;
	}

	// Allocate space for and create a bitvector to be used repeatedly to transfer the data:
	uint32_t* transferSpace = kmalloc(BLOCKSIZE);
	int blockNum = 0;

	// start of higher-level algo:
	int bytes_read = 0;
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
	int bytes_written;
        file_descriptor* fd = get_file_descriptor(fd_int);
        if (fd->permission != 'w' || fd->permission != 'b') {
                os_printf("no permission \n");
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
			transmit(transferSpace, blockNum, fd->offset, total_bytes_left);

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
			transmit(transferSpace, blockNum, fd->offset, bytes_left_in_block);

			bytes_written += bytes_left_in_block;
			total_bytes_left -= tbytes_left_in_block;
			fd->offset += bytes_left_in_block;
		} else {
			os_memcpy(buf_offset, transferSpace, (os_size_t) BLOCKSIZE);
			transferSpace -= BLOCKSIZE;
			// pointer to start, blockNum, where we are in file, lengh of write
			transmit(transferSpace, blockNum, fd->offset, BLOCKSIZE);

			bytes_written += BLOCKSIZE;
			total_bytes_left -= BLOCKSIZE;
			fd->offset += BLOCKSIZE;
		}
	}
	return bytes_written;
} // end kwrite();



/* close the file fd, return 1 if the close was successful */
int kclose(int fd) {
	int error;
	error = delete_from_opentable(fd);
	return error;
} // end kclose();



/* seek within the file, return an error if you are outside the boundaries */
int kseek(int fd_int, int num_bytes) {
	int error;
        file_descriptor* fd = get_file_descriptor(fd_int);
        if (fd->permission != 'r' || fd->permission != 'w' || fd->permission != 'b') {
                os_printf("no permission \n");
                return -1;
        }
	fd->offset += num_bytes;	
	return 0;
} // end kseek();



/* create a new file, if we are unsuccessful return -1 */
int kcreate(char* filepath, int mode) {
	int error;

	return error;
} // end kcreate();



/* delete the file with the path filepath. Return -1 if the file does not excist */
int kdelete(char* filepath) {
	int error;
	
	return error;
} // end kdelete();


