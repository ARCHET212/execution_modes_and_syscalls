#include <stdarg.h>
#include "stdio.h"

// Esta es la ÚNICA forma de imprimir desde un proceso
void trigger_svc_write(const char *s) {
    asm volatile (
        "mov r0, %0\n"
        "svc #0\n"
        :
        : "r"(s)
        : "r0"
    );
}

// Necesitas una versión local de itoa si quieres imprimir números
void local_itoa(int n, char *s) {
    int i, sign;
    if ((sign = n) < 0) n = -n;
    i = 0;
    do {
        s[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);
    if (sign < 0) s[i++] = '-';
    s[i] = '\0';
    // Aquí faltaría un reverse simple de la cadena 's'
}

void PRINT(const char *format, ...) {
    // Por ahora, para probar, enviemos el formato directo o 
    // implementa un buffer local si necesitas %d.
    // Como prueba rápida, enviemos el string directamente:
    trigger_svc_write(format); 
}

/*
void READ(const char *format, ...) {
    va_list lista;
    va_start(lista, format);

    char mi_buffer[64]; 
    
    // Recorremos el formato letra por letra
    for (int i = 0; format[i] != '\0'; i++) {
        
        if (format[i] == '%') {
            i++;
            
            // Cada vez que vemos un %, pedimos una entrada del teclado
            uart_gets_input(mi_buffer, 64);

            if (format[i] == 'd') {
                // Sacamos la "dirección de memoria" de la variable (el puntero)
                int *donde_guardar = va_arg(lista, int *);
                // Convertimos el texto del buffer a número y lo guardamos ahí
                *donde_guardar = uart_atoi(mi_buffer);
            } 
            else if (format[i] == 'f') {
                // Lo mismo para decimales
                float *donde_guardar_f = va_arg(lista, float *);
                *donde_guardar_f = uart_atof(mi_buffer);
            }
          
        }
    }

    va_end(lista);
}*/