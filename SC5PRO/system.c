/**
 * @file main.c
 * @author chao.wei
 * @brief application logical start
 */
#include <libopencm3/cm3/scb.h>
#include <libopencm3/cm3/vector.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <stdarg.h>
#include <stdio.h>
#include <tick.h>
#include <power.h>
#include <adc.h>
#include <timer.h>
#include <pin.h>
#include <chip.h>
#include <system.h>

#define AHB_FREQ	(32 * 1000 * 1000)
#define APB1_FREQ	AHB_FREQ
#define APB2_FREQ	AHB_FREQ

static unsigned long heap_start;
static unsigned long heap_end;

static void usart_init(uint32_t usart)
{
	usart_disable(usart);
	usart_set_baudrate(usart, DEFAULT_BAUDRATE);
	usart_set_databits(usart, 8);
	usart_set_stopbits(usart, USART_STOPBITS_1);
	usart_set_parity(usart, USART_PARITY_NONE);
	usart_set_flow_control(usart, USART_FLOWCONTROL_NONE);
	usart_disable_overrun_detection(usart);
	usart_set_mode(usart, USART_MODE_TX_RX);
	usart_enable(usart);
}

void system_init(void)
{
	/* we may load this to a place other than default reset address */
	/* so relocated vtor -- vector table offset register */
	SCB_VTOR = (uint32_t)&vector_table;

	/* enable all gpio power and clock */
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_GPIOC);
	rcc_periph_clock_enable(RCC_GPIOD);
	rcc_periph_clock_enable(RCC_GPIOE);
	rcc_periph_clock_enable(RCC_GPIOH);

	/* i2c1, i2c2 */
	rcc_periph_clock_enable(RCC_I2C1);
	rcc_periph_clock_enable(RCC_I2C2);
	rcc_periph_clock_enable(RCC_I2C3);

	/* usart 1, 2, 4 */
	rcc_periph_clock_enable(RCC_USART1);
	rcc_periph_clock_enable(RCC_USART2);
	rcc_periph_clock_enable(RCC_USART4);

	/* for external interrupts */
	/* enable PCIE RESET PIN interrupt */
	rcc_periph_clock_enable(RCC_SYSCFG);

	pin_init();

	/* TODO: remove it after refactor */
	/* for uart output */
	gpio_set(EN_B78_VDD_1_8V_PORT, EN_B78_VDD_1_8V_PIN);

	tick_init();
	timer_setup();
	adc_init();

	usart_init(USART1);
	usart_init(USART2);
	usart_init(USART4);
}

void clock_init(void)
{
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

int putchar(int c)
{
	if (c == '\n')
		usart_send_blocking(STD_UART, '\r');
	usart_send_blocking(STD_UART, c);
	return c;
}

int puts(const char *s)
{
	int i;

	for (i = 0; *s; ++s, ++i) {
		putchar(*s);
	}
	putchar('\n');

	return i;
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
		putchar(*q);

	return len;
}

void *_sbrk(unsigned long inc)
{
	void *last;

	if (heap_start == 0) {
		heap_start = (unsigned long)&_ebss;
		heap_end = heap_start;
	}
	last = (void *)heap_end;
	heap_end += inc;
	return last;
}

void uart_putc(int c)
{
	usart_send_blocking(UPG_UART, c);
}

int uart_getc(void)
{
	if (usart_is_recv_ready(UPG_UART))
		return usart_recv(UPG_UART);
	return -1;
}

int uart_puts(const char *s)
{
	int i;

	for (i = 0; s[i]; ++i) {
		if (s[i] == '\n')
			usart_send_blocking(UPG_UART, '\r');
		usart_send_blocking(UPG_UART, s[i]);
	}

	return i;
}
