#ifndef _FLOAT_H
#define _FLOAT_H

#define SYSCALL_PRCS_YIELD 17  // create a file
#define SYSCALL_PRCS_LISTEN 18  // create a file
#define SYSCALL_PRCS_EMIT 19  // create a file

int process_yield();

int process_listen(
		void (*listener)(uint32_t src_tid, uint32_t event, char * data,
				int length));

int process_emit(uint32_t dest_pid, uint32_t event, char * data, int len);

#endif
