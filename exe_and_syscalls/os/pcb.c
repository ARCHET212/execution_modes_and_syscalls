#include "uart.h"
#include "pcb.h"
#include "board.h"

pcb_t pcb_table[MAX_PROCESSES];
pcb_ext_t pcb_ext_table[MAX_PROCESSES];
int current_pid = 0;

void pcb_init_process(int pid, unsigned int entry_point, unsigned int stack_top) {
    pcb_table[pid].pid   = pid;
    pcb_table[pid].state = READY;
    pcb_table[pid].pc    = entry_point;
    pcb_table[pid].sp    = stack_top;
    pcb_table[pid].lr    = entry_point;

    // 0x10 = ARM USR mode, IRQ habilitado (bit 7 = 0)
    pcb_table[pid].spsr  = 0x10;

    for (int i = 0; i < 13; i++) {
        pcb_table[pid].r[i] = 0;
    }

    pcb_ext_table[pid].exit_code          = 0;
    pcb_ext_table[pid].fault_type         = 0;
    pcb_ext_table[pid].termination_reason = REASON_ALIVE;
}

void pcb_setup_system(void) {
    pcb_init_process(0, KERNEL_START, KERNEL_STACK);
    pcb_init_process(1, P1_START,     P1_STACK);
    pcb_init_process(2, P2_START,     P2_STACK);
}

void schedule_next_process(void) {
    // Si el proceso actual sigue vivo, lo ponemos en READY para volver más tarde.
    // pid=0 es el kernel — nunca se reinserta en el scheduler.
    if (current_pid != 0 && pcb_table[current_pid].state != TERMINATED) {
        pcb_table[current_pid].state = READY;
    }

    // FIX: El scheduler solo considera procesos de usuario (pid >= 1).
    // pid=0 es el kernel y nunca debe ejecutarse como tarea de usuario
    // porque su PC apunta a _start y su código tiene instrucciones privilegiadas
    // que causarían una excepción al correr en USR mode.
    int intentos = 0;
    do {
        // Avanzar dentro del rango de procesos de usuario [1 .. MAX_PROCESSES-1]
        current_pid = (current_pid % (MAX_PROCESSES - 1)) + 1;
        intentos++;

        // Política de idle: si todos los procesos de usuario terminaron, halt.
        if (intentos > MAX_PROCESSES - 1) {
            os_write("[KERNEL] Todos los procesos terminaron. Sistema en halt.\n");
            while (1);
        }
    } while (pcb_table[current_pid].state == TERMINATED);

    pcb_table[current_pid].state = RUNNING;
}