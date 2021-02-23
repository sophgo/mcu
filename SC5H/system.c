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
#include <libopencm3/stm32/usart.h>
#include <stdarg.h>
#include <stdio.h>
#include <tick.h>
#include <power.h>
#include <adc.h>
#include <timer.h>
#include <pin.h>
#include <led.h>
#include <chip.h>

#define UART			USART2
#define RCC_UART		RCC_USART2

#define AHB_FREQ	(32 * 1000 * 1000)
#define APB1_FREQ	AHB_FREQ
#define APB2_FREQ	AHB_FREQ

static unsigned long heap_start;
static unsigned long heap_end;

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
	rcc_periph_clock_enable(RCC_GPIOH);

	chip_init();
	power_init();

	/* i2c1, i2c2 */
	rcc_periph_clock_enable(RCC_I2C1);
	rcc_periph_clock_enable(RCC_I2C2);

	/* enable i2c1 interrupt */
	nvic_enable_irq(NVIC_I2C1_IRQ);

	tick_init();

	timer_setup();
	adc_init();

	/* mux i2c2 pin to PB10 -- SCL, PB11 -- SDA */
	/* mux i2c1 pin to PB8 -- SCL, PB9 -- SDA */
	gpio_set(GPIOB, GPIO8 | GPIO9 | GPIO10 | GPIO11);
	gpio_set_af(GPIOB, GPIO_AF6, GPIO10 | GPIO11);
	gpio_set_af(GPIOB, GPIO_AF4, GPIO8 | GPIO9);
	gpio_set_output_options(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_VERYHIGH,
			GPIO10 | GPIO11 | GPIO8 | GPIO9);
	gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE,
			GPIO10 | GPIO11 | GPIO8 | GPIO9);

	/* usart2 */
	gpio_set_af(GPIOA, GPIO_AF4, GPIO2 | GPIO3);
	gpio_set_output_options(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_VERYHIGH,
			GPIO2 | GPIO3);
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2 | GPIO3);
	rcc_periph_clock_enable(RCC_UART);
	usart_set_baudrate(UART, 921600);
	usart_set_databits(UART, 8);
	usart_set_stopbits(UART, USART_STOPBITS_1);
	usart_set_parity(UART, USART_PARITY_NONE);
	usart_set_flow_control(UART, USART_FLOWCONTROL_NONE);
	usart_set_mode(UART, USART_MODE_TX_RX);
	usart_disable_overrun_detection(UART);
	usart_enable(UART);

	/* init user led */
	led_init();
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

int uart_read(void)
{
	if (!usart_is_recv_ready(UART))
		return -1;
	return usart_recv(UART);
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
