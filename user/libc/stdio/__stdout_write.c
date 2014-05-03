#include "stdio_impl.h"
#include <termios.h>
#include <sys/ioctl.h>

size_t __stdout_write(FILE *f, const unsigned char *buf, size_t len)
{
    return __syscall(SYS_write, buf, len);
}
