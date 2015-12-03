#include <thread.h>
#include <assert.h>
#include <stdio.h>

void* f(void* arg)
{
   printf("in thread\n");

   return arg;
}

void main(void)
{
   thread_t thread;
   int rc = thread_create(&thread, f, (void*) 1);
   assert(rc == 0);

   int res=0;

   printf("before join\n");

   rc = thread_join(thread, (void**) &res);
   assert(rc == 0 || rc == ERR_THREAD_TERMINATED);

   printf("after join\n");  

   assert(res==1);

   printf("passed\n");

   while (1) { }
}
