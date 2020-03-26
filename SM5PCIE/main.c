/**
 * @file main.c
 * @author chao.wei
 * @brief application logical start
 */
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>
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
#include <adc.h>
#include <string.h>
#include <sd.h>

struct i2c_slave_ctx i2c1_slave_ctx;

int main(void)
{
	void *buf;
	unsigned long sector_num = 0;
	unsigned long sector_offset = 0;
	unsigned long samples = 0;

	system_init();

	adc_setup();
	dma_setup();
	adc_start();

	if (sd_init()) {
		error("sd card init failed\r\n");
		return -1;
	}

	/* 256K samples will be acquired */
	while (samples < 256 * 1024) {

		buf = dma_buffer_get(&sector_num);
		if (buf) {
			if (sd_write(buf, sector_offset, sector_num))
				error("sd card write error\r\n");
			sector_offset += sector_num;
			samples += 1024;
			printf("%lu samples\r", samples);
			dma_buffer_put(buf);
		}
	}

	dma_destroy();
	adc_stop();

	printf("\r\nBITMAIN SOPHONE SM5 PCIE BOARD -- %s\r\n", VERSION);
	printf("%ld samples acquired, %ld sectors (%ld bytes) write\r\n",
			samples, sector_offset, sector_offset * 512);

	tick_init();

	i2c1_slave_ctx.id = 1;
	i2c_master_init(I2C1);
	i2c_slave_init(&i2c1_slave_ctx, (void *)I2C1_BASE);
	i2c_slave_start(&i2c1_slave_ctx);

	while (1)
		;

	return 0;
}
