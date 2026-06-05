.section .text
.syntax unified
.code 32

@ Funciones de bajo nivel compartidas
.global PUT32
PUT32:
    str r1, [r0]
    bx lr

.global GET32
GET32:
    ldr r0, [r0]
    bx lr

.global enable_irq
enable_irq:
    mrs r0, CPSR 
    bic r0, r0, #0x80 
    msr CPSR_c, r0 
    bx lr