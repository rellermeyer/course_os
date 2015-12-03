#include <thread.h>
#include <assert.h>
#include <stdio.h>

#define NUM_THREADS 10

void* f(void* arg)
{
   printf("this is the thread\n");
   return arg;
}

void main(void)
{
   thread_t thread[NUM_THREADS];
   printf("before thread_create\n");
   for (int i=0; i<10; i++)
   {
   	thread_create(&thread[i], f, (void*) i);
   }
   printf("after thread_create\n");

   while (1) { }
}
