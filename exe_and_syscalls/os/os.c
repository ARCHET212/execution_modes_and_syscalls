#include "uart.h"
#include "timer.h"
#include "board.h"
#include "../lib/stdio.h"
#include "pcb.h"

extern void pcb_setup_system(void);
extern int current_pid;
extern void context_restore(void);

int kernel_main(void) { 
  
    board_watchdog_disable(); 
    
    os_write("Cargando Sistema Operativo...\n");
    
    pcb_setup_system();
    timer_init();
    
     schedule_next_process();  // current_pid pasa de 0 → 1

    // Trace obligatorio con el pid real del primer task USR (§3.8 fila 1)
    mini_printf("MODE_SWITCH KERNEL_TO_USER pid=%d reason=initial_launch\n", current_pid);

    enable_irq();

    // §3.4: lanzar el primer task con el MISMO mecanismo que el scheduler usa
    // (no un while(1) que espera el timer — eso viola el requisito del spec)
    // Desde C no podemos llamar context_restore directamente, así que declaramos
    // el símbolo externo y saltamos vía asm:
    __asm__ volatile (
        "msr cpsr_c, #0xD2\n"   // Modo IRQ (desde donde opera context_restore)
        "b context_restore\n"    // Restaura el PCB de current_pid y entra a USR
    );

    // Nunca se llega aquí
    while(1);
}