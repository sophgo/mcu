/**
 * @file main.c
 * @author chao.wei
 * @brief application logical start
 */
#include <libopencm3/cm3/scb.h>
#include <libopencm3/cm3/vector.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/flash.h>
#include <tick.h>
#include <stdstub.h>

static void clock_init(void)
{
	struct rcc_clock_scale clk = {
		.pll_mul = RCC_CFGR_PLLMUL_MUL4,
		.pll_div = RCC_CFGR_PLLDIV_DIV2,
		.pll_source = RCC_CFGR_PLLSRC_HSI16_CLK,
		.flash_waitstates = FLASH_ACR_LATENCY_1WS,
		.voltage_scale = PWR_SCALE1,
		.hpre = RCC_CFGR_HPRE_NODIV,
		.ppre1 = RCC_CFGR_PPRE1_NODIV,
		.ppre2 = RCC_CFGR_PPRE2_NODIV,
		.ahb_frequency = 32 * 1000 * 1000, /* 32M */
		.apb1_frequency = 32 * 1000 * 1000, /* 32M */
		.apb2_frequency = 32 * 1000 * 1000, /* 32M */
		.msi_range = 0, /* ignored by driver */
	};

	rcc_clock_setup_pll(&clk);
}

static void gpio_init(void)
{
	/* enable all gpio power and clock */
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
#if 0
	rcc_periph_clock_enable(RCC_GPIOC);
	rcc_periph_clock_enable(RCC_GPIOD);
	rcc_periph_clock_enable(RCC_GPIOH);
#endif
	/* GPIO port A */
	/* spi nss */
	gpio_set(GPIOA, GPIO15);
	/* gpio_set_af(GPIOA, GPIO_AF0, GPIO15); */
	gpio_set_output_options(GPIOA, GPIO_OTYPE_PP,
				GPIO_OSPEED_VERYHIGH, GPIO15);
	gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT,
			GPIO_PUPD_PULLUP, GPIO15);
	/* uart2 tx, rx */
#ifdef NUCLEO
	gpio_set_af(GPIOA, GPIO_AF4, GPIO9 | GPIO10);
	gpio_set_output_options(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_VERYHIGH,
			       GPIO9 | GPIO10);
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO9 | GPIO10);
#else
	gpio_set_af(GPIOA, GPIO_AF4, GPIO2 | GPIO3);
	gpio_set_output_options(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_VERYHIGH,
			       GPIO2 | GPIO3);
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO2 | GPIO3);
#endif
	/* adc 5 way */
	gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE,
			GPIO0 | GPIO1 | GPIO5 | GPIO6 | GPIO7);

	/* GPIO port B */
	/* spi clk, miso, mosi */
	gpio_set_af(GPIOB, GPIO_AF0, GPIO3 | GPIO4 | GPIO5);
	gpio_set_output_options(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_VERYHIGH,
			       GPIO3 | GPIO4 | GPIO5);
#if 1
	gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP,
			GPIO3 | GPIO4 | GPIO5);
#else
	gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE,
			GPIO3 | GPIO4 | GPIO5);
#endif

	/* i2c1 scl, sda */
	gpio_set_af(GPIOB, GPIO_AF4, GPIO8 | GPIO9);
	gpio_set_output_options(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_VERYHIGH,
			       GPIO8 | GPIO9);
	gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP,
			GPIO8 | GPIO9);
	/* led, sdoe */
	gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,
			GPIO0 | GPIO1 | GPIO2 | GPIO7);
#ifdef NUCLEO
	std_stub_init(USART1);
#else
	std_stub_init(USART2);
#endif
}

void system_init(void)
{
	/* we may load this to a place other than default reset address */
	/* so relocated vtor -- vector table offset register */
	SCB_VTOR = (uint32_t)&vector_table;
	clock_init();
	gpio_init();
}
