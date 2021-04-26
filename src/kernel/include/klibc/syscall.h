#ifndef SYSCALL_H
#define SYSCALL_H

#include <interrupt.h>

void syscall(const int syscall, const int one, const int two, const int three);

#endif
