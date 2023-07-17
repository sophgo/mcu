/* Sophgo Technology. Created by Donghao on 7/13/23. */


#ifndef __SYSTEM_H__
#define __SYSTEM_H__


#include <include.h>


#define DEFAULT_BAUD_RATE	115200

#define DEBUG_UART        	USART1


void evb_system_init(void);
void acp_system_init(void);
void system_init(void);
void system_i2c1_init(void);
void system_adc_init(void);

void uart_putc(uint8_t ch);
int uart_getc(void);
int uart_puts(const char *s);
int dbg_printf(const char *fmt, ...);


#endif /* __SYSTEM_H__ */
