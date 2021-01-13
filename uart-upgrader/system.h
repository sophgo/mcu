#ifndef __SYSTEM_H__
#define __SYSTEM_H__

void system_init(void);
unsigned long tick_get(void);
void mdelay(unsigned long ms);
void system_set_stdout(int port, int rcc, int baudrate);
void uart_putc(int c);
int uart_getc(void);
int uart_puts(const char *s);
unsigned long system_get_flash_size(void);

#endif
