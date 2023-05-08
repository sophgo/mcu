#define DEBUG 1
#include <gd32e50x.h>
#include <gd32e50x_gpio.h>
#include <gd32e50x_misc.h>
#include <stdio.h>
#include <isl68224.h>
#include <console.h>
#include <common.h>
#include <system.h>
#include <sdcard.h>
#include <usbd.h>
#include <i2c_master.h>
#include <tick.h>
#include <debug.h>
#include <pin.h>
#include <timer.h>
#include <ff.h>

#define VALID_PRESS_TIME	3000

sd_card_info_struct sd_cardinfo;
void nvic_config(void);
sd_error_enum sd_io_init(void);

static unsigned long current_time, last_time;

int main(void)
{
	sd_error_enum sd_error;
	int i = 10;
	FATFS SDC_FS;
	FRESULT f_ret;

	debug("\nHello WOLFCLAW User\n");
	debug("firmware build time:%s-%s\n", __DATE__, __TIME__);

	system_init();
	led_flash(2);
	led_init();
	set_default_device(ISL68224);
	set_default_addr(SLAVE_DEFAULT_ADDR);
	set_default_file_name("0:firmware.hex");
	console_init();
	led_set_frequency(1);

usb_mode:
	/* detect the sd card */
	while (!is_sdcard_inserted())
	{
		timer_mdelay(1000);
		printf("please insert sdcard\n");
		console_poll();
	}
	
	// if (!is_sdcard_inserted()) {
	// 	printf("SDcard isn't inserted\n");
	// 	printf("In USB mode...\n");
	// 	usbd_progress();
	// }

	timer_mdelay(100);
	nvic_config();
	printf("SDcard inserted\n");
	printf("In SDcard mode...\n");

	/* Initialize sd card */
	do {
		sd_error = sd_io_init();
	} while ((SD_OK != sd_error) && (--i));

	if (i != 0) {
		printf("SDcard init success!\r\n");
		led_2_0_on();
	} else {
		printf("SDcard init failed!\r\n");
		led_2_1_on();

		while (1)
			console_poll();
	}

	/* To mount sd card */
	f_ret = f_mount(&SDC_FS, "0:", 1);
	if(f_ret == FR_OK){
		printf("SDcard mount success!\r\n");
		led_2_0_on();
	} else{
		printf("SDcard mount failed!\r\n");
		led_2_1_on();
	}
	led_3_0_on();

	current_time = last_time = tick_get();
	while(1) {
		if (is_key_pressed()) {
			current_time = tick_get();
			if (current_time - last_time >= VALID_PRESS_TIME) {
				led_4_0_on();
				timer_mdelay(1000);
				isl68224_program(get_default_filename());
				led_4_1_on();
				current_time = last_time = tick_get();
			}
		} else
			last_time = tick_get();

		if (!is_sdcard_inserted())
			goto usb_mode;
		console_poll();
	}

	return 0;
}

sd_error_enum sd_io_init(void)
{
	sd_error_enum status = SD_OK;
	uint32_t cardstate = 0;
	status = sd_init();
	if (SD_OK == status) {
		status = sd_card_information_get(&sd_cardinfo);
	}
	if (SD_OK == status) {
		status = sd_card_select_deselect(sd_cardinfo.card_rca);
	}
	status = sd_cardstatus_get(&cardstate);
	if (cardstate & 0x02000000) {
		printf("\r\n the card is locked!");
		while (1) {}
	}
	if ((SD_OK == status) && (!(cardstate & 0x02000000))) {
		/* set bus mode */
		status = sd_bus_mode_config(SDIO_BUSMODE_4BIT);
		// status = sd_bus_mode_config( SDIO_BUSMODE_1BIT );
	}
	if (SD_OK == status) {
		/* set data transfer mode */
		// status = sd_transfer_mode_config( SD_DMA_MODE );
		status = sd_transfer_mode_config( SD_POLLING_MODE );
	}
	return status;
}

void nvic_config(void)
{
	 nvic_priority_group_set(NVIC_PRIGROUP_PRE1_SUB3);
	 nvic_irq_enable(SDIO_IRQn, 0, 0);
}

void SDIO_IRQHandler(void)
{
    sd_interrupts_process();
}
