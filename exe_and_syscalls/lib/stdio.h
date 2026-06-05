#ifndef STDIO_H
#define STDIO_H

void PRINT(const char *format, ...);
void READ(const char *format, ...);
void trigger_svc_write(const char *s);
void mini_printf(const char *fmt, ...);
void uart_putnum_solo_digitos(unsigned int num);

#endif