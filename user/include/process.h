#ifndef _FLOAT_H
#define _FLOAT_H

#define SYSCALL_PRCS_YIELD 17  // create a file
#define SYSCALL_PRCS_LISTEN 18  // create a file
#define SYSCALL_PRCS_EMIT 19  // create a file

int process_yield();
//Opens an existing file in filepath with permissions mode. Returns the filedescriptor, or -1 if unsuccessful

#endif
