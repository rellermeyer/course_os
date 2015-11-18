#ifndef _GLOBAL_DEFS_H_
#define _GLOBAL_DEFS_H_

#include <stdint.h>

#define TRUE 1
#define FALSE 0

#ifndef NULL
#define NULL  ((void*) 0)
#endif

typedef char *String;
typedef short Boolean;

#ifndef __HOST
typedef uint32_t size_t;
#endif

#define UNUSED(x) (void)(x)

#define NBBY 8

#define STATUS_OK 0
#define STATUS_FAIL -1

#define TEST_OK     0
#define TEST_FAIL   -1

#endif //defined _GLOBAL_DEFS_H_
