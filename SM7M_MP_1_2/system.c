#include <gd32e50x_usart.h>
#include <gd32e50x_rcu.h>
#include <gd32e50x_gpio.h>
#include <i2c_master.h>
#include <tick.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/unistd.h>

static void system_uart_init(void)
{
	/* tx push-pull, rx floating */
	gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_2);
	gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_10MHZ, GPIO_PIN_3);

	rcu_periph_clock_enable(RCU_USART1);
	/* uart setup for debug messages */
	usart_deinit(USART1);
	usart_baudrate_set(USART1, 115200);
	usart_parity_config(USART1, USART_PM_NONE);
	usart_word_length_set(USART1, USART_WL_8BIT);
	usart_stop_bit_set(USART1, USART_STB_1BIT);
	usart_transmit_config(USART1, USART_TRANSMIT_ENABLE);
	usart_receive_config(USART1, USART_RECEIVE_ENABLE);
	usart_enable(USART1);
}

static void system_gpio_init(void)
{
	uint32_t pins;

	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_GPIOB);
	rcu_periph_clock_enable(RCU_GPIOC);
	// rcu_periph_clock_enable(RCU_GPIOD);
	rcu_periph_clock_enable(RCU_AF);

	/* JTAG disable, SWD enable, for some pin function */
	gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP, ENABLE);

	/* GPIOA */
	gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_2MHZ,
		  GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4);

	/* setup output level first */
	pins =  GPIO_PIN_7 |  GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11;
	gpio_bit_reset(GPIOA, pins);
	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, pins);
	/* setup input level first */
	pins =  GPIO_PIN_5 |  GPIO_PIN_6 | GPIO_PIN_12;
	gpio_init(GPIOA, GPIO_MODE_IPU, GPIO_OSPEED_2MHZ, pins);

	/* GPIOB */
	gpio_init(GPIOB, GPIO_MODE_IPU, GPIO_OSPEED_2MHZ,
		  GPIO_PIN_7 | GPIO_PIN_13 | GPIO_PIN_14);
	gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_2MHZ,GPIO_PIN_6);

	pins = GPIO_PIN_0;
	gpio_bit_reset(GPIOB, pins);
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, pins);

	pins = GPIO_PIN_1 | GPIO_PIN_2;
	gpio_bit_reset(GPIOB, pins);
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, pins);

	pins = GPIO_PIN_3;
	gpio_bit_reset(GPIOB, pins);
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, pins);

	pins = GPIO_PIN_5;
	gpio_bit_reset(GPIOB, pins);
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, pins);

	pins = GPIO_PIN_8;
	gpio_bit_reset(GPIOB, pins);
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, pins);

	pins = GPIO_PIN_15;
	gpio_bit_set(GPIOB, pins);
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, pins);

	/* GPIOC */
	pins = GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
	gpio_bit_reset(GPIOC, pins);
	gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, pins);
}

static void __attribute__((unused)) system_i2c0_init(void)
{
	rcu_periph_clock_enable(RCU_I2C0);

	gpio_pin_remap_config(GPIO_I2C0_REMAP, ENABLE);

	gpio_init(GPIOB, GPIO_MODE_AF_OD, GPIO_OSPEED_10MHZ,
		  GPIO_PIN_8 | GPIO_PIN_9);

	/* magic deinit -- aka rcu reset, if not do this, af pin cannot work */ 
	i2c_deinit(I2C0);

	i2c_master_init(I2C0);
}

static void system_i2c1_init(void)
{
	rcu_periph_clock_enable(RCU_I2C1);

	gpio_init(GPIOB, GPIO_MODE_AF_OD, GPIO_OSPEED_10MHZ,
		  GPIO_PIN_10 | GPIO_PIN_11);

	i2c_deinit(I2C1);

	i2c_master_init(I2C1);

	// asm volatile ("1: b 1b");
}

void system_i2c2_init(void)
{
	rcu_periph_clock_enable(RCU_I2C2);

	gpio_afio_port_config(AFIO_PB4_I2C2_CFG, ENABLE);
	gpio_afio_port_config(AFIO_PA8_I2C2_CFG, ENABLE);

	gpio_init(GPIOB, GPIO_MODE_AF_OD, GPIO_OSPEED_10MHZ, GPIO_PIN_4);
	gpio_init(GPIOA, GPIO_MODE_AF_OD, GPIO_OSPEED_10MHZ, GPIO_PIN_8);

	i2c_deinit(I2C2);
	i2c_enable(I2C2);
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
	rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV8);
	rcu_periph_clock_enable(RCU_ADC0);
	adc_deinit(ADC0);
}

void system_init(void)
{
	system_gpio_init();
	system_uart_init();
	//system_i2c0_init();
	system_i2c1_init();
	system_i2c2_init();
	system_timer_init();
	system_adc_init();

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

static inline void usart_putc(uint8_t ch)
{
	usart_data_transmit(USART1, ch);
	while (!usart_flag_get(USART1, USART_FLAG_TBE))
		;
}

_ssize_t _write_r(struct _reent *ptr, int fd,
		  const void *buf, size_t cnt)
{
	size_t i;
	uint8_t ch;

	for (i = 0; i < cnt; ++i) {

		ch = ((uint8_t *)buf)[i];
		if (ch == '\n')
			usart_putc('\r');
		usart_putc(ch);
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
