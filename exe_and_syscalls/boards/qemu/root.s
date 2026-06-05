.section .text
.syntax unified
.code 32

.globl _start
.extern pcb_table
.extern current_pid
.extern kernel_main
.extern timer_irq_handler
.extern os_write
@ --- NUEVOS MÓDULOS EN C ---
.extern syscall_handler_c
.extern data_abort_c
.extern prefetch_abort_c

_start:
    b reset_handler

// --- TABLA DE VECTORES DE EXCEPCIÓN ---
.align 5
vector_table:
    ldr pc, reset_addr
    ldr pc, undef_addr
    ldr pc, swi_addr
    ldr pc, prefetch_addr
    ldr pc, data_addr
    ldr pc, reserved_addr
    ldr pc, irq_addr
    ldr pc, fiq_addr

// Direcciones absolutas resueltas por el Linker
reset_addr:    .word reset_handler
undef_addr:    .word undefined_handler
swi_addr:      .word swi_handler
prefetch_addr: .word prefetch_handler
data_addr:     .word data_handler
reserved_addr: .word 0
irq_addr:      .word irq_handler
fiq_addr:      .word fiq_handler

// --- RUTINA PRINCIPAL DE ARRANQUE ---
reset_handler:
    // 1. Configurar Stacks para todos los modos
    ldr r0, =_stack_top    @ R0 = Tope de la RAM (8MB según tu linker.ld)

    msr cpsr_c, #0xD2      @ Entrar a Modo IRQ
    mov sp, r0             @ Stack de IRQ (Tope de RAM)

    msr cpsr_c, #0xD3      @ Entrar a Modo SVC (Para llamadas al sistema SWI)
    sub r0, r0, #0x1000    @ Bajar 4KB
    mov sp, r0             @ Stack de SVC

    msr cpsr_c, #0xD7      @ Entrar a Modo ABT (Para Data/Prefetch Abort)
    sub r0, r0, #0x1000    @ Bajar 4KB
    mov sp, r0             @ Stack de Abortos

    msr cpsr_c, #0xDF      @ Entrar a Modo System (OS y Procesos)
    sub r0, r0, #0x1000    @ Bajar 4KB
    mov sp, r0             @ Stack de System

    // 2. Limpiar BSS (Asumiendo que r1 y r2 se preparan antes en tu código original, 
    // lo dejo tal cual lo tenías aunque falten las declaraciones de _bss_start)
bss_loop:
    cmp r0, r1
    strlo r2, [r0], #4
    blo bss_loop
    
    // 3. Copiar Tabla de Vectores a 0x00000000
    mov r0, #0x00000000    
    ldr r1, =vector_table  
    
    ldmia r1!, {r2-r9}     
    stmia r0!, {r2-r9}     
    ldmia r1!, {r2-r9}     
    stmia r0!, {r2-r9}     
    
    // 4. Saltar a C
    bl kernel_main


// =====================================================================
// === MANEJADOR DE LLAMADAS AL SISTEMA (SVC / SWI) ===
// =====================================================================
swi_handler:
    @ A diferencia de antes, una Syscall (ej. SYS_YIELD o SYS_EXIT) puede cambiar de proceso.
    @ Por lo tanto, DEBEMOS guardar todo el contexto en el PCB igual que el timer.
    push {r0-r12}          @ Guardado preventivo en stack SVC

    ldr r0, =current_pid
    ldr r0, [r0]
    ldr r1, =pcb_table
    mov r2, #76
    mla r1, r0, r2, r1     @ r1 = pcb_t actual

    str lr, [r1, #8]       @ Guardar PC de retorno
    mrs r2, spsr
    str r2, [r1, #20]      @ Guardar SPSR

    msr cpsr_c, #0xDF      @ Bajar a modo System/User
    mov r2, sp             
    mov r3, lr             
    msr cpsr_c, #0xD3      @ Volver a modo SVC
    str r2, [r1, #12]      @ Guardar SP del proceso
    str r3, [r1, #16]      @ Guardar LR del proceso

    @ Mover R0-R12 del stack SVC al PCB
    mov r2, sp
    add r3, r1, #24
    ldmia r2, {r4-r12}
    stmia r3!, {r4-r12}
    add r2, r2, #36
    ldmia r2, {r4-r7}
    stmia r3, {r4-r7}      

    @ --- Llamar al C ---
    @ Pasamos como argumento (R0) el puntero directo a pcb.regs[0].
    @ Así el C puede leer los argumentos y guardar el valor de retorno (regs[0] = éxito).
    add r0, r1, #24        
    bl syscall_handler_c   

    @ Limpiamos el stack de SVC
    add sp, sp, #52        

    @ Para restaurar, pasamos a modo IRQ y usamos la rutina unificada
    msr cpsr_c, #0xD2      
    b context_restore


// =====================================================================
// === MANEJADOR DE CAMBIO DE CONTEXTO (TIMER IRQ) ===
// =====================================================================
irq_handler:
    sub lr, lr, #4         
    push {r0-r12}          

    ldr r0, =current_pid
    ldr r0, [r0]
    ldr r1, =pcb_table
    mov r2, #76            
    mla r1, r0, r2, r1     

    str lr, [r1, #8]       
    mrs r2, spsr
    str r2, [r1, #20]      

    msr cpsr_c, #0xDF      
    mov r2, sp             
    mov r3, lr             
    msr cpsr_c, #0xD2      
    str r2, [r1, #12]      
    str r3, [r1, #16]      

    mov r2, sp
    add r3, r1, #24        
    ldmia r2, {r4-r12}     
    stmia r3!, {r4-r12}    
    add r2, r2, #36        
    ldmia r2, {r4-r7}      
    stmia r3, {r4-r7}      

    @ --- PLANIFICADOR EN C ---
    bl timer_irq_handler

    @ Limpiamos el stack de IRQ antes de saltar a la restauración
    add sp, sp, #52        

// =====================================================================
// === RUTINA UNIFICADA DE RESTAURACIÓN DE CONTEXTO (TRAP-FRAME) ===
// =====================================================================
.global context_restore
context_restore:
    @ A este punto podemos llegar desde el Timer, desde un Syscall, o desde un Fallo.
    @ Siempre se ejecutará en modo IRQ (0xD2)
    ldr r0, =current_pid
    ldr r0, [r0]
    ldr r1, =pcb_table
    mov r2, #76
    mla r1, r0, r2, r1

    ldr r2, [r1, #20]      
    msr spsr_cxsf, r2      @ Restaurar SPSR
    ldr lr, [r1, #8]       @ Restaurar PC en LR IRQ

    ldr r2, [r1, #12]
    ldr r3, [r1, #16]
    msr cpsr_c, #0xDF      @ Bajar a modo System/User
    mov sp, r2
    mov lr, r3
    msr cpsr_c, #0xD2      @ Subir a modo IRQ

    @ Restaurar R0-R12 directos al CPU
    add r2, r1, #24
    ldmia r2, {r0-r12}

    subs pc, lr, #0        @ Retornar ejecución y aplicar SPSR a CPSR


// =====================================================================
// === EXCEPCIONES FATALES (ABORTOS) ===
// =====================================================================
undefined_handler:
    b .

prefetch_handler:
    sub lr, lr, #4
    push {r0-r12, lr}      @ Guardar en stack ABT para la llamada a C
    mov r0, lr
    bl prefetch_abort_c    @ C aísla el proceso, lo termina y llama al planificador
    pop {r0-r12, lr}       @ Limpiar stack ABT
    
    msr cpsr_c, #0xD2      @ Forzar paso a modo IRQ
    b context_restore      @ Cargar el proceso sano elegido

data_handler:
    sub lr, lr, #8
    push {r0-r12, lr}      @ Guardar en stack ABT para la llamada a C
    mov r0, lr
    bl data_abort_c        @ C aísla el proceso, lo termina y llama al planificador
    pop {r0-r12, lr}       @ Limpiar stack ABT
    
    msr cpsr_c, #0xD2      @ Forzar paso a modo IRQ
    b context_restore      @ Cargar el proceso sano elegido

fiq_handler:
    b .