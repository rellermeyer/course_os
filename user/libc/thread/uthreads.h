
#ifndef __UTHREADS__
#define __UTHREADS__

#include <global_defs.h>
#include <stdint.h>
#include "jump.h"

size_t size(struct queue* q);
struct thread* back(struct queue* q);
struct thread* front(struct queue* q);
void push(struct queue* q, struct thread* data);
void pop(struct queue* q);
struct thread* thread_create(void(*f)(void *arg), void *arg);
void thread_add_runqueue(struct thread * t);
void thread_exit();
void dispatch();
void schedule(void);
void thread_yield(void);
void thread_start_threading();

#endif
