#include "syscall_api.h"

// Envoltorio ensamblador genérico (Trap al Kernel)
static int do_syscall(int id, int arg1, int arg2, int arg3) {
    register int r0 __asm__("r0") = id;
    register int r1 __asm__("r1") = arg1;
    register int r2 __asm__("r2") = arg2;
    register int r3 __asm__("r3") = arg3;

    __asm__ volatile (
        "svc #0"
        : "=r" (r0)
        : "r" (r0), "r" (r1), "r" (r2), "r" (r3)
        : "memory"
    );
    return r0;
}

void sys_yield(void) {
    do_syscall(SYS_YIELD, 0, 0, 0);
}

// FIX: sys_exit ahora acepta y pasa el exit code al kernel (§4.6)
void sys_exit(int code) {
    do_syscall(SYS_EXIT, code, 0, 0);
    while (1) {}  // Nunca debe retornar
}

int sys_write(int fd, const char* str, int len) {
    return do_syscall(SYS_WRITE, fd, (int)str, len);
}