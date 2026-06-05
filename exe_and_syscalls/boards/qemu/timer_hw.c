#include "board.h"

void timer_hw_init(void) {
    // Cargar valor de interrupción (ajusta según el tickrate deseado)
    PUT32(TIMER_LOAD, 100000); 
    
    // Configurar Timer0: 
    // Bit 7: Enable, Bit 6: Periodic, Bit 5: IntEnable, Bit 1: 32-bit
    // 0xE2 = 1110 0010 en binario
    PUT32(TIMER_CTRL, 0xE2);

    // Habilitar la interrupción de Timer0/1 en el VIC (Bit 4 y 5)
    // El Timer0 de VersatilePB dispara la IRQ 4
    PUT32(VIC_INTENABLE, (1 << 4)); 
}

void timer_hw_ack(void) {
    // 1. Limpiar el Timer SP804
    PUT32(0x101E200C, 1);       
    
    // 2. Avisarle al VIC (Vectored Interrupt Controller) de VersatilePB
    // Escribir en VICVECTADDR (0x10140030) libera la interrupción.
    PUT32(0x10140030, 0); 
}