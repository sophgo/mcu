#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#define DEFAULT_BAUDRATE	115200

#define LINK_TO_BM_UART USART0
#define DEBUG_UART USART1

#define STD_UART	USART0
#define MON_UART	USART0
#define UPG_UART	USART0
#define CONSOLE_UART	USART0

void system_init(void);
void clock_init(void);

void uart_putc(uint8_t ch);
int uart_getc(void);
int uart_puts(const char *s);

#endif
