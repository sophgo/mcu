/*
 * system.c:
 *
 * (C) Copyright 2023 Sophgo Technology
 * Author: Donghao Zhao <donghao.zhao@sophgo.com>
 */

#include <gd32e50x_usart.h>
#include <gd32e50x_rcu.h>
#include <gd32e50x_gpio.h>
#include <i2c/i2c_master/i2c_master.h>
#include <tick/tick.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#include <system/system.h>
#include <pin.h>
#include <stdarg.h>

static void system_uart_init(void)
{
	/* TX Push-Pull, RX Floating */
	gpio_init(MCU_USART_TX_PORT, GPIO_MODE_AF_PP, GPIO_OSPEED_10MHZ,
		  MCU_USART_TX_PIN);
	gpio_init(MCU_USART_RX_PORT, GPIO_MODE_IN_FLOATING,
		  GPIO_OSPEED_10MHZ, MCU_USART_RX_PIN);

	gpio_pin_remap_config(GPIO_USART1_REMAP, ENABLE);

	rcu_periph_clock_enable(RCU_USART1);

	/* UART1 Setup for Debug Messages */
	usart_deinit(DEBUG_UART);
	usart_baudrate_set(DEBUG_UART, DEFAULT_BAUD_RATE);
	usart_parity_config(DEBUG_UART, USART_PM_NONE);
	usart_word_length_set(DEBUG_UART, USART_WL_8BIT);
	usart_stop_bit_set(DEBUG_UART, USART_STB_1BIT);
	usart_transmit_config(DEBUG_UART, USART_TRANSMIT_ENABLE);
	usart_receive_config(DEBUG_UART, USART_RECEIVE_ENABLE);
	usart_enable(DEBUG_UART);
}

static void system_gpio_init(void)
{
	uint32_t pins;

	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_GPIOB);
	rcu_periph_clock_enable(RCU_GPIOC);
	rcu_periph_clock_enable(RCU_GPIOD);
	rcu_periph_clock_enable(RCU_GPIOE);
	rcu_periph_clock_enable(RCU_AF);

	/* JTAG disable, SWD enable, for some pin function */
	gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP, ENABLE);

	/* GPIOA */
	/* GPIOA Output PINs, setup output level first */
	pins = GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12;
	gpio_bit_reset(GPIOA, pins);
	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, pins);

	/* GPIOB */
	/* GPIOB Output PINs */
	pins = GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
	gpio_bit_reset(GPIOB, pins);
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, pins);
	/* GPIOB Input PINs */
	pins = GPIO_PIN_2 | GPIO_PIN_12;
	gpio_bit_reset(GPIOB, pins);
	gpio_init(GPIOB, GPIO_MODE_IPU, GPIO_OSPEED_2MHZ, pins);

	/* GPIOC */
	/* GPIOC Output PINs */
	pins = GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_13;
	gpio_bit_reset(GPIOC, pins);
	gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, pins);

	/* GPIOD */
	/* GPIOD Output PINs */
	pins = GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 |
		GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
	gpio_bit_reset(GPIOD, pins);
	gpio_init(GPIOD, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, pins);

	/* GPIOE */
	/* GPIOE Output PINs */
	pins = GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_9;
	gpio_bit_reset(GPIOE, pins);
	gpio_init(GPIOE, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, pins);
	/* GPIOE Input PINs */
	pins = GPIO_PIN_5;
	gpio_bit_reset(GPIOE, pins);
	gpio_init(GPIOE, GPIO_MODE_IPU, GPIO_OSPEED_2MHZ, pins);
}

static void system_i2c0_init(void)
{
	rcu_periph_clock_enable(RCU_I2C0);

	/* Don't remap */
	gpio_pin_remap_config(GPIO_I2C0_REMAP, DISABLE);

	gpio_init(GPIOB, GPIO_MODE_AF_OD, GPIO_OSPEED_10MHZ,
		  GPIO_PIN_6 | GPIO_PIN_7);

	i2c_deinit(I2C0);
	i2c_master_init(I2C0);
}

void system_i2c1_init(void)
{
	rcu_periph_clock_enable(RCU_I2C1);

	gpio_init(GPIOB, GPIO_MODE_AF_OD, GPIO_OSPEED_10MHZ,
		  GPIO_PIN_10 | GPIO_PIN_11);

	/* Magic deinit -- aka rcu reset, if not doing this, af pin can't work */
	i2c_deinit(I2C1);
	i2c_enable(I2C1);
}

static void system_timer_init(void)
{
	rcu_periph_clock_enable(RCU_TIMER6);
	rcu_periph_reset_enable(RCU_TIMER6RST);
	rcu_periph_reset_disable(RCU_TIMER6RST);
}

void system_init(void)
{
	system_gpio_init();
	system_uart_init();
	system_timer_init();
	/* I2C0 as Master */
	system_i2c0_init();
	/* I2C1 as Slave */
	system_i2c1_init();
	tick_init();
}

/* newlib stub */

static unsigned long heap_start;
static unsigned long heap_end;

extern char __ld_bss_end[0];

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

/* For DEBUG put and get */
int usart_is_recv_ready(uint32_t usart_periph)
{
	return USART_STAT0(usart_periph) & USART_STAT0_RBNE;
}

int uart_getc(void)
{
	if (usart_is_recv_ready(DEBUG_UART))
		return usart_data_receive(DEBUG_UART);
	return -1;
}

void uart_putc(uint8_t ch)
{
	usart_data_transmit(DEBUG_UART, ch);
	while (!usart_flag_get(DEBUG_UART, USART_FLAG_TBE))
		;
}

int uart_puts(const char *s)
{
	int i;

	for (i = 0; s[i]; ++i) {
		if (s[i] == '\n')
			uart_putc('\r');
		uart_putc(s[i]);
	}

	return i;
}

/* printf UART */
_ssize_t _write_r(struct _reent *ptr, int fd,
		  const void *buf, size_t cnt)
{
	size_t i;
	uint8_t ch;

	for (i = 0; i < cnt; ++i) {

		ch = ((uint8_t *)buf)[i];
		if (ch == '\n')
			uart_putc('\r');
		uart_putc(ch);
	}

	return cnt;
}

int _close_r(struct _reent *reent, int fd)
{
	return 0;
}

int _fstat_r(struct _reent *reent, int fd, struct stat *stat)
{
	if (fd == STDOUT_FILENO || fd == STDERR_FILENO) {
		memset(stat, 0, sizeof(struct stat));
		stat->st_mode = S_IFCHR;
		return 0;
	}

	errno = EBADF;

	return -errno;
}

int _isatty_r(struct _reent *reent, int fd)
{
	return 1;
}

off_t _lseek_r(struct _reent *reent, int fd, off_t offset, int pos)
{
	return 0;
}

_ssize_t _read_r(struct _reent *reent, int fd, void *buf, size_t len)
{
	return 0;
}

static inline void dbg_putc(int c)
{
	if (c == '\n') {
		usart_data_transmit(DEBUG_UART, '\r');
		while (!usart_flag_get(DEBUG_UART, USART_FLAG_TBE))
		;
	}

	usart_data_transmit(DEBUG_UART, c);
	while (!usart_flag_get(DEBUG_UART, USART_FLAG_TBE))
		;
}

int dbg_printf(const char *fmt, ...)
{
	va_list ap;
	char p[128];
	int len;

	va_start(ap, fmt);
	len = vsnprintf(p, sizeof(p), fmt, ap);
	va_end(ap);

	char *q;

	for (q = p; *q; ++q)
		dbg_putc(*q);

	return len;
}
