/*
 * system.c: CV84X6EVB 系统初始化
 *
 * (C) Copyright 2024 Sophgo Technology
 */
#include <gd32e50x_usart.h>
#include <gd32e50x_gpio.h>
#include <gd32e50x_rcu.h>
#include <gd32e50x_i2c.h>
#include <gd32e50x_misc.h>
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
#include <debug.h>

#define DEBUG_UART	USART2
#define DEFAULT_BAUD_RATE	115200

static void system_gpio_init(void)
{
	/* 使能 GPIO 时钟 */
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_GPIOB);
	rcu_periph_clock_enable(RCU_GPIOC);
	rcu_periph_clock_enable(RCU_GPIOD);
	rcu_periph_clock_enable(RCU_GPIOE);
	rcu_periph_clock_enable(RCU_AF);

	gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP, ENABLE);

	/* 输出引脚默认低电平 */
	gpio_bit_reset(PG_IND_PORT, PG_IND_PIN);
	gpio_bit_reset(PCIE_SEL_PORT, PCIE_SEL_PIN);
	gpio_bit_reset(EN_12V0_PORT, EN_12V0_PIN);
	gpio_bit_reset(POWEREN1_PORT, POWEREN1_PIN);
	gpio_bit_reset(POWEREN2_PORT, POWEREN2_PIN);
	gpio_bit_reset(POWEREN3_PORT, POWEREN3_PIN);
	gpio_bit_reset(SYS_RSTN_H_PORT, SYS_RSTN_H_PIN);
	gpio_bit_reset(PWR_RSTN_H_PORT, PWR_RSTN_H_PIN);
	gpio_bit_reset(PWR_BUTTON1_H_PORT, PWR_BUTTON1_H_PIN);
	gpio_bit_reset(PWR_ON_H_PORT, PWR_ON_H_PIN);
	gpio_bit_reset(PWR_WAKEUP_H_PORT, PWR_WAKEUP_H_PIN);
	gpio_bit_reset(PWM_VDDC_PORT, PWM_VDDC_PIN);

	/* LED 推挽输出 */
	gpio_init(PG_IND_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, PG_IND_PIN);

	/* GPIO 控制输出 */
	gpio_init(PCIE_SEL_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, PCIE_SEL_PIN);
	gpio_init(EN_12V0_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, EN_12V0_PIN);
	gpio_init(POWEREN1_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, POWEREN1_PIN);
	gpio_init(POWEREN2_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, POWEREN2_PIN);
	gpio_init(POWEREN3_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, POWEREN3_PIN);
	gpio_init(SYS_RSTN_H_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, SYS_RSTN_H_PIN);
	gpio_init(PWR_RSTN_H_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, PWR_RSTN_H_PIN);
	gpio_init(PWR_BUTTON1_H_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, PWR_BUTTON1_H_PIN);
	gpio_init(PWR_ON_H_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, PWR_ON_H_PIN);
	gpio_init(PWR_WAKEUP_H_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, PWR_WAKEUP_H_PIN);

	/* Power Good 检测引脚，输入上拉 */
	gpio_init(PG_VDD_CORE_PORT, GPIO_MODE_IPU, GPIO_OSPEED_2MHZ, PG_VDD_CORE_PIN);
	gpio_init(DDR_VDD_PG_PORT, GPIO_MODE_IPU, GPIO_OSPEED_2MHZ, DDR_VDD_PG_PIN);
	gpio_init(DDR_VDDQ_PG_PORT, GPIO_MODE_IPU, GPIO_OSPEED_2MHZ, DDR_VDDQ_PG_PIN);
	gpio_init(PG_5V_PORT, GPIO_MODE_IPU, GPIO_OSPEED_2MHZ, PG_5V_PIN);
}

static void clock_init(void)
{
	/* HXTAL 8MHz -> PLL 200MHz, SYSCLK = 200MHz, HCLK = 100MHz, APB1 = 50MHz, APB2 = 100MHz */
	rcu_osci_on(RCU_HXTAL);
	while (rcu_osci_stab_wait(RCU_HXTAL) != SUCCESS);

	rcu_pll_config(RCU_PLLSRC_HXTAL_IRC48M, RCU_PLL_MUL25);
	rcu_osci_on(RCU_PLL_CK);
	while (rcu_osci_stab_wait(RCU_PLL_CK) != SUCCESS);

	rcu_system_clock_source_config(RCU_CKSYSSRC_PLL);
	while (rcu_system_clock_source_get() != RCU_SCSS_PLL);

	rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV2);
	rcu_apb1_clock_config(RCU_APB1_CKAHB_DIV2);
	rcu_apb2_clock_config(RCU_APB2_CKAHB_DIV1);
}

static void system_uart_init(void)
{
	rcu_periph_clock_enable(RCU_USART2);
	rcu_periph_clock_enable(RCU_GPIOD);

	/* PD5 = USART2_TX, PD6 = USART2_RX */
	gpio_init(MCU_UART_TX_PORT, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ,
		  MCU_UART_TX_PIN);
	gpio_init(MCU_UART_RX_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ,
		  MCU_UART_RX_PIN);

	usart_deinit(DEBUG_UART);
	usart_baudrate_set(DEBUG_UART, DEFAULT_BAUD_RATE);
	usart_word_length_set(DEBUG_UART, USART_WL_8BIT);
	usart_stop_bit_set(DEBUG_UART, USART_STB_1BIT);
	usart_parity_config(DEBUG_UART, USART_PM_NONE);
	usart_hardware_flow_rts_config(DEBUG_UART, USART_RTS_DISABLE);
	usart_hardware_flow_cts_config(DEBUG_UART, USART_CTS_DISABLE);
	usart_receive_config(DEBUG_UART, USART_RECEIVE_ENABLE);
	usart_transmit_config(DEBUG_UART, USART_TRANSMIT_ENABLE);
	usart_enable(DEBUG_UART);
}

static void system_timer_init(void)
{
	rcu_periph_clock_enable(RCU_TIMER6);
	rcu_periph_reset_enable(RCU_TIMER6RST);
	rcu_periph_reset_disable(RCU_TIMER6RST);
}

void system_i2c0_init(void)
{
	rcu_periph_clock_enable(RCU_GPIOB);
	rcu_periph_clock_enable(RCU_I2C0);

	gpio_pin_remap_config(GPIO_I2C0_REMAP, DISABLE);

	gpio_init(GPIOB, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_6 | GPIO_PIN_7);

	i2c_deinit(I2C0);
	i2c_master_init(I2C0);
}

void system_i2c1_init(void)
{
	rcu_periph_clock_enable(RCU_GPIOB);
	rcu_periph_clock_enable(RCU_I2C1);

	gpio_init(GPIOB, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ,
		  GPIO_PIN_10 | GPIO_PIN_11);

	i2c_deinit(I2C1);
	i2c_enable(I2C1);
}

static int usart_is_recv_ready(uint32_t usart_periph)
{
	return USART_STAT0(usart_periph) & USART_STAT0_RBNE;
}

void uart_putc(char c)
{
	usart_data_transmit(DEBUG_UART, (uint8_t)c);
	while (!usart_flag_get(DEBUG_UART, USART_FLAG_TBE))
		;
}

char uart_getc(void)
{
	if (usart_is_recv_ready(DEBUG_UART))
		return (char)usart_data_receive(DEBUG_UART);
	return -1;
}

void system_init(void)
{
	clock_init();
	system_gpio_init();
	system_uart_init();
	system_timer_init();
	system_i2c0_init();
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

_ssize_t _write_r(struct _reent *ptr, int fd,
		  const void *buf, size_t cnt)
{
	size_t i;
	uint8_t ch;

	(void)ptr;
	(void)fd;

	for (i = 0; i < cnt; ++i) {
		ch = ((uint8_t *)buf)[i];
		if (ch == '\n')
			uart_putc('\r');
		uart_putc((char)ch);
	}

	return cnt;
}

int _close_r(struct _reent *reent, int fd)
{
	(void)reent;
	(void)fd;
	return 0;
}

int _fstat_r(struct _reent *reent, int fd, struct stat *stat)
{
	(void)reent;

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
	(void)reent;
	(void)fd;
	return 1;
}

off_t _lseek_r(struct _reent *reent, int fd, off_t offset, int pos)
{
	(void)reent;
	(void)fd;
	(void)offset;
	(void)pos;
	return 0;
}

_ssize_t _read_r(struct _reent *reent, int fd, void *buf, size_t len)
{
	(void)reent;
	(void)fd;
	(void)buf;
	(void)len;
	return 0;
}
