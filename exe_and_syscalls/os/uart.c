#include <stdarg.h>
#include "uart.h"

void uart_putc(char c) {
    uart_hw_putc(c); // El SO llama a la abstracción
}   

char uart_getc(void) {
    return uart_hw_getc();
}

// Helper function to print an unsigned integer
void uart_putnum(unsigned int num) {
    char buf[10];
    int i = 0;
    if (num == 0) {
        uart_putc('0');
        uart_putc('\n');
        return;
    }
    do {
        buf[i++] = (num % 10) + '0';
        num /= 10;
    } while (num > 0 && i < 10);
    while (i > 0) {
        uart_putc(buf[--i]);
    }
    uart_putc('\n');
}

// Function to send a string via UART
void os_write(const char *s) {
    while (*s) {
        uart_putc(*s++);
    }
}

// Function to receive a line of input via UART
void os_read(char *buffer, int max_length) {
    int i = 0;
    char c;
    while (i < max_length - 1) { // Leave space for null terminator
        c = uart_getc();
        if (c == '\n' || c == '\r') {
            uart_putc('\n'); // Echo newline
            break;
        }
        uart_putc(c); // Echo character
        buffer[i++] = c;
    }
    buffer[i] = '\0'; // Null terminate the string
}

void uart_puthex(unsigned int num) {
    os_write("0x");
    for (int i = 28; i >= 0; i -= 4) {
        int hex_val = (num >> i) & 0xF;
        if (hex_val < 10) {
            uart_putc(hex_val + '0');
        } else {
            uart_putc((hex_val - 10) + 'A');
        }
    }
}


// ... funciones uart_putc y os_write ...

void uart_putnum_solo_digitos(unsigned int num) {
    char buf[10];
    int i = 0;
    if (num == 0) {
        uart_putc('0');
        return;
    }
    do {
        buf[i++] = (num % 10) + '0';
        num /= 10;
    } while (num > 0 && i < 10);
    while (i > 0) {
        uart_putc(buf[--i]);
    }
    // QUITAMOS el uart_putc('\n') de aquí para que el \n del main funcione
}

void mini_printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    while (*fmt) {
        if (*fmt == '%') {
            fmt++;
            if (*fmt == 'd') {
                uart_putnum_solo_digitos(va_arg(args, int));
            } else if (*fmt == 'c') {
                uart_putc((char)va_arg(args, int));
            } else if (*fmt == 's') {
                os_write(va_arg(args, char *));
            }
        } else {
            uart_putc(*fmt);
        }
        fmt++;
    }
    va_end(args);
}