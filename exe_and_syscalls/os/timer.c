#include "timer.h"
#include "pcb.h"
#include "../lib/stdio.h" // Obligatorio para poder imprimir el log

extern void timer_hw_init(void);
extern void timer_hw_ack(void);
extern int current_pid;

void timer_init(void) {
    timer_hw_init();
}

void timer_irq_handler(void) {
    timer_hw_ack(); 

    // LOG OBLIGATORIO: Entrada al kernel por culpa del timer
    mini_printf("MODE_SWITCH USER_TO_KERNEL pid=%d reason=timer_irq\n", current_pid);

    schedule_next_process();

    // LOG OBLIGATORIO: Salida hacia el nuevo proceso
    mini_printf("MODE_SWITCH KERNEL_TO_USER pid=%d reason=dispatch\n", current_pid);
}