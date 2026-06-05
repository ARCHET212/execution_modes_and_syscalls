#include "board.h"

void timer_hw_init(void) {
    PUT32(0x44E00508, 0x1); // Reloj 24MHz
    PUT32(CM_PER_TIMER2_CLKCTRL, 0x2);
    PUT32(INTC_MIR_CLEAR2, (1 << 4));
    PUT32(TCLR, 0);
    
    // Cambiamos a 0xFFFC5680 para interrupciones cada 10ms (100Hz)
    PUT32(TLDR, 0xFFD23940); 
    PUT32(TCRR, 0xFFD23940);
    
    PUT32(TIER, 0x2);
    PUT32(TCLR, 0x3);
}

void timer_hw_ack(void) {
    PUT32(TISR, 0x2);         // Limpiar Timer
    PUT32(INTC_CONTROL, 0x1); // Avisar al Interrupt Controller de BBB
}