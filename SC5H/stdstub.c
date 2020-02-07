#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>
#include <string.h>
/* std stubs */

static unsigned long heap_start;
static unsigned long heap_end;

void std_stub_init(void)
{
	/* usart1 */
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO2 | GPIO3);
	gpio_set_af(GPIOA, GPIO_AF6, GPIO2 | GPIO3);
	rcc_periph_clock_enable(RCC_USART1);
	usart_enable(USART1);
	usart_set_baudrate(USART1, 115200);
	usart_set_databits(USART1, 8);
	usart_set_stopbits(USART1, USART_STOPBITS_1);
	usart_set_parity(USART1, USART_PARITY_NONE);
	usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);
	usart_set_mode(USART1, USART_MODE_TX_RX);
}

int _write(int file, char *s, int len)
{
	int i;

	for (i = 0; i < len; ++i) {
		usart_send_blocking(USART1, s[i]);
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

