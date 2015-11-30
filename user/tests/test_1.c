#include <thread.h>
#include <assert.h>
#include <stdio.h>

void* f(void* arg)
{
   printf("this is the thread\n");
   assert(((int)arg) == 1);
   return arg;
}

void main(void)
{
   thread_t thread;
   printf("before thread_create\n");
   thread_create(&thread, f, (void*) 1);
   printf("after thread_create\n");
}
