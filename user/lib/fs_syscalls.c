#include <stdio.h>
#include <stdint.h>
#include "../../kernel/include/hw_handler.h"
#include "../../kernel/include/open_table.h"


/*open() returns a file descriptor, an int used to call later fuctions.
A return value of -1 tells that an error occurred and the file cannot be opened*/
int open(char* filepath, char mode){
	if(mode != 'r' || mode != 'w' || mode != 'a' || mode!='b'){
		os_printf("mode is not a valid option. \n");
		os_printf("r=read, w=write, b=both read and write, a=append.\n");
		return -1;
	}//end if

	int fd;
	// preserve registers r1, r2 on the stack
         asm volatile("push r1" "\n\t"
                      "push r2" "\n\t");

	// move the arguments into registers for kopen() to use:
	asm volatile("mov %0, (filepath)": "r1");
	asm volatile("mov %0, (mode)": "r2");

	// trigger the software_interrupt_handler in hw_handler.c:
	asm volatile("swi SYSCALL_OPEN");
	// kopen() gets called from within hw_handler.c

	// retrieve the return values from kopen to pass back to user:
	asm volatile("mov %0, r1" : "=r" (fd));

	// Restore r1, r2, and r3 to original values
        asm volatile("pop r2" "\n\t"
                     "pop r1" "\n\t");
	
	return fd;
}//end open syscall
 


/*create() returns a file descriptor to use in later functions*/
int create(char* filepath){
	int fd;
	fd = open(filepath, 'w');
	return fd;
}//end create syscall



/*read() returns the amount of bytes successfully read from the file*/
int read(int fd, void* buf, int numBytes){

	if(!file_is_open(fd)){
		os_printf("file not open \n");
		return -1;
	}
	if (numBytes<=0)
		os_printf("Invalid number of bytes \n");

	int bytesRead; //this will be what we return

        // preserve registers r1, r2, and r3 on the stack
        asm volatile("push r1" "\n\t"
                     "push r2" "\n\t"
                     "push r3" "\n\t");

	// move the arguments into registers for kread to use:
	asm volatile("mov %0, (fd)": "r1");
	asm volatile("mov %0, (buf)": "r2")
	asm volatile("mov %0, (numBytes)": "r3");

	// trigger the software_interrupt_handler in hw_handler.c:
	asm volatile("swi SYSCALL_READ");
	// kread() gets called from within hw_handler.c

	// retrieve the return values from kread to pass back to user:
	asm volatile("mov %0, r1" : "=r" (bytesRead));

	// Restore r1, r2, and r3 to original values
        asm volatile("pop r3" "\n\t"
                 "pop r2" "\n\t"
                 "pop r1" "\n\t");
	return bytesRead;
	//note, actual data read from file gets moved to buf in kread()
}//end open syscall



/*write() returns the number of bytes that were actually written*/
int write(int fd, void* buf, int numBytes){

	if(!file_is_open(fd)){
		os_printf("file not open \n");
		return -1;
	}
	if (numBytes<=0){
		os_printf("Invalid number of bytes\n");
		return -1;
	}
	
	int bytesWritten; //this will be what we return

	// preserve registers r1, r2, and r3 on the stack
         asm volatile("push r1" "\n\t"
                      "push r2" "\n\t"
                      "push r3" "\n\t");

	// move the arguments into registers for kwrite to use:
	asm volatile("mov %0, (fd)": "r1");
	asm volatile("mov %0, (buf)": "r2");
	asm volatile("mov %0, (numBytes)": "r3");

	// trigger the software_interrupt_handler in hw_handler.c:
	asm volatile("swi SYSCALL_WRITE");
	// kwrite() gets called from within hw_handler.c	

	// retrieve the return values from kwrite() to pass back to user:
	asm volatile("mov %0, r1" : "=r" (bytesWritten));

	// Restore r1, r2, and r3 to original values
         asm volatile("pop r3" "\n\t"
                      "pop r2" "\n\t"
                      "pop r1" "\n\t");
	return bytesWritten;
	//note, actual data written from file gets moved to buf in kwrite()
}//end write syscall



/*close() returns the 0 if successful and -1 if not*/
int close(int fd){
	int error; //0 if close ok, -1 if error. This will be return.
	
        if(!file_is_open(fd)){
                os_printf("file not open \n");
                return -1;
        }

	// preserve registers r1, r2, and r3 on the stack  —— need all 3?
        asm volatile("push r1" "\n\t");

	// move the arguments into registers for kclose to use:
	asm volatile("mov %0, (fd)": "r1");

	// trigger the software_interrupt_handler in hw_handler.c:
	asm volatile("swi SYSCALL_CLOSE");
	// kclose() gets called from within hw_handler.c	

	// retrieve the return values from kread to pass back to user:
	asm volatile("mov %0, r1" : "=r" (error));

	// Restore r1, r2, and r3 to original values
        asm volatile("pop r1" "\n\t");

	return error;
}//end close syscall



/*seek() returns the number of bytes that the file offset is successfully moved
  if successful, and -1 if the numBytes puts puts them outside the file length*/
int seek(int fd, int numBytes){
	int error; //-1 if numBytes + offset exceeds file length. This will be return.
	// Error checking...no need to go into kernel if numBytes is invalid:
	if(numBytes == 0){
		return 0;
	} else if ((numBytes > 0) && ((fd->offset + numBytes) > (fd->file)->size)){
		os_printf("Error! file offset exceeds file size \n");
		return -1;
	}else if ((numBytes < 0) && ((fd->offset + numBytes) < 0)){
		os_printf("Error! file offset exceeds beginning of file \n");
		return -1;
	}//end if else

	// preserve registers r1, r2 on the stack  —— need all 3?
        asm volatile("push r1" "\n\t"
                     "push r2" "\n\t");

	// move the arguments into registers for kseek() to use:
	asm volatile("mov %0, (fd)": "r1");
	asm volatile("mov %0, (numBytes)": "r2");

	// trigger the software_interrupt_handler in hw_handler.c:
	asm volatile("swi SYSCALL_SEEK");
	// kseek() gets called from within hw_handler.c	

	// retrieve the return values from kread to pass back to user:
	asm volatile("mov %0, r1" : "=r" (error));

	// Restore r1, r2 to original values
        asm volatile("pop r2" "\n\t"
                     "pop r1" "\n\t");
	return error;
}//end seek syscall



/*delete() returns the 0 if successful and -1 if not*/
int delete(char* filepath){
	int error; //0 if delete ok, -1 if error. This will be return.
	// error checking:
	// probably will have to check  if file exists in kdelete, but check here if possible...
	// preserve registers r1 on the stack
        asm volatile("push r1" "\n\t");

	// move the arguments into registers for kdelete() to use:
	asm volatile("mov %0, (filepath)": "r1");

	// trigger the software_interrupt_handler in hw_handler.c:
	asm volatile("swi SYSCALL_DELETE");
	// kopen() gets called from within hw_handler.c

	// retrieve the return values from kopen to pass back to user:
	asm volatile("mov %0, r1" : "=r" (error));

	// Restore r1 to original values
        asm volatile("pop r1" "\n\t");
	return error;
}//end delete syscall





