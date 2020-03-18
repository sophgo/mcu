/**
 * @file main.c
 * @author chao.wei
 * @brief application logical start
 */
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/dma.h>
#include <libopencm3/stm32/usart.h>
#include <stdio.h>
#include <tick.h>
#include <system.h>
#include <i2c_master.h>
#include <i2c_slave.h>
#include <info.h>
#include <mcu.h>
#include <debug.h>
#include <dma.h>
#include <string.h>
#include <sd.h>

struct i2c_slave_ctx i2c1_slave_ctx;

int adc_setup(void)
{
	uint8_t channels[] = {0, 1, 5, 6, 7};
	/* init adc and dma */
	adc_power_off(ADC1);
	adc_calibrate(ADC1);
	adc_enable_dma(ADC1);
	adc_enable_dma_circular_mode(ADC1);
	adc_set_continuous_conversion_mode(ADC1);
	adc_disable_discontinuous_mode(ADC1);
	adc_set_right_aligned(ADC1);
	adc_set_resolution(ADC1, 0);
	adc_set_regular_sequence(ADC1, sizeof(channels), channels);
	/* about 10us per-sample, we have 5way, so 50us per-round */
	adc_set_sample_time_on_all_channels(ADC1, ADC_SMPTIME_160DOT5);
	adc_power_on(ADC1);

	return 0;
}

void adc_start(void)
{
	adc_start_conversion_regular(ADC1);
}

int main(void)
{
	system_init();

	printf("BITMAIN SOPHONE SM5 PCIE BOARD -- %s\n", VERSION);

	i2c1_slave_ctx.id = 1;
	i2c_master_init(I2C1);
	i2c_slave_init(&i2c1_slave_ctx, (void *)I2C1_BASE);
	i2c_slave_start(&i2c1_slave_ctx);

	if (sd_init()) {
		error("sd card init failed\n");
		return -1;
	}
#ifdef DEBUG
	puts("uart test passed\n");

#if 0
	printf("system timer test\n");
	tick_test();
#endif
	printf("sd card self test %s\n",
	       sd_test() ? "failed" : "pass");
#if 0
	printf("sd card benchmark\n");
	sd_benchmark();
	printf("benchmark done\n");
#endif

#endif

	adc_setup();
	dma_setup();
	adc_start();

	while (1) {
	}

	return 0;
}
