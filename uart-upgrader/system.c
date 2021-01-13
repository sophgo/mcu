/**
 * @file main.c
 * @author chao.wei
 * @brief application logical start
 */
#include <libopencm3/cm3/scb.h>
#include <libopencm3/cm3/vector.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/cm3/cortex.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/desig.h>
#include <string.h>
#include <project.h>
#include <stdarg.h>
#include <stdio.h>

/* software system tick in ms */
static volatile unsigned long tick;
static int uart_port;
static int uart_rcc;
static int uart_rate;

void system_set_stdout(int port, int rcc, int baudrate)
{
	uart_port = port;
	uart_rcc = rcc;
	uart_rate = baudrate;
}

unsigned long system_get_flash_size(void)
{
	return desig_get_flash_size() * 1024;
}

static inline void tick_init(void)
{
	systick_counter_disable();
	systick_clear();
	systick_set_frequency(1000, rcc_ahb_frequency);
	systick_interrupt_enable();
	systick_counter_enable();
	cm_enable_interrupts();
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

void clock_init(void)
{
	const unsigned long AHB_FREQ = 32 * 1000 * 1000;
	const unsigned long APB1_FREQ = AHB_FREQ;
	const unsigned long APB2_FREQ = AHB_FREQ;

	if (rcc_is_osc_ready(RCC_HSI16)) {
		/* we have set it up before */
		rcc_ahb_frequency = AHB_FREQ;
		rcc_apb1_frequency = APB1_FREQ;
		rcc_apb2_frequency = APB2_FREQ;
	} else {
		struct rcc_clock_scale clk = {
			.pll_mul = RCC_CFGR_PLLMUL_MUL4,
			.pll_div = RCC_CFGR_PLLDIV_DIV2,
			.pll_source = RCC_CFGR_PLLSRC_HSI16_CLK,
			.flash_waitstates = FLASH_ACR_LATENCY_1WS,
			.voltage_scale = PWR_SCALE1,
			.hpre = RCC_CFGR_HPRE_NODIV,
			.ppre1 = RCC_CFGR_PPRE1_NODIV,
			.ppre2 = RCC_CFGR_PPRE2_NODIV,
			.ahb_frequency = AHB_FREQ,
			.apb1_frequency = APB1_FREQ,
			.apb2_frequency = APB2_FREQ,
			.msi_range = 0, /* ignored by driver */
		};

		rcc_clock_setup_pll(&clk);
	}
}

void system_init(void)
{
	clock_init();

	project_init();

	rcc_periph_clock_enable(uart_rcc);
	usart_disable(uart_port);
	usart_enable(uart_port);

	usart_set_baudrate(uart_port, uart_rate);
	usart_set_databits(uart_port, 8);
	usart_set_stopbits(uart_port, USART_STOPBITS_1);
	usart_set_parity(uart_port, USART_PARITY_NONE);
	usart_set_flow_control(uart_port, USART_FLOWCONTROL_NONE);
	usart_set_mode(uart_port, USART_MODE_TX_RX);

	tick_init();
}

static unsigned long heap_start;
static unsigned long heap_end;

extern unsigned char __ld_bss_end[];

int putchar(int c)
{
	usart_send_blocking(uart_port, c);
	return c;
}

int puts(const char *s)
{
	int i;

	for (i = 0; *s; ++s, ++i)
		putchar(*s);

	return i;
}

int printf(const char *fmt, ...)
{
	va_list ap;
	char p[128], pch;
	int len;
	char *q;

	va_start(ap, fmt);
	len = vsnprintf(p, sizeof(p), fmt, ap);
	va_end(ap);

	pch = 0;

	for (q = p; *q; ++q) {
		/* insert \r to \n if we have not send \r out before \n */
		if (*q == '\n' && pch != '\r')
			putchar('\r');
		putchar(*q);
		pch = *q;
	}

	return len;
}


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


int xmodem_uart_send(int data)
{
    usart_send_blocking(uart_port, data);
    return 0;
}

/* 0xffffffff means infinity */
int xmodem_uart_recv(unsigned long timeout)
{
    unsigned long tick_start = tick_get();
    do {
        if (usart_is_recv_ready(uart_port)) {
            return usart_recv(uart_port);
        }
    } while (tick_get() - tick_start <= timeout || timeout == 0xffffffff);
    return -1;
}

void uart_putc(int c)
{
	usart_send_blocking(uart_port, c);
}

int uart_getc(void)
{
	if (usart_is_recv_ready(uart_port))
		return usart_recv(uart_port);
	return -1;
}

int uart_puts(const char *s)
{
	int i;

	for (i = 0; s[i]; ++i) {
		if (s[i] == '\n')
			usart_send_blocking(uart_port, '\r');
		usart_send_blocking(uart_port, s[i]);
	}

	return i;
}
