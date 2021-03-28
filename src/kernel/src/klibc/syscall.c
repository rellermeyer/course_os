/**
 * Defines a simple syscall interface
 */

void syscall(const int syscall, const int one, const int two, const int three) {
    asm volatile("push {lr}");
    asm volatile("bl perform_syscall");
    asm volatile("pop {lr}");

}
