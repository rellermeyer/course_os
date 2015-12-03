#include <thread.h>
#include <assert.h>
#include <stdio.h>

void* f(void* arg)
{
   printf("in thread\n");

   assert(thread_self() != NULL);

   printf("passed\n");

   return NULL;
}

void main(void)
{
   thread_t thread;
   int rc = thread_create(&thread, f, (void*) 1);
   assert(rc == 0);

   while (1) { }
}
