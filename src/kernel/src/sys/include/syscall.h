#define SYS_fork 57
#define SYS_kill 62
#define SYS_dummy 0
#define SYS_exit 100 
#define SYS_timer_create 222
#define SYS_timer_settime 223
#define SYS_timer_gettime 224
#define SYS_timer_getoverrun 225
#define SYS_timer_delete 226

#include <stdarg.h>

int syscall(long, ...);

int _ksys_create_process(); // only used by the kernel to create the first processes
int sys_fork();
int sys_exit(int status); // status code to be passed to the parent process after exit
int sys_kill(int pid, int sig); // TODO: signals?

// TODO: are timers necessary?
int sys_timer_create();
int sys_timer_settime();
int sys_timer_gettime();
int sys_timer_getoverrun();
int sys_timer_delete();
