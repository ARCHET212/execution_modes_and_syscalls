#include "board.h"
void uart_hw_putc(char c) {
    while ((GET32(UART_LSR) & UART_LSR_THRE) == 0);
    PUT32(UART_THR, c);
}
char uart_hw_getc(void) {
    while ((GET32(UART_LSR) & UART_LSR_RXFE) != 0);
    return (char)(GET32(UART_THR) & 0xFF);
}