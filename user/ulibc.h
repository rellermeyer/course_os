/***********************************************
* User-space libc implementation header file
* Author: Jeffrey Tang
***********************************************/

// Terminates calling process
void exit(int status);

// Yields to another (scheduler-determined) process
void proc_yield();

// Prints to stdout
int printf(const char *format, ...);

