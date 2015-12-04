#include <thread.h>
#include <assert.h>
#include <stdio.h>

void* f(void* arg)
{
   printf("this is the thread\n");

   assert(((int)arg) == 1);

   thread_exit(NULL);
}

void main(void)
{
   thread_t thread;

   printf("before thread_create\n");

   int rc = thread_create(&thread, f, (void*) 1);
   assert(rc == 0);

   printf("after thread_create\n");

   while(1) { }
}
