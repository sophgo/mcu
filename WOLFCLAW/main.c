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
#include <i2c_master.h>
#include <tick.h>
#include <debug.h>
#include <pin.h>
#include <timer.h>
#include <ff.h>

sd_card_info_struct sd_cardinfo;

void nvic_config(void);
sd_error_enum sd_io_init(void);
void card_info_get(void);

int main(void)
{
	sd_error_enum sd_error;
	uint16_t i = 5;
	FATFS SDC_FS;
	FRESULT f_ret;
	int isl68224_addr = 0x60;

	system_init();
	led_flash(2);
	led_init();
	console_init();
	debug("\nHello WOLFCLAW User\n");
	debug("firmware build time:%s-%s\n", __DATE__, __TIME__);
	led_set_frequency(1);
	i2c_master_init(I2C1);

	nvic_config();
	/* initialize the card */
	do{
		sd_error = sd_io_init();
	}while((SD_OK != sd_error) && (--i));

	if(i){
		printf("\r\n Card init success!\r\n");
		led_2_0_on();
	}else{
		printf("\r\n Card init failed!\r\n");
		/* turn off LED2*/
		led_2_1_on();
		while (1){
			;
		}
	}

	f_ret = f_mount(&SDC_FS, "0:", 1);
	if(f_ret == FR_OK){
		printf("Card mount success!\r\n");
		led_2_0_on();
	} else{
		printf("Card mount failed!\r\n");
		led_2_1_on();
	}

	printf("68224 availiable nvm slots: %u\r\n", isl68224_get_nvm_slot_num(I2C1, isl68224_addr));

	isl68224_get_device_id(I2C1, isl68224_addr);
	isl68224_get_reversion_id(I2C1, isl68224_addr);

	while(1) {
		if(gpio_get(GPIOA, GPIO_PIN_8) == 0){
			timer_mdelay(200);
			if(gpio_get(GPIOA, GPIO_PIN_8) == 0){
				led_4_0_on();
				isl68224_get_reversion_id(I2C1, isl68224_addr);
				isl68224_program(I2C1, isl68224_addr, "0:BM1684X.hex");
			} else
				printf("press longer\r\n");
		}

		console_poll();
	}

	return 0;
}

sd_error_enum sd_io_init(void)
{
	sd_error_enum status = SD_OK;
	uint32_t cardstate = 0;
	status = sd_init();
	if(SD_OK == status){
		status = sd_card_information_get(&sd_cardinfo);
	}
	if(SD_OK == status){
		status = sd_card_select_deselect(sd_cardinfo.card_rca);
	}
	status = sd_cardstatus_get(&cardstate);
	if(cardstate & 0x02000000){
		dbg_printf("\r\n the card is locked!");
		while (1){
		}
	}
	if((SD_OK == status) && (!(cardstate & 0x02000000))){
		/* set bus mode */
		status = sd_bus_mode_config(SDIO_BUSMODE_4BIT);
//		status = sd_bus_mode_config( SDIO_BUSMODE_1BIT );
	}
	if(SD_OK == status){
		/* set data transfer mode */
//		status = sd_transfer_mode_config( SD_DMA_MODE );
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

void card_info_get(void)
{
	uint8_t sd_spec, sd_spec3, sd_spec4, sd_security;
	uint32_t block_count, block_size;
	uint16_t temp_ccc;
	printf("\r\n Card information:");
	sd_spec = (sd_scr[1] & 0x0F000000) >> 24;
	sd_spec3 = (sd_scr[1] & 0x00008000) >> 15;
	sd_spec4 = (sd_scr[1] & 0x00000400) >> 10;
	if(2 == sd_spec){
		if(1 == sd_spec3){
			if(1 == sd_spec4){
				printf("\r\n## Card version 4.xx ##");
			}else{
				printf("\r\n## Card version 3.0x ##");
			}
		}else{
			printf("\r\n## Card version 2.00 ##");
		}
	}else if(1 == sd_spec){
		printf("\r\n## Card version 1.10 ##");
	}else if(0 == sd_spec){
		printf("\r\n## Card version 1.0x ##");
	}

	sd_security = (sd_scr[1] & 0x00700000) >> 20;
	if(2 == sd_security){
		printf("\r\n## SDSC card ##");
	}else if(3 == sd_security){
		printf("\r\n## SDHC card ##");
	}else if(4 == sd_security){
		printf("\r\n## SDXC card ##");
	}

	block_count = (sd_cardinfo.card_csd.c_size + 1)*1024;
	block_size = 512;
	printf("\r\n## Device size is %dKB ##", (int)sd_card_capacity_get());
	printf("\r\n## Block size is %dB ##", (int)block_size);
	printf("\r\n## Block count is %d ##", (int)block_count);

	if(sd_cardinfo.card_csd.read_bl_partial){
		printf("\r\n## Partial blocks for read allowed ##" );
	}
	if(sd_cardinfo.card_csd.write_bl_partial){
		printf("\r\n## Partial blocks for write allowed ##" );
	}
	temp_ccc = sd_cardinfo.card_csd.ccc;
	printf("\r\n## CardCommandClasses is: %x ##", temp_ccc);
	if((SD_CCC_BLOCK_READ & temp_ccc) && (SD_CCC_BLOCK_WRITE & temp_ccc)){
		printf("\r\n## Block operation supported ##");
	}
	if(SD_CCC_ERASE & temp_ccc){
		printf("\r\n## Erase supported ##");
	}
	if(SD_CCC_WRITE_PROTECTION & temp_ccc){
		printf("\r\n## Write protection supported ##");
	}
	if(SD_CCC_LOCK_CARD & temp_ccc){
		printf("\r\n## Lock unlock supported ##");
	}
	if(SD_CCC_APPLICATION_SPECIFIC & temp_ccc){
		printf("\r\n## Application specific supported ##");
	}
	if(SD_CCC_IO_MODE & temp_ccc){
		printf("\r\n## I/O mode supported ##");
	}
	if(SD_CCC_SWITCH & temp_ccc){
		printf("\r\n## Switch function supported ##");
	}
}
