#include <stdio.h>
#include <stdint.h>
#include "../../kernel/include/hw_handler.h"
#include "../../kernel/include/open_table.h"


/*open() returns a file descriptor...which is a file_descriptor pointer
   to the fd in the open file description table...found in open_table.c*/
file_descriptor* open(char* filepath, char mode){
	if(mode != 'r' || mode != 'w' || mode != 'a'){
		os_printf("mode is not a valid option. must be r, w, or a\n");
		return -1;
	}//end if

	file_descriptor* fd;
	// preserve registers r1, r2, and r3 on the stack
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

/*create() returns a file descriptor...which is a file_descriptor pointer
   to the fd in the open file description table...found in open_table.c*/
file_descriptor* create(char* filepath){
	file_descriptor fd;
	fd = open(filepath, 'w');
	return fd;
}//end create syscall

/*read() returns a file descriptor...which is a small non-negative int value
  that corresponds to the index of the file in the system wide open file
  description table*/
int read(file_descriptor* fd, void* buf, int numBytes){
	if(!fd->open){
		os_printf("fd does not point to a valid open file\n");
		return -1;
	}//end if
	if (numBytes<=0)
		os_printf(“Invalid number of bytes \n”);
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
	returns bytesRead;
	//note, actual data read from file gets moved to buf in kread()
}//end open syscall

/*write() returns the number of bytes that were actually written*/
int write(file_descriptor* fd, void* buf, int numBytes){
	// Error checking:
	if(!fd->open){
		os_printf("fd does not point to a valid open file\n");
		return -1;
	}//end if
	// more error checking:
	if (numBytes<=0){
		os_printf("Invalid number of bytes\n");
		return -1;
	}//end if
	
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
int close(file_descriptor* fd){
	int error; //0 if close ok, -1 if error. This will be return.
	/* the compiler should  throw an error if a user passes a pointer argument that
	   is not of the type file_descriptor*/
	// error checking:
	if(!fd->open){
		// file is already closed, so return:
		return 0;
	}// no other error checking needed...it is up to the user to pass the right fd pointer

	// preserve registers r1, r2, and r3 on the stack  —— need all 3?
    asm volatile("push r1" "\n\t");

	// move the arguments into registers for kclose to use:
	asm volatile("mov %0, (fd)": "r1");//note, <register> not necessarily correct...need to figure out which register to use

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
int seek(file_descriptor* fd, int numBytes){
	int error; //-1 if numBytes + current file offset exceeds file length. This will be return.
	// Error checking...no need to go into kernel if numBytes is invalid:
	if(numBytes == 0){
		return 0;
	} else if ((numBytes > 0) && ((fd->offset + numBytes) > (fd->file)->size)){
		os_printf("Error! file offset exceeds file size (numBytes is positive\n");
		return -1;
	}else if ((numBytes < 0) && ((fd->offset + numBytes) < 0)){
		os_printf("Error! file offset exceeds beginning of file (numBytes is negative\n");
		return -1;
	}//end if else

	// preserve registers r1, r2, and r3 on the stack  —— need all 3?
    asm volatile("push r1" "\n\t"
                 "push r2" "\n\t");

	// move the arguments into registers for kseek() to use:
	asm volatile("mov %0, (fd)": "r1");//note, <register> not necessarily correct...need to figure out which register to use
	asm volatile("mov %0, (numBytes)": "r2");//note, <register> not necessarily correct...need to figure out which register to use

	// trigger the software_interrupt_handler in hw_handler.c:
	asm volatile("swi SYSCALL_SEEK");
	// kseek() gets called from within hw_handler.c	

	// retrieve the return values from kread to pass back to user:
	asm volatile("mov %0, r1" : "=r" (error));

	// Restore r1, r2, and r3 to original values
    asm volatile("pop r2" "\n\t"
                 "pop r1" "\n\t");
	return error;
}//end seek syscall


/*delete() returns the 0 if successful and -1 if not*/
int delete(char* filepath){
	int error; //0 if delete ok, -1 if error. This will be return.
	// error checking:
	// probably will have to check  if file exists in kdelete, but check here if possible...
	// preserve registers r1, r2, and r3 on the stack
    asm volatile("push r1" "\n\t");

	// move the arguments into registers for kopen() to use:
	asm volatile("mov %0, (filepath)": "r1");

	// trigger the software_interrupt_handler in hw_handler.c:
	asm volatile("swi SYSCALL_OPEN");
	// kopen() gets called from within hw_handler.c

	// retrieve the return values from kopen to pass back to user:
	asm volatile("mov %0, r1" : "=r" (error));

	// Restore r1, r2, and r3 to original values
    asm volatile("pop r1" "\n\t");
	return error;
}//end delete syscall
