#include "../lib/stdio.h"
#include "../lib/syscall_api.h"

void main(void) {
    int contador = 0;
    char buffer[] = "[P1] Digito: X\n"; 
    
    while(1) {
        buffer[13] = '0' + contador;
        sys_write(1, buffer, 15);
        
        // Hacemos el retardo AQUÍ para darle tiempo al Timer de actuar
        for (volatile int i = 0; i < 4000000; i++);
        
        contador++;
        
        // Retrasamos el fallo hasta el dígito 8
        if (contador == 8) {
            sys_write(1, "[P1] Voy a hacer algo ilegal...\n", 32);
            void (*crash)(void) = (void *)0xDEADC0DE;
            crash(); 
        }
    }
}