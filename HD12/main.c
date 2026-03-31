#define DEBUG
#include <gd32f4xx_gpio.h>
#include <system.h>
#include <pin.h>
#include <common.h>
#include <console.h>
#include <power.h>
#include <timer.h>
#include <debug.h>
#include <chip.h>
#include <tick.h>
#include <pca9848.h>
#include <ct7451.h>
#include <multiphase.h>
#include <mon.h>
#include <slave.h>
#include <aw95124.h>
#include <pcie.h>
#include <mcu.h>
#include <dvfs.h>
#include <ddr.h>

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

int main()
{
	/* init mcu itself, inlcuding gpio,adc,iic and uart*/
	system_init();

	/* set led on */
	led_init();

	dbg_printf("firmware build time:%s-%s\n", __DATE__, __TIME__);

	// check_clk_buffer();
	// check_host_pwrgd();

	/* set board power */
	board_power_init();

	pcie_init();

	check_gpio_power_good();
	/* pca9848 init */
	pca9848_init();

	/* CT7451 init */
	ct7451_init(0);
	ct7451_init(1);

	/* set process to collect basic info including temperature*/
	mon_init();

	// set module id for later use
	// module id is decided by the state of 4 pins
	set_module_id();

	/* set console for debug */
	console_init();
	// debug_check_vector_table();

	slave_init();
	dvfs_init();

	/* multiphase init, instead of isl68224*/
	//multiphase_init();

	ddr_size_init(DDR_SIZE_2R_128G);

	while(1) {
		if (chip_enable()) {
			mon_process();
		}

		ct7451_process();
		mdelay(1);
		mcu_process();
		console_poll();
		dvfs_process();
	}

	return 0;
}
