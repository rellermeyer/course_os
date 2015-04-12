#include <stdint>
#include "../../include/linked_list.h"
#include "../../include/bitvector.h"
#include "../../include/open_table.h"
#include "../../include/mmci.h" //TO BE MADE

//NOTES:
//free list = keeps track of which blocks on the storage device are free (i.e. available)...must be stored persistently


//CONSTANTS:
const uint16_t BLOCKSIZE = 2048;


//STRUCTS:
struct file
{
	/* data */
	char* fname;
	list inode;
	uint16_t size;
	bitvector perms; // a bitvector of length three to track: read, write, execute
	time creation_time; // need to look up CourseOS specific data type
	uint16_t offset;
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
			os_printf("File permission passed");
	}

	fd = add_to_opentable(f, mode);

	return fd;
}//end kopen()



/* read from fd, put it in buf, then return the number of bytes read in numBytes */
int kread(int fd, void* buf, int numBytes) {
	int bytes_read;
	struct file_descriptor filedescr = table[fd];
	if (filedescr.permission != 'r' || filedescr.permission != 'b') {
		os_printf("no permission \n");
		return -1;
	}

	//PASS PARAMETER TO RECIEVE	
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



/* delete the file with the path filepath. Return -1 if the file does not excist */
int kdelete(char* filepath) {
	int error;
	
	return error;
} // end kdelete();


/*put these in kopen() implementation:
	switch(mode){}
		case "r":
   	    //inline assembly code to move the filepath and mode arguments to registers to be used in kopen():
		
	       break;
	    case "w":
   	    //inline assembly code to move the filepath and mode arguments to registers to be used in kopen():

	       
	       break;
	    case "a":
   	    //inline assembly code to move the filepath and mode arguments to registers to be used in kopen():

	  	    break;
	    default :
			os_printf("Error! Must pass a valid mode parameter\n");
		}
*/



