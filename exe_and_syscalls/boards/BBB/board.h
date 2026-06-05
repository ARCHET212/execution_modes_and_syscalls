#ifndef BOARD_H
#define BOARD_H 

// BeagleBone Black UART0 base address
#define UART0_BASE     0x44E09000
#define UART_THR       (UART0_BASE + 0x00)  // Transmit Holding Register
#define UART_LSR       (UART0_BASE + 0x14)  // Line Status Register
#define UART_LSR_THRE  0x20                  // Transmit Holding Register Empty
#define UART_LSR_RXFE  0x10                  // Receive FIFO Empty

// BeagleBone Black DMTIMER2 base address
#define DMTIMER2_BASE    0x48040000
#define TCLR             (DMTIMER2_BASE + 0x38)  // Timer Control Register
#define TCRR             (DMTIMER2_BASE + 0x3C)  // Timer Counter Register
#define TISR             (DMTIMER2_BASE + 0x28)  // Timer Interrupt Status Register
#define TIER             (DMTIMER2_BASE + 0x2C)  // Timer Interrupt Enable Register
#define TLDR             (DMTIMER2_BASE + 0x40)  // Timer Load Register

// BeagleBone Black Interrupt Controller (INTCPS) base address
#define INTCPS_BASE      0x48200000
#define INTC_MIR_CLEAR2  (INTCPS_BASE + 0xC8)    // Interrupt Mask Clear Register 2
#define INTC_CONTROL     (INTCPS_BASE + 0x48)    // Interrupt Controller Control
#define INTC_ILR68       (INTCPS_BASE + 0x210)   // Interrupt Line Register 68

// Clock Manager base address
#define CM_PER_BASE      0x44E00000
#define CM_PER_TIMER2_CLKCTRL (CM_PER_BASE + 0x80)  // Timer2 Clock Control

#define KERNEL_START 0x82000000
#define KERNEL_STACK (KERNEL_START + 0x10000)  // + 64K de Stack

#define P1_START     0x82100000
#define P1_STACK     (P1_START + 0x10000)      // + 64K de Stack

#define P2_START     0x82200000
#define P2_STACK     (P2_START + 0x10000)      // + 64K de Stack

#define WDT_BASE       0x44E35000
#define WDT_WSPR        (WDT_BASE + 0x48)  // Watchdog Timer Write Posting Register
#define WDT_WWPS        (WDT_BASE + 0x34)  // Watchdog Timer Write Posting Status Register


// Low-level OS interface functions
void os_write(const char *s);
void os_read(char *buffer, int max_length);

// Timer functions
void timer_init(void);
void timer_irq_handler(void);

// Interrupt control
void enable_irq(void);

// Low-level memory access functions (implemented in root.s)
void PUT32(unsigned int addr, unsigned int value);
unsigned int GET32(unsigned int addr);
void board_watchdog_disable(void);



#endif //
