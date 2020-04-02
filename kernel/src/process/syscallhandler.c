#include <syscallhandler.h>
#include <stdio.h>
#include <scheduler.h>

void handle_syscall(size_t syscallnr, size_t param1, size_t param2, size_t param3, size_t param4) {
    schedule_out(get_scheduler());
    INFO("Handling syscall %i", syscallnr);
    schedule_in(get_scheduler());
//    switch (syscallnr) {
//        case
//
//    }
}
