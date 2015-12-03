#include <thread.h>
#include <assert.h>
#include <stdio.h>

void main(void)
{
   thread_t thread;

   int rc = thread_create(&thread, NULL, (void*) 1);

   assert(rc == ERR_INVALID_ARGUMENTS);

   printf("passed\n");
}
