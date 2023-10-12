/*
 * system.h:
 *
 * (C) Copyright 2023 Sophgo Technology
 * Author: Donghao Zhao <donghao.zhao@sophgo.com>
 */

#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include <stdint.h>

#define DEFAULT_BAUD_RATE	115200
#define DEBUG_UART        	USART1

void system_init(void);
void system_adc_init(void);

void uart_putc(uint8_t ch);
int uart_getc(void);
int uart_puts(const char *s);
int dbg_printf(const char *fmt, ...);

#endif /* __SYSTEM_H__ */
