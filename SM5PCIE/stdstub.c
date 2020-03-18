#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>
#include <string.h>
/* std stubs */

static unsigned long heap_start;
static unsigned long heap_end;

static int uart;

int std_stub_init(int _uart)
{
	unsigned int rcc_uart;

	uart = _uart;

	switch (uart) {
	case USART1:
		rcc_uart = RCC_USART1;
		break;
	case USART2:
		rcc_uart = RCC_USART2;
		break;
	default:
		return -1;
	}

	rcc_periph_clock_enable(rcc_uart);
	usart_set_baudrate(uart, 115200);
	usart_set_databits(uart, 8);
	usart_set_stopbits(uart, USART_STOPBITS_1);
	usart_set_parity(uart, USART_PARITY_NONE);
	usart_set_flow_control(uart, USART_FLOWCONTROL_NONE);
	usart_set_mode(uart, USART_MODE_TX_RX);
	usart_enable(uart);
	return 0;
}

int _write(int file, char *s, int len)
{
	int i;

	for (i = 0; i < len; ++i) {
		if (s[i] == '\n')
			usart_send_blocking(uart, '\r');
		usart_send_blocking(uart, s[i]);
	}

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

int puts(const char *s)
{
	int i;

	for (i = 0; *s; ++s, ++i) {
		if (*s == '\n')
			usart_send_blocking(uart, '\r');
		usart_send_blocking(uart, *s);
	}
	return i;
}

