#ifndef _GLOBAL_DEFS_H_
#define _GLOBAL_DEFS_H_

#include <stdint.h>

#define TRUE 1
#define FALSE 0
#define NULL  ((void*) 0)

typedef char *String;
typedef short Boolean;

typedef uint32_t size_t;

#define UNUSED(x) (void)(x)

#define NBBY 8

#define STATUS_OK 1
#define STATUS_FAIL 0

#define TEST_FAIL   0
#define TEST_OK     1

#endif //defined _GLOBAL_DEFS_H_
