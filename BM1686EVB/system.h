#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include <libopencm3/stm32/usart.h>

#define DEFAULT_BAUDRATE	115200

#define USART2_BAUDRATE	DEFAULT_BAUDRATE

#define STD_UART	USART2

void system_init(void);
void clock_init(void);
int std_read(void);
void uart_putc(int c);
int uart_getc(void);
int uart_puts(const char *s);


#endif
