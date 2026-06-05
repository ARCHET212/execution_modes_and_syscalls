#include "board.h"

void uart_hw_putc(char c) {
    // Esperar mientras la FIFO de transmisión esté llena
    while (GET32(UART_FR) & UART_FR_TXFF);
    PUT32(UART_DR, c);
}

char uart_hw_getc(void) {
    // Esperar mientras la FIFO de recepción esté vacía
    while (GET32(UART_FR) & UART_FR_RXFE);
    return (char)(GET32(UART_DR) & 0xFF);
}