#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#define DEFAULT_BAUDRATE	115200

#define LINK_TO_BM_UART USART0
#define DEBUG_UART USART1

#define ALL_UART LINK_TO_BM_UART

#define STD_UART	ALL_UART
#define MON_UART	ALL_UART
#define UPG_UART	ALL_UART
#define CONSOLE_UART	ALL_UART

void system_init(void);
void clock_init(void);

void uart_putc(uint8_t ch);
int uart_getc(void);
int uart_puts(const char *s);
int dbg_printf(const char *fmt, ...);
#endif
