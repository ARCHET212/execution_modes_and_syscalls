#ifndef BOARD_H
#define BOARD_H 

// QEMU VersatilePB UART0 (PL011)
#define UART0_BASE     0x101F1000
#define UART_DR        (UART0_BASE + 0x00) // Data Register
#define UART_FR        (UART0_BASE + 0x18) // Flag Register
#define UART_FR_TXFF   (1 << 5)            // Transmit FIFO Full
#define UART_FR_RXFE   (1 << 4)            // Receive FIFO Empty

// QEMU VersatilePB Timer 0 (SP804)
#define TIMER0_BASE    0x101E2000
#define TIMER_LOAD     (TIMER0_BASE + 0x00)
#define TIMER_VALUE    (TIMER0_BASE + 0x04)
#define TIMER_CTRL     (TIMER0_BASE + 0x08)
#define TIMER_INTCLR   (TIMER0_BASE + 0x0C)

// QEMU VersatilePB VIC (Vectored Interrupt Controller)
#define VIC_BASE       0x10140000
#define VIC_INTENABLE  (VIC_BASE + 0x10)
#define VIC_VECTADDR   (VIC_BASE + 0x30)

// Memoria principal y Procesos en QEMU
#define KERNEL_START 0x00010000
#define KERNEL_STACK (KERNEL_START + 0x10000)

#define P1_START     0x00110000
#define P1_STACK     (P1_START + 0x10000)

#define P2_START     0x00210000
#define P2_STACK     (P2_START + 0x10000)

// Funciones de bajo nivel
void os_write(const char *s);
void os_read(char *buffer, int max_length);
void timer_init(void);
void timer_irq_handler(void);
void enable_irq(void);
void PUT32(unsigned int addr, unsigned int value);
unsigned int GET32(unsigned int addr);
void board_watchdog_disable(void);

#endif // BOARD_H