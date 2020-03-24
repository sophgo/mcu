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

	system_init();

	/* set none buffered mode */
	// setvbuf(stdout, NULL, _IONBF, 0);

	debug("\r\nBITMAIN SOPHONE SM5 PCIE BOARD -- %s\r\n", VERSION);

	i2c1_slave_ctx.id = 1;
	i2c_master_init(I2C1);
	i2c_slave_init(&i2c1_slave_ctx, (void *)I2C1_BASE);
	i2c_slave_start(&i2c1_slave_ctx);

	if (sd_init()) {
		error("sd card init failed\r\n");
		return -1;
	}
#ifdef DEBUG
	puts("uart test passed\r\n");

#if 0
	debug("system timer test\r\n");
	tick_test();
#endif

#if 0
	debug("sd card self test %s\r\n",
	      sd_test() ? "failed" : "pass");
#endif

#if 0
	debug("sd card benchmark\r\n");
	sd_benchmark();
	debug("benchmark done\r\n");
#endif

#endif

	debug("adc setup done\r\n");
	adc_setup();
	debug("dma setup done\r\n");
	dma_setup();
	debug("adc start\r\n");
	adc_start();

	while (1) {

		buf = dma_buffer_get(&sector_num);
		if (buf) {
#if 1
			if (sd_write(buf, sector_offset, sector_num))
				error("sd card write error\r\n");
#endif
			sector_offset += sector_num;
			printf("%ld sectors write\r\n", sector_offset);
			dma_buffer_put(buf);
		}
		if (sector_offset >= 2048)
			break;
	}

	dma_destroy();

	printf("\r\n");

	debug("capture done\r\n");

	while (1)
		;

	return 0;
}
