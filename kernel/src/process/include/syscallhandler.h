#ifndef SYSCALLHANDLER_H
#define SYSCALLHANDLER_H

#include <stddef.h>

void handle_syscall(size_t syscallnr, size_t param1, size_t param2, size_t param3, size_t param4);

#endif
