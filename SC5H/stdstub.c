#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
/* std stubs */

static unsigned long heap_start;
static unsigned long heap_end;

#define UART			USART2
#define RCC_UART		RCC_USART2

void std_stub_init(void)
{
	/* usart2 */
	gpio_set_af(GPIOA, GPIO_AF4, GPIO2 | GPIO3);
	gpio_set_output_options(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_VERYHIGH,
			GPIO2 | GPIO3);
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2 | GPIO3);
	rcc_periph_clock_enable(RCC_UART);
	usart_enable(UART);
	usart_set_baudrate(UART, 115200);
	usart_set_databits(UART, 8);
	usart_set_stopbits(UART, USART_STOPBITS_1);
	usart_set_parity(UART, USART_PARITY_NONE);
	usart_set_flow_control(UART, USART_FLOWCONTROL_NONE);
	usart_set_mode(UART, USART_MODE_TX_RX);
}

int uart_read(void)
{
	if (!usart_is_recv_ready(UART))
		return -1;
	return usart_recv(UART);
}

int _write(int file, char *s, int len)
{
	int i;

	for (i = 0; i < len; ++i) {
		usart_send_blocking(UART, s[i]);
	}

	return i;
}

int putchar(int c)
{
	usart_send_blocking(UART, c);
	return c;
}

int puts(const char *s)
{
	int i;

	for (i = 0; *s; ++s, ++i)
		putchar(*s);

	return i;
}

int _read(int file, char *s, int len)
{
	memset(s, 0x00, len);
	return len;
}

int _close(int file)
{
	return 0;
}

int _fstat(int fd, void *unsued)
{
	return 0;
}

int _lseek(int fd, long offset, unsigned int whence)
{
	return 0;
}

int _isatty(int fd)
{
	return 0;
}

void *_sbrk(unsigned long inc)
{
	extern unsigned char _ebss[];
	void *last;

	if (heap_start == 0) {
		heap_start = (unsigned long)_ebss;
		heap_end = heap_start;
	}
	last = (void *)heap_end;
	heap_end += inc;
	return last;
}

int printf(const char *fmt, ...)
{
	va_list ap;
	char p[128];
	int len;

	va_start(ap, fmt);
	len = vsnprintf(p, sizeof(p), fmt, ap);
	va_end(ap);

	char *q;

	for (q = p; *q; ++q)
		usart_send_blocking(UART, *q);

	return len;
}

