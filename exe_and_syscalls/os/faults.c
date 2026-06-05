#include "faults.h"
#include "pcb.h"
#include "../lib/stdio.h"

extern int current_pid;
extern void schedule_next_process(void);

static void isolate_faulting_process(const char* fault_type_str, uint32_t fault_type_num) {
    // TRACE: Entrada por fallo — §3.8 fila 6
    mini_printf("MODE_SWITCH USER_TO_KERNEL pid=%d reason=fault type=%s\n",
                current_pid, fault_type_str);

    mini_printf("[KERNEL ALERT] Proceso %d fallo (%s). Aislando...\n",
                current_pid, fault_type_str);

    // §6: registrar tipo y razón en el PCB antes de cambiar current_pid
    pcb_ext_table[current_pid].fault_type         = fault_type_num;
    pcb_ext_table[current_pid].termination_reason = REASON_FAULT;
    pcb_table[current_pid].state                  = TERMINATED;

    schedule_next_process(); // current_pid pasa al proceso sano

    // TRACE: Recuperación — §3.8 fila 7 (con el nuevo current_pid)
    mini_printf("MODE_SWITCH KERNEL_TO_USER pid=%d reason=fault_recovery\n", current_pid);
}

void data_abort_c(unsigned int fault_addr) {
    isolate_faulting_process("data_abort", FAULT_TYPE_DATA_ABORT);
}

void prefetch_abort_c(unsigned int fault_addr) {
    isolate_faulting_process("prefetch_abort", FAULT_TYPE_PREFETCH_ABORT);
}