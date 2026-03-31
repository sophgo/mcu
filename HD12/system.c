#include <gd32f4xx_gpio.h>
#include <gd32f4xx_rcu.h>
#include <gd32f4xx_usart.h>
#include <i2c_master.h>
#include <adc.h>
#include <tick.h>
#include <stdio.h>
// #include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#include <system.h>
#include <stdarg.h>

static void system_uart_init(void)
{
	/*configure usart0 as link to chip0*/
	/* tx push-pull, rx floating */
	rcu_periph_clock_enable(RCU_USART0);
	gpio_af_set(GPIOA, GPIO_AF_7, GPIO_PIN_9);
	gpio_af_set(GPIOA, GPIO_AF_7, GPIO_PIN_10);

	gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_9);
	gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, GPIO_PIN_9);
	gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_10);
	gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, GPIO_PIN_10);

	 /* uart0 */
	usart_deinit(LINK_TO_BM1_UART);
	usart_baudrate_set(LINK_TO_BM1_UART, 115200);
	usart_parity_config(LINK_TO_BM1_UART, USART_PM_NONE);
	usart_word_length_set(LINK_TO_BM1_UART, USART_WL_8BIT);
	usart_stop_bit_set(LINK_TO_BM1_UART, USART_STB_1BIT);
	usart_transmit_config(LINK_TO_BM1_UART, USART_TRANSMIT_ENABLE);
	usart_receive_config(LINK_TO_BM1_UART, USART_RECEIVE_ENABLE);
	usart_enable(LINK_TO_BM1_UART);

	/*configure usart1 as debug uart*/
	/* tx push-pull, rx floating */
	rcu_periph_clock_enable(RCU_USART1);
	gpio_af_set(GPIOD, GPIO_AF_7, GPIO_PIN_5);
	gpio_af_set(GPIOD, GPIO_AF_7, GPIO_PIN_6);

	gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_5);
	gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, GPIO_PIN_5);
	gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_6);
	gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, GPIO_PIN_6);

	 /* uart1 setup for debug messages */
	usart_deinit(DEBUG_UART);
	usart_baudrate_set(DEBUG_UART, 115200);
	usart_parity_config(DEBUG_UART, USART_PM_NONE);
	usart_word_length_set(DEBUG_UART, USART_WL_8BIT);
	usart_stop_bit_set(DEBUG_UART, USART_STB_1BIT);
	usart_transmit_config(DEBUG_UART, USART_TRANSMIT_ENABLE);
	usart_receive_config(DEBUG_UART, USART_RECEIVE_ENABLE);
	usart_enable(DEBUG_UART);

	/*configure usart2 as resume to burn SN*/
	/* tx push-pull, rx floating */
	rcu_periph_clock_enable(RCU_USART2);
	gpio_af_set(GPIOC, GPIO_AF_7, GPIO_PIN_10);
	gpio_af_set(GPIOC, GPIO_AF_7, GPIO_PIN_11);

	gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_10);
	gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, GPIO_PIN_10);
	gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_11);
	gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, GPIO_PIN_11);

	 /* uart2 */
	usart_deinit(BURNING_UART);
	usart_baudrate_set(BURNING_UART, 115200);
	usart_parity_config(BURNING_UART, USART_PM_NONE);
	usart_word_length_set(BURNING_UART, USART_WL_8BIT);
	usart_stop_bit_set(BURNING_UART, USART_STB_1BIT);
	usart_transmit_config(BURNING_UART, USART_TRANSMIT_ENABLE);
	usart_receive_config(BURNING_UART, USART_RECEIVE_ENABLE);
	usart_enable(BURNING_UART);

}


static void system_gpio_init(void)
{
	uint32_t pins;

	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_GPIOB);
	rcu_periph_clock_enable(RCU_GPIOC);
	rcu_periph_clock_enable(RCU_GPIOD);
	rcu_periph_clock_enable(RCU_GPIOE);
	rcu_periph_clock_enable(RCU_GPIOF);

	/* -------------------------- GPIOA -------------------------- */
	/* GPIOA OUTPUT PINS */
	/* Module_EN should be pull up immediately, once the pin is ready to be controled by software */
	pins = GPIO_PIN_11;
	gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLDOWN, pins);
	gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, pins);
	gpio_bit_reset(GPIOA,pins);

	/* LED status */
	pins = GPIO_PIN_15;
	gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pins);
	gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, pins);

	/* GPIOA INTPUT PINS */
	pins = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
	// gpio_bit_set(GPIOA, pins);
	gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, pins);

	/* PG signal */
	pins = GPIO_PIN_12;
	gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, pins);
	gpio_bit_reset(GPIOA,pins);

	/* GPIOA ANALOG PINS */
	pins = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4;
	gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, pins);

	/* -------------------------- GPIOB -------------------------- */
	/* GPIOB OUTPUT PINS */
	pins = GPIO_PIN_0 | GPIO_PIN_1;
	gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLDOWN, pins);
	gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, pins);
	gpio_bit_reset(GPIOB,pins);
	
	pins = GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 |  GPIO_PIN_9 |
		GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
	gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLDOWN, pins);
	gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, pins);
	gpio_bit_reset(GPIOB, pins);

	/* GPIOB INTPUT PINS */
	pins = GPIO_PIN_2;
	gpio_mode_set(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_PULLDOWN, pins);

	/* -------------------------- GPIOC -------------------------- */
	/* GPIOC OUTPUT PINS */
	pins = GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
	gpio_mode_set(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLDOWN, pins);
	gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, pins);
	gpio_bit_reset(GPIOC, pins);
	
	/* GPIOC INPUT PINS */
	pins = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4;
	gpio_mode_set(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_PULLDOWN, pins);
	gpio_bit_set(GPIOC, pins);

	pins = GPIO_PIN_5;
	gpio_mode_set(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_NONE, pins);

	/* -------------------------- GPIOD -------------------------- */
	/* GPIOD OUTPUT PINS */
	pins = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_7 |
		GPIO_PIN_9 | GPIO_PIN_10;
	gpio_mode_set(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLDOWN, pins);
	gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, pins);
	gpio_bit_reset(GPIOD, pins);

	pins = GPIO_PIN_12 | GPIO_PIN_13;
	gpio_mode_set(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLDOWN, pins);
	gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, pins);
	gpio_bit_reset(GPIOD, pins);

	/* GPIOD INPUT PINS */
	pins = GPIO_PIN_15;
	gpio_mode_set(GPIOD, GPIO_MODE_INPUT, GPIO_PUPD_NONE, pins);

	/* -------------------------- GPIOE -------------------------- */
	/* GPIOE INPUT PINS */
	pins = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 |
		GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 |
		GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 |GPIO_PIN_14 | GPIO_PIN_15;
	gpio_mode_set(GPIOE, GPIO_MODE_INPUT, GPIO_PUPD_NONE, pins);
	gpio_bit_reset(GPIOE,pins);
}


static void system_i2c0_init(void)
{
	rcu_periph_clock_enable(RCU_I2C0);

	gpio_af_set(GPIOB, GPIO_AF_4, GPIO_PIN_6);
	gpio_af_set(GPIOB, GPIO_AF_4, GPIO_PIN_7);

	gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_6);
	gpio_output_options_set(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_25MHZ, GPIO_PIN_6);
	gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_7);
	gpio_output_options_set(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_25MHZ, GPIO_PIN_7);

	i2c_deinit(I2C0);
	i2c_master_init(I2C0);

}

static void system_i2c1_init(void)
{
	rcu_periph_clock_enable(RCU_I2C1);

	gpio_af_set(GPIOB, GPIO_AF_4, GPIO_PIN_10);
	gpio_af_set(GPIOB, GPIO_AF_4, GPIO_PIN_11);

	gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_10);
	gpio_output_options_set(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_25MHZ, GPIO_PIN_10);
	gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_11);
	gpio_output_options_set(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_25MHZ, GPIO_PIN_11);

	i2c_deinit(I2C1);
	i2c_master_init(I2C1);

}

static void system_i2c2_init(void)
{
	rcu_periph_clock_enable(RCU_I2C2);

	gpio_af_set(GPIOA, GPIO_AF_4, GPIO_PIN_8);
	gpio_af_set(GPIOC, GPIO_AF_4, GPIO_PIN_9);

	gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_8);
	gpio_output_options_set(GPIOA, GPIO_OTYPE_OD, GPIO_OSPEED_25MHZ, GPIO_PIN_8);
	gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_9);
	gpio_output_options_set(GPIOC, GPIO_OTYPE_OD, GPIO_OSPEED_25MHZ, GPIO_PIN_9);

	i2c_deinit(I2C2);
	i2c_master_init(I2C2);
}


static void system_timer_init(void)
{
	rcu_periph_clock_enable(RCU_TIMER6);
	rcu_periph_reset_enable(RCU_TIMER6RST);
	rcu_periph_reset_disable(RCU_TIMER6RST);

}

static void system_adc_init(void)
{
	/* apb clock is 180M, adc clock should less than 35M */
	/* adc working at 22.5M */
	adc_clock_config(ADC_ADCCK_PCLK2_DIV8);
	rcu_periph_clock_enable(RCU_ADC0);
	adc_deinit();
}


void system_init(void)
{
	system_gpio_init();
	system_uart_init();
	system_i2c0_init();
	system_i2c1_init();
	system_i2c2_init();
	system_timer_init();
	system_adc_init();
	adc_init();

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
/* for console put and get*/
int usart_is_recv_ready(uint32_t usart_periph)
{
	return USART_STAT0(usart_periph) & USART_STAT0_RBNE;
}

int uart_getc(void)
{
	if (usart_is_recv_ready(CONSOLE_UART))
		return usart_data_receive(CONSOLE_UART);
	return -1;
}

void uart_putc(uint8_t ch)
{
	usart_data_transmit(CONSOLE_UART, ch);
	while (!usart_flag_get(CONSOLE_UART, USART_FLAG_TBE))
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
/* printf uart*/
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


/* for xmode uart*/
int xmodem_uart_send(int data)
{
    usart_data_transmit(UPG_UART, data);
    return 0;
}

/* 0xffffffff means infinity */
int xmodem_uart_recv(unsigned long timeout)
{
    unsigned long tick_start = tick_get();
    do {
        if (usart_is_recv_ready(UPG_UART))
			return usart_data_receive(UPG_UART);
    } while (tick_get() - tick_start <= timeout || timeout == 0xffffffff);
    return -1;
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


// int uart2_getc(void)
// {
// 	if (usart_is_recv_ready(BURNING_UART))
// 		return usart_data_receive(BURNING_UART);
// 	return -1;
// }

// void uart2_putc(uint8_t ch)
// {
// 	usart_data_transmit(BURNING_UART, ch);
// 	while (!usart_flag_get(BURNING_UART, USART_FLAG_TBE))
// 		;
// }

// int uart2_puts(const char *s)
// {
// 	int i;

// 	for (i = 0; s[i]; ++i) {
// 		if (s[i] == '\n')
// 			uart2_putc('\r');
// 		uart2_putc(s[i]);
// 	}

// 	return i;
// }


static inline void debug_putc(int c)
{
	if (c == '\n') {
		usart_data_transmit(BURNING_UART, '\r');
		while (!usart_flag_get(BURNING_UART, USART_FLAG_TBE))
		;
	}

	usart_data_transmit(BURNING_UART, c);
	while (!usart_flag_get(BURNING_UART, USART_FLAG_TBE))
		;
}

int debug_printf(const char *fmt, ...)
{
	va_list ap;
	char p[128];
	int len;

	va_start(ap, fmt);
	len = vsnprintf(p, sizeof(p), fmt, ap);
	va_end(ap);

	char *q;

	for (q = p; *q; ++q)
		debug_putc(*q);

	return len;
}
