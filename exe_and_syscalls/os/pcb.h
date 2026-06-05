#include <stdint.h>
#ifndef PCB_H
#define PCB_H

#define REASON_ALIVE       0u
#define REASON_NORMAL_EXIT 1u
#define REASON_FAULT       2u

typedef struct {
    int pid;
    int state;
    unsigned int pc;
    unsigned int sp;
    unsigned int lr;
    unsigned int spsr;
    unsigned int r[13];
} pcb_t;

#define MAX_PROCESSES 3

typedef enum {
    READY = 0,
    RUNNING = 1,
    WAITING = 2,
    TERMINATED = 3
} process_state_t;

typedef struct {
    int32_t  exit_code;
    uint32_t fault_type;
    uint32_t termination_reason;
    // §6 syscall transient state — para observabilidad y logging
    int32_t  last_syscall_id;
    int32_t  last_syscall_rc;
} pcb_ext_t;

extern pcb_ext_t pcb_ext_table[MAX_PROCESSES];

extern pcb_t pcb_table[3];
extern int current_pid;

void schedule_next_process(void);
void pcb_init_process(int pid, unsigned int entry_point, unsigned int stack_top);
void pcb_setup_system(void);

#endif // PCB_H