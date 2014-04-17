#ifndef	_SETJMP_H
#define	_SETJMP_H

#include <bits/setjmp.h>

typedef struct __jmp_buf_tag {
	__jmp_buf __jb;
	unsigned long __fl;
	unsigned long __ss[128/sizeof(long)];
} jmp_buf[1];

// typedef jmp_buf sigjmp_buf;
// int sigsetjmp (sigjmp_buf, int);
// _Noreturn void siglongjmp (sigjmp_buf, int);

int setjmp (jmp_buf);
_Noreturn void longjmp (jmp_buf, int);

#define setjmp setjmp
#define longjmp longjmp

#endif
