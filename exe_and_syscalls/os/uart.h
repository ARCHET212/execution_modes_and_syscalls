#ifndef UART_H
#define UART_H
void uart_putc(char c);
char uart_getc(void);
void uart_putnum(unsigned int num);
void os_write(const char *s);
void os_read(char *buffer, int max_length);
void uart_puthex(unsigned int num);

extern void uart_hw_putc(char c);
extern char uart_hw_getc(void);
#endif // UART_H