#include <thread.h>

void* f(void* arg)
{
   return arg;
}

void main(void)
{
   thread_t thread;
   thread_create(&thread, f, (void*) 1);
}
