#include <thread.h>
#include <assert.h>

void* f(void* arg)
{
   return arg;
}

void main(void)
{
   thread_t thread;
   thread_create(&thread, f, (void*) 1);

   int res=0;

   thread_join(thread, (void**) &res);
   assert(res==1);
}
