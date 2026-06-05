#ifndef SYSCALL_API_H
#define SYSCALL_API_H

// IDs de las Syscalls (§4.4)
#define SYS_YIELD 0
#define SYS_EXIT  1
#define SYS_WRITE 2

// Prototipos de la API para el usuario
void sys_yield(void);
void sys_exit(int code);   // FIX: acepta exit code (§4.6)
int  sys_write(int fd, const char* str, int len);

#endif // SYSCALL_API_H