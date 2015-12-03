#include <thread.h>
#include <assert.h>
#include <stdio.h>

void* f(void* arg)
{
   printf("in thread\n");

   return thread_self();
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

   int t1 = thread_get_id(thread);
   int t2 = thread_get_id((thread_t) res);

   assert(t1 == t2);

   printf("passed\n");

   while (1) { }
}
