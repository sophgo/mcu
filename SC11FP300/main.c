#include <gd32e50x_gpio.h>
#include <gd32e50x_misc.h>
#include <tick.h>
#include <system.h>
#include <i2c_slave.h>
#include <debug.h>
#include <timer.h>
#include <power.h>
#include <pin.h>
#include <common.h>
#include <chip.h>
#include <mon.h>
#include <upgrade.h>
#include <project.h>
#include <pca9848.h>
#include <console.h>
#include <stdio.h>
#include <slave.h>
#include <pcie.h>
#include <ct7451.h>
#include <dbgi2c.h>
#include <isl68224.h>
#include <freq.h>
#include <mcu.h>
#include <dvfs.h>
#include <flash.h>

void HardFault_Handler(void)
{
	while (1)
		;
}
void NMI_Handler(void)
{
	while (1)
		;
}

int main(void)
{
	system_init();
	gpio_bit_set(CARD_PWR_PRES_PORT,CARD_PWR_PRES_PIN);
	gpio_bit_reset(EN_VDD_3V3_PORT, EN_VDD_3V3_PIN);
	gpio_bit_reset(SYS_RST_X_H_BM1_PORT, SYS_RST_X_H_BM1_PIN);
	gpio_bit_reset(SYS_RST_X_H_BM0_PORT, SYS_RST_X_H_BM0_PIN);
	led_init();

	//debug("\nBITMAIN SOPHONE SC11FP300\n");
	dbg_printf("firmware build time:%s-%s\n", __DATE__, __TIME__);
	dbg_printf("BITMAIN SOPHONE SC11FP300\n");
	timer_udelay(100);

#ifndef STANDALONE
	if (get_stage() == RUN_STAGE_LOADER)
		if (check_app() == 0)
			app_start();
#else
// #warn stand alone environment
#endif
	pca9848_init();
	ct7451_init(0);
	ct7451_init(1);
	board_power_init();
	mon_init();
	slave_init();
	pcie_init();
	dvfs_init();

	while (1) {
		if (chip_enable()) {
			mon_process();
		}

		ct7451_process();
		mcu_process();
		dvfs_process();
	}

	return 0;
}
