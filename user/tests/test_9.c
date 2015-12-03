#include <thread.h>
#include <assert.h>
#include <stdio.h>

void main(void)
{
   thread_self();

   printf("passed\n");

   while (1) { }
}
