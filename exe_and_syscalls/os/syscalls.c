#include "syscalls.h"
#include "pcb.h"
#include "../lib/stdio.h"
#include "uart.h"

extern int current_pid;
extern void schedule_next_process(void);

// Límites de las regiones de memoria de usuario.
// Se seleccionan automáticamente según la placa objetivo definida por el Makefile
// (-D_QEMU_ o -D_BBB_), por lo que este archivo compila correctamente para ambas.
#if defined(_QEMU_)
    #define P1_MEM_START  0x00110000U
    #define P1_MEM_END    0x00120000U   // P1_START + 64K stack (QEMU)
    #define P2_MEM_START  0x00210000U
    #define P2_MEM_END    0x00220000U   // P2_START + 64K stack (QEMU)
#elif defined(_BBB_)
    #define P1_MEM_START  0x82100000U
    #define P1_MEM_END    0x82110000U   // P1_START + 64K stack (BBB)
    #define P2_MEM_START  0x82200000U
    #define P2_MEM_END    0x82210000U   // P2_START + 64K stack (BBB)
#else
    #error "Placa no definida. Usa BOARD=qemu o BOARD=BBB al compilar."
#endif

// §4.7: Valida que buf+len quepan DENTRO de una región de usuario válida.
// Previene punteros kernel, punteros salvajes (0xFFFFFFFF) y overflow aritmético.
static int is_valid_user_ptr(unsigned int ptr, unsigned int len) {
    if (len == 0) return 0;

    // Verificar overflow aritmético en ptr + len
    if (ptr + len < ptr) return 0;

    unsigned int end = ptr + len;

    if (ptr >= P1_MEM_START && end <= P1_MEM_END) return 1;
    if (ptr >= P2_MEM_START && end <= P2_MEM_END) return 1;

    return 0;
}

// BUG FIX §4.7: os_write asume null-termination; escribimos byte a byte
// usando la longitud real del usuario para no depender del terminador.
static unsigned int write_user_bytes(const char *buf, unsigned int len) {
    unsigned int i;
    for (i = 0; i < len; i++) {
        uart_putc(buf[i]);
    }
    return len;
}

void syscall_handler_c(unsigned int *regs) {
    int syscall_id = (int)regs[0];

    // §4.1 / §4.5: Verificar que la syscall vino desde modo usuario (USR = 0x10).
    unsigned int caller_mode = pcb_table[current_pid].spsr & 0x1F;
    if (caller_mode != 0x10) {
        regs[0] = (unsigned int)(-1);
        return;
    }

    // §6: guardar syscall transient state en el PCB para observabilidad
    pcb_ext_table[current_pid].last_syscall_id = syscall_id;
    pcb_ext_table[current_pid].last_syscall_rc = 0;

    // TRACE: Entrada al kernel
    mini_printf("MODE_SWITCH USER_TO_KERNEL pid=%d reason=syscall id=%d\n",
                current_pid, syscall_id);

    switch (syscall_id) {

        case 0: // SYS_YIELD — §4.5
            schedule_next_process();
            regs[0] = 0;
            // TRACE: retorno (current_pid puede haber cambiado tras yield)
            pcb_ext_table[current_pid].last_syscall_rc = 0;
            mini_printf("MODE_SWITCH KERNEL_TO_USER pid=%d reason=syscall_return id=0 rc=0\n",
                        current_pid);
            break;

        case 1: { // SYS_EXIT — §4.6
            // BUG FIX: guardar pid y exit_code ANTES de schedule_next_process(),
            // que sobreescribe current_pid con el proceso nuevo.
            int exiting_pid = current_pid;
            int32_t exit_code = (int32_t)regs[1];

            pcb_ext_table[exiting_pid].termination_reason = REASON_NORMAL_EXIT;
            pcb_ext_table[exiting_pid].exit_code          = exit_code;
            pcb_table[exiting_pid].state                  = TERMINATED;

            // BUG FIX: emitir trace de SALIDA con el pid saliente ANTES de
            // cambiar current_pid, con rc = exit_code según §3.8 fila 5.
            pcb_ext_table[exiting_pid].last_syscall_rc = exit_code;
            mini_printf("MODE_SWITCH KERNEL_TO_USER pid=%d reason=syscall_return id=1 rc=%d\n",
                        exiting_pid, (int)exit_code);

            schedule_next_process(); // current_pid pasa al nuevo proceso
            // SYS_EXIT nunca retorna al caller; context_restore cargará el nuevo proceso
            break;
        }

        case 2: { // SYS_WRITE — §4.7
            if (regs[1] != 1) {
                regs[0] = (unsigned int)(-2);
                mini_printf("MODE_SWITCH KERNEL_TO_USER pid=%d reason=syscall_return id=2 rc=-2\n",
                            current_pid);
                break;
            }

            unsigned int buffer_ptr = regs[2];
            unsigned int length     = regs[3];

            // §4.7: Validar puntero+longitud dentro de región de usuario válida
            if (!is_valid_user_ptr(buffer_ptr, length)) {
                regs[0] = (unsigned int)(-3);
                mini_printf("MODE_SWITCH KERNEL_TO_USER pid=%d reason=syscall_return id=2 rc=-3\n",
                            current_pid);
                break;
            }

            // BUG FIX: escribir exactamente 'length' bytes sin depender de null-terminator
            unsigned int written = write_user_bytes((const char*)buffer_ptr, length);
            regs[0] = written;

            mini_printf("MODE_SWITCH KERNEL_TO_USER pid=%d reason=syscall_return id=2 rc=%d\n",
                        current_pid, (int)written);
            break;
        }

        default:
            regs[0] = (unsigned int)(-1);
            mini_printf("MODE_SWITCH KERNEL_TO_USER pid=%d reason=syscall_return id=%d rc=-1\n",
                        current_pid, syscall_id);
            break;
    }
}