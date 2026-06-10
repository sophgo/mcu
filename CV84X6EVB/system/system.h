/*
 * system.h:
 */
#ifndef __SYSTEM_H__
#define __SYSTEM_H__

void system_init(void);
void system_uart_init(void);
void system_i2c0_init(void);
void system_i2c1_init(void);
void uart_putc(char c);
char uart_getc(void);

#endif /* __SYSTEM_H__ */
