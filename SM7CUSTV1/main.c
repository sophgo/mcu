/**
 * @file main.c
 * @author chao.wei
 * @brief application logical start
 */
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>
#include <stdio.h>
#include <system.h>
#include <i2c_master.h>
#include <debug.h>
#include <chip.h>
#include <mp5475.h>
#include <pin.h>
#include <timer.h>
#include <mon.h>
#include <common.h>
#include <upgrade.h>
#include <mcu.h>
#include <pic.h>
#include <tca6416a.h>
#include <project.h>
#include <power.h>
//#include <se5.h>
#include <loop.h>
#include <keyboard.h>
#include <eeprom.h>
#include <wdt.h>
#include <pcie.h>
#include <slt.h>
#include <console.h>
#include <tmp451.h>
#include <rst_key.h>
#include <mon_print.h>
#include <sm7.h>

static struct i2c_slave_ctx i2c1_slave_ctx;
static struct i2c_slave_ctx i2c2_slave_ctx;

int main(void)
{
	clock_init();
	system_init();

	debug("\nBITMAIN SOPHON SM7CUSTV1\n");
	timer_udelay(50*1000);
	debug("firmware build time:%s-%s\n", __DATE__, __TIME__);
	
#ifndef STANDALONE
	if (get_stage() == RUN_STAGE_LOADER && check_app() == 0)
		app_start();
#endif

	led_init();
	
	i2c_master_init(I2C1);
	i2c_master_init(I2C2);
	timer_udelay(1000);

	pic_probe();

	timer_udelay(50*1000);
	power_init();
	gpio_set(EN_12V_SYS_PORT, EN_12V_SYS_PIN);
	timer_udelay(1000);
	mp5475_init();
	power_on();
	chip_init();

	tca6416a_probe();
	debug("tca6416a:%d \n",tca6416a_available());
	
	set_board_type(SM7CUSTV1);

	debug("%s %s working at ",
	      get_board_type_name(),
	      get_stage() == RUN_STAGE_LOADER ? "loader" : "application");
	if (get_work_mode() == WORK_MODE_SOC)
		debug("soc mode\n");
	else if (get_work_mode() == WORK_MODE_PCIE)
		debug("pcie mode\n");
	else if (get_work_mode() == WORK_MODE_MIXED)
		debug("mix mode\n");
	else
		debug("unkown mode\n");

	nvic_enable_irq(NVIC_I2C1_IRQ);
	i2c_slave_init(&i2c1_slave_ctx, (void *)I2C1_BASE,
		       I2C1_OA1, I2C1_OA2, I2C1_OA2_MASK);

	mon_init();
	mon_print_init();

	mcu_init(&i2c1_slave_ctx);
	eeprom_init(&i2c1_slave_ctx);
	wdt_init(&i2c1_slave_ctx);
	slt_init(&i2c1_slave_ctx);

	if (tca6416a_available())
		tca6416a_init(&i2c1_slave_ctx);

	kbd_init(&i2c1_slave_ctx);
	//if (pic_available()) {
	//	kbd_init(&i2c1_slave_ctx);
	//	pic_init(&i2c1_slave_ctx);
	//}

	tmp451_init(&i2c1_slave_ctx);

	/* start i2c slaves */
	i2c_slave_start(&i2c1_slave_ctx);

	if (get_work_mode() == WORK_MODE_SOC)
		chip_enable();
	else
		pcie_init();

	sm7_init();

	rst_key_init();
	console_add();
	/* never return */
	loop_start();

	return 0;
}

void i2c1_isr(void)
{
	i2c_slave_isr(&i2c1_slave_ctx);
}

void i2c2_isr(void)
{
	i2c_slave_isr(&i2c2_slave_ctx);
}
