#ifndef SYSCALLS_H
#define SYSCALLS_H

// Prototipo para el manejador que será llamado desde ensamblador (root.s)
void syscall_handler_c(unsigned int *regs);

#endif // SYSCALLS_H