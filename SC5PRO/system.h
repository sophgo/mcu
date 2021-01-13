#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include <libopencm3/stm32/usart.h>

#define DEFAULT_BAUDRATE	921600

#define USART1_BAUDRATE	DEFAULT_BAUDRATE
#define USART2_BAUDRATE	DEFAULT_BAUDRATE
#define USART4_BAUDRATE	DEFAULT_BAUDRATE

#define STD_UART	USART4
#define MON_UART	USART4
#define UPG_UART	USART4

void system_init(void);
void clock_init(void);

void uart_putc(int c);
int uart_getc(void);
int uart_puts(const char *s);

#endif
