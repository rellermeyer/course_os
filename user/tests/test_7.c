#include <thread.h>
#include <assert.h>
#include <stdio.h>

void main(void)
{
   thread_t thread = (thread_t) 0x1;

   int rc = thread_join(thread, NULL);

   assert(rc == ERR_INVALID_THREAD);

   printf("passed\n");

   while (1) { }
}
