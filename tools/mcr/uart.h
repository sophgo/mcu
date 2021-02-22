#ifndef __UART_H__
#define __UART_H__

struct uart_ctx {
	int fd;
};

/**
 * open uart port with parameter 115200 8N1,
 * without hardware or software flow control
 *
 * @param ctx uart context
 * @param name name of uart port, on posix system, usually /dev/ttyUSBx
 *
 * @return 0 on success, or an error number is returned
 *
 * porting guide:
 *	if you neednot select uart port by name, you may ignore this parameter
 *	if uart hardware init sequence always success, 0 can be returned
 */
int uart_open(struct uart_ctx *ctx, char *name);

/**
 * try to get a character from uart port
 *
 * @param ctx uart context
 *
 * @return none negative number if a character is received. a negative number on
 * failure or no character in rx fifo.
 */
int uart_getc(struct uart_ctx *ctx);

/**
 * output a character though uart port
 *
 * @param ctx uart context
 * @param ch character to be send out
 *
 * @return character just be sent on success, a negative number on failure
 */
int uart_putc(struct uart_ctx *ctx, unsigned char ch);

/**
 * close uart port
 *
 * @param ctx uart context
 */
void uart_close(struct uart_ctx *ctx);

#endif
