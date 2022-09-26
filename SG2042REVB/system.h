#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#define DEFAULT_BAUDRATE		115200

#define DEBUG_UART		USART1
#define CONSOLE_UART		USART2


void system_init(void);
void clock_init(void);

void uart_putc(uint8_t ch);
int uart_getc(void);
int uart_puts(const char *s);
int dbg_printf(const char *fmt, ...);
#endif
