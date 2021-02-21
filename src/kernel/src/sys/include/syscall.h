//#define SYS_fork 57
#define SYS_fork 99
#define SYS_kill 62
#define SYS_dummy 9
#define SYS_exit 100 
#define SYS_sleep 101
#define SYS_join 102
#define SYS_timer_create 222
#define SYS_timer_settime 223
#define SYS_timer_gettime 224
#define SYS_timer_getoverrun 225
#define SYS_timer_delete 226

#include <stdarg.h>

int syscall(int, ...);

int _ksys_create_process(); // only used by the kernel to create the first processes
int sys_fork(int* pid); // the pid of the newly created process will be written to wherever `pid` points to
int sys_kill(int pid, int sig); // TODO: signals?
int sys_exit(int status); // status code to be passed to the parent process after exit
int sys_sleep(int ms);

// wait for child to terminate
int sys_join();

// TODO: are timers necessary?
int sys_timer_create();
int sys_timer_settime();
int sys_timer_gettime();
int sys_timer_getoverrun();
int sys_timer_delete();
