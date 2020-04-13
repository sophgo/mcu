#ifndef __STDSTUB_H__
#define __STDSTUB_H__

int std_stub_init(int uart);
void uart_putc(int c);
int uart_getc(void);

#endif
