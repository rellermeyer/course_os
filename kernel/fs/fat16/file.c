#include <stdint>
#include "../../include/linked_list.h"
#include "../../include/bitvector.h"
#include "../../include/open_table.h"
#include "../../include/mmci.h" //TO BE MADE

//NOTES:
//free list = keeps track of which blocks on the storage device are free (i.e. available)...must be stored persistently


//CONSTANTS:
const int BLOCKSIZE = 2048;


//STRUCTS:
struct file
{
	/* data */
	char* fname;
	list inode;
	int size;
	bitvector perms; // a bitvector of length three to track: read, write, execute
	time creation_time; // need to look up CourseOS specific data type
};

// typedef struct inode
// {
// 	 data 

// } inode;

int kopen(char* filepath, char mode){
	int fd;

	// TODO reach filepath & return pointer to struct file
	//struct file* f = returned file
	bitvector *p = f->perms;
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

	fd = add_to_opentable(f, mode);

	return fd;
}//end kopen()



/* read from fd, put it in buf, then return the number of bytes read in numBytes */
int kread(int fd, void* buf, int numBytes) {
	
	/* Start of my idea of Algo */

	int bytes_read = 0;
	if(numBytes < BLOCKSIZE) {
		while(bytes_read < numBytes) {
			f1();
		}
	} else if(numBytes >= BLOCKSIZE) {
		f1();
	}
	while((numBytes - bytes_read) > BLOCKSIZE) {
		f2();
	} 
	if(bytes_read < numBytes) {
		f1();
	}


	f1() {
		int bytesLeftInBlock = offset % BLOCKSIZE;
		int index = offset / BLOCKSIZE;
		if(bytesLeftInBlock == 0) {
			 ___________________
			|~~~~~~~~~|			|
			---------------------
		} else if((((numBytes - bytes_read) + offset) / BLOCKSIZE) > index) {
			_____________________
			|           |~~~~~~~~|
			----------------------
		} else {
			______________________
			|      |~~~~|         |
			-----------------------
		}
	}

	f2() {
		// read BLOCKSIZE
	}

	/* End my idea of Algo */



	int bytes_read = 0;
	void* buf_offset = buf; //this allows us to move data incrementally to user's buf via buf_offset
	//while retaining the original pointer to return back to the user

	struct file_descriptor filedescr = table[fd];
	if (filedescr.permission != 'r' || filedescr.permission != 'b') {
		os_printf("no permission \n");
		return -1;
	}

	// Allocate space for and create a bitvector to be used repeatedly to transfer the data:
	void* transferSpace = kmalloc(BLOCKSIZE);
	int ct = 0; //for debugging only TODO: remove once debugged
	int blockNum = 0;
	if(numBytes < BLOCKSIZE){








	}else if(numBytes > BLOCKSIZE){
		//read and transfer first chunk:
		blockNum = fd->offset / BLOCKSIZE;
	 	int success = recieve(transferSpace, blockNum);
	 	if(success < 0){
	 		// failed on a block receive, therefore the whole kread fails; return failure error
	 		os_printf("failed to receive block number %d\n", ct);
	 		return -1;
	 	}//end if
	 	ct++;
		// dif is the difference between the current offset and the end of the bloc (always less than BLOCKSIZE)
		int dif = fd->offset % BLOCKSIZE;
		// Actually move the data:
		int i;
		for(i = 0; i < dif; i++){
			*buf_offset = *(transferSpace + (BLOCKSIZE - dif) +)
			buf_offset ++;
			bytes_read ++;
			fd->offset ++;
		}//end for
		//DONE READING FIRST CHUNK, now fd-> is pointing to start of block
		


	}else{






	}




	while(numBytes > BLOCKSIZE){
		blockNum = fd->offset / BLOCKSIZE;
	 	int success = recieve(transferSpace, blockNum);
	 	if(success < 0){
	 		// failed on a block receive, therefore the whole kread fails; return failure error
	 		os_printf("failed to receive block number %d\n", ct);
	 		return -1;
	 	}//end if
	 	ct++;
		numBytes -= BLOCKSIZE;
		if(fd->)
	}//end while

	if(numBytes > 0){
		// then there is more to be read, but less than 1 block
		int success = recieve(transferSpace, );
	 	if(success < 0){
	 		// failed on a block receive, therefore the whole kread fails; return failure error
	 		os_printf("failed to receive block number %d\n", ct);
	 		return -1;
	 	}//end if
	 	ct++;
	}

	recieve();
	//GET RETURN BUFFER FROM RECIVE() 

	return bytes_read;
} // end kread();



/* write from fd, put it in buf, then return the number of bytes written in numBytes */
int kwrite(int fd, void* buf, int num_bytes) {
	int bytes_written;
        struct file_descriptor filedescr = table[fd];
        if (filedescr.permission != 'w' || filedescr.permission != 'b') {
                os_printf("no permission \n");
                return -1;
        }

	//PASS PARAMETERS TO TRANSMIT	
	transmit();
	//GET RETURN VALUES FROM RECEIVE()

	return bytes_written;
} // end kwrite();



/* close the file fd, return 1 if the close was successful */
int kclose(int fd) {
	int error;
	error = delete_from_opentable(fd);
	return error;
} // end kclose();



/* seek within the file, return an error if you are outside the boundaries */
int kseek(int fd, int num_bytes) {
	int error;
        struct file_descriptor filedescr = table[fd];
        if (filedescr.permission != 'r' || filedescr.permission != 'w' || filedescr.permission != 'b') {
                os_printf("no permission \n");
                return -1;
        }
	filedescr.offset += num_bytes;	
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




