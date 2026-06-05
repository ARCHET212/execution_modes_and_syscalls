#include "../lib/stdio.h"
#include "../lib/syscall_api.h"

void main(void) {
    char letra = 'a';
    // Buffer con 18 caracteres de largo
    char buffer[] = "Proceso 2 (P2): X\n"; 
    
    while(1) {
        // Reemplazamos la 'X' (posición 16) con la letra actual
        buffer[16] = letra;
        
        // Un solo sys_write
        sys_write(1, buffer, 18);
        
        /*
        if (letra == 'h') {
            sys_write(1, "[P2] Me volvi loco, adios mundo...\n", 35);
            
            volatile int *puntero_prohibido = (volatile int *)0xFFFFFFFF;
            *puntero_prohibido = 42; 
        }
        */
        
        letra++;
        if (letra > 'z') letra = 'a'; 
        for (volatile int i = 0; i < 4000000; i++);
        sys_yield(); 
    }
}