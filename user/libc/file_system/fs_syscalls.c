#include <stdint.h>
#include "../../include/fs_syscalls.h"
#include "../arch/arm/syscall_arch.h"

/* open() returns a file descriptor, a long used to call later fuctions.
A return value of -1 tells that an error occurred and the file cannot be opened */
int open(char* filepath, char mode){
	if(mode != 'r' && mode != 'w' && mode != 'a' && mode!='b'){
		
		//os_printf("mode is not a valid option. \n");
		//os_printf("r=read, w=write, b=both read and write, a=append.\n");
		return -1;
	}

	/* move filepath and mode into registers r0 and r1, call SYSCALL_OPEN */
	long fd = __syscall2(SYSCALL_OPEN, (long)filepath, (long)mode);

	return (int) fd;
}//end open syscall
 
/* create() returns a file descriptor to use in later functions */
int create(char* filepath, char mode){

	if(mode != 'r' && mode != 'w' && mode != 'a' && mode!='b'){
		
		//os_printf("mode is not a valid option. \n");
		//os_printf("r=read, w=write, b=both read and write, a=append.\n");
		return -1;
	}

	/* move filepath and mode into registers r0 and r1, call SYSCALL_MKDIR */
	long error = __syscall2(SYSCALL_CREATE, (long)filepath, (long)mode);

	return (int)error;
}//end create syscall

//creates a directory
int mkdir(char* filepath) {
	long error = __syscall1(SYSCALL_MKDIR, (long)filepath);
	return (int)error;
}

/* read() returns the amount of bytes successfully read from the file */
int read(int fd, void* buf, int numBytes){

	if (numBytes<=0) { return -1; }//os_printf("Invalid number of bytes \n"); } */
		
	/* move fd, buf, and numBytes into r0, r1, r2 and call SYSCALL_READ */
	long bytesRead = __syscall3(SYSCALL_READ, (long)fd, (long)buf, (long)numBytes);

	return (int)bytesRead;
}//end open syscall

/*write() returns the number of bytes that were actually written*/
int write(int fd, void* buf, int numBytes){

	if (numBytes<=0){
		//os_printf("Invalid number of bytes\n");
		return -1;
	}
	
	/* move fd, buf, and numBytes into r0, r1, r2 and call SYSCALL_WRITE */
	long bytesWritten = __syscall3(SYSCALL_READ, (long)fd, (long)buf, (long)numBytes);

	return (int)bytesWritten;
}//end write syscall

/* close() returns the 0 if successful and -1 if not */
int close(int fd){

	/* move fd to r0 and call SYSCALL_CLOSE */
    long error = __syscall1(SYSCALL_CLOSE, (long)fd);

	return (int)error;
}//end close syscall

/* seek() returns the number of bytes that the file offset is successfully moved
  if successful, and -1 if the numBytes puts puts them outside the file length */
int seek(int fd, int numBytes){

	if(numBytes == 0){
		return 0;
	} 

	/* move fd, numBytes into registers r0, r1 and call SYSCALL_SEEK */
	long error = __syscall2(SYSCALL_SEEK, (long) fd, (long) numBytes);

	return (int)error;
}//end seek syscall

/* delete() returns the 0 if successful and -1 if not */
int delete(char* filepath){

	/* move filepath to r0 and call SYSCALL_DELETE */
	long error = __syscall1(SYSCALL_DELETE, (long) filepath);

	return (int)error;
}//end delete syscall

//copies contents of a file
int copy(char* source, char* dest, char mode) {
	if (mode != '0' && mode != 'r' && mode != 'w') {
		//os_printf("Enter mode of new file as r for read only or w for write, or 0 if copying a directory \n");
		return -1;
	}
	long error = __syscall3(SYSCALL_COPY, (long)source, (long) dest, (long) mode);
	return (int)error;
}

//lists elements in a directory
int ls(char* filepath) {
	if (filepath == 0x0) {
		//os_printf("enter a path! \n");
		return -1;
	}
	long error = __syscall1(SYSCALL_LS, (long)filepath);
	return (int)error;
}

