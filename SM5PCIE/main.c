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
#include <common.h>
#include <led.h>
#include <timer.h>
#include <config.h>

struct i2c_slave_ctx i2c1_slave_ctx;

const char channel_labels[] = {
	CHANNEL_LABELS
};

int main(void)
{
	void *buf;
	/* skip control block and information block */
	unsigned long sector_offset = DATA_START_SECTOR;
	unsigned long round = 0;
	unsigned long limit_time, current_time;
	struct ctrl_block ctrl;
	struct info_block info;

	system_init();

	adc_setup();
	dma_setup();
	timer_setup(SAMPLE_RATE * CHANNEL_NUMBER);
	timer_start();

	if (sd_init()) {
		error("sd card init failed\r\n");
		return -1;
	}

	if (sd_read(&ctrl, 0, 1)) {
		error("read control block from sd card failed\r\n");
		return -1;
	}

	if (memcmp(ctrl.magic, MAGIC, sizeof(ctrl.magic))) {
		debug("invalid control block\r\n");
		limit_time = DEFAULT_ACQUIRE_TIME;
	} else
		limit_time = ctrl.time ? ctrl.time : DEFAULT_ACQUIRE_TIME;

	debug("acquire %lu seconds\r\n", (unsigned long)limit_time);

	info.channels = CHANNEL_NUMBER;
	info.sample_rate = SAMPLE_RATE;
	info.bits_per_sample = BITS_PER_SAMPLE;
	info.data_offset = DATA_START_SECTOR * SECTOR_SIZE;
	info.samples = 0;
	memcpy(info.channel_labels, channel_labels, sizeof(channel_labels));
	memset(info.channel_labels + sizeof(channel_labels), 0x00,
	       sizeof(info.channel_labels) - sizeof(channel_labels));

	while (1) {
		buf = dma_buffer_get();
		if (buf) {
			if (sd_write(buf, sector_offset,
				     SIG_BUF_SIZE / 2 / SECTOR_SIZE))
				error("sd card write error\r\n");
			dma_buffer_put(buf);

			sector_offset += SIG_BUF_SIZE / 2 / SECTOR_SIZE;
			++round;

			if (round == CHANNEL_NUMBER) {
				/* update information block */
				info.samples += SIG_BUF_SIZE / 2;
				current_time = info.samples / SAMPLE_RATE;
				debug("%lus %lu smp\r\n",
				      (unsigned long)current_time,
				      (unsigned long)info.samples);
				if (current_time > limit_time)
					break;
				if (sd_write(&info, 1, 1)) {
					error("write infomation block to sd card failed\r\n");
					return -1;
				}
				round = 0;
				led_working();
			}
		}
	}

	dma_destroy();
	adc_stop();

	tick_init(1000);

	info("\r\nBITMAIN SOPHONE SM5 PCIE BOARD -- %s\r\n", VERSION);
	info("%ld samples acquired\r\n", (unsigned long)info.samples);


	i2c1_slave_ctx.id = 1;
	i2c_master_init(I2C1);
	i2c_slave_init(&i2c1_slave_ctx, (void *)I2C1_BASE);
	i2c_slave_start(&i2c1_slave_ctx);

	while (1)
		;

	return 0;
}
