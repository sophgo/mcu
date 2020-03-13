/**
 * @file main.c
 * @author chao.wei
 * @brief application logical start
 */
#include <libopencm3/cm3/scb.h>
#include <libopencm3/cm3/vector.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/gpio.h>
#include <string.h>
#include <project_id.h>

/* software system tick in ms */
static volatile unsigned long tick;
static int stduart;
uint32_t project;

static inline void tick_init(void)
{
	systick_counter_disable();
	systick_clear();
	systick_set_frequency(1000, rcc_ahb_frequency);
	systick_interrupt_enable();
	systick_counter_enable();
}

unsigned long tick_get(void)
{
	return tick;
}

void mdelay(unsigned long ms)
{
	unsigned long cur = tick;
	/* make sure, wait time is larger than the given one */
	while (tick - cur <= ms)
		;
}

/* overwrite the default one */
void isr_systick(void)
{
	++tick;
}

void sc5h_init(void)
{
	/* usart1 */
	gpio_set_af(GPIOA, GPIO_AF6, GPIO2 | GPIO3);
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO2 | GPIO3);
	rcc_periph_clock_enable(RCC_USART1);
	usart_enable(USART1);
	usart_set_baudrate(USART1, 115200);
	usart_set_databits(USART1, 8);
	usart_set_stopbits(USART1, USART_STOPBITS_1);
	usart_set_parity(USART1, USART_PARITY_NONE);
	usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);
	usart_set_mode(USART1, USART_MODE_TX_RX);
	stduart = USART1;
}

struct proj_ops {
	uint32_t id;
	void (*init)(void);
};

struct proj_ops proj_ops_table[] = {
	{PROJ_SC5H, sc5h_init},
};

#define ARRAY_SIZE(array)	(sizeof(array) / sizeof((array)[0]))

void system_init(void)
{
	tick_init();
	struct proj_ops *proj_ops;
	int i;
	for (i = 0; i < ARRAY_SIZE(proj_ops_table); ++i) {
		proj_ops = proj_ops_table + i;
		if (proj_ops->id == project) {
			proj_ops->init();
		}
	}
}

static unsigned long heap_start;
static unsigned long heap_end;

int _write(int file, char *s, int len)
{
	int i;

	for (i = 0; i < len; ++i) {
		if (s[i] == '\n')
			usart_send_blocking(stduart, '\r');
		usart_send_blocking(stduart, s[i]);
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

extern unsigned char __ld_bss_end[];

void *_sbrk(unsigned long inc)
{
	void *last;

	if (heap_start == 0) {
		heap_start = (unsigned long)__ld_bss_end;
		heap_end = heap_start;
	}
	last = (void *)heap_end;
	heap_end += inc;
	return last;
}


int uart_send(int data)
{
    usart_send_blocking(USART1, data);
    return 0;
}

/* 0xffffffff means infinity */
int uart_recv(unsigned long timeout)
{
    unsigned long tick_start = tick_get();
    do {
        if (usart_is_recv_ready(USART1)) {
            return usart_recv(USART1);
        }
    } while (tick_get() - tick_start <= timeout || timeout == 0xffffffff);
    return -1;
}
