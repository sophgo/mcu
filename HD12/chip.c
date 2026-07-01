#include <pin.h>
#include <tick.h>
#include <timer.h>
#include <debug.h>
#include <common.h>
#include <chip.h>
#include <system.h>

/* wait debug i2c ready */
#define CHIP_BOOT_TIME	1500

static volatile int is_chip_ready;
static volatile int is_chip_enabled;
static volatile int need_enable_chip;

static volatile int chip_vddr_status;

int chip_is_enabled(void)
{
	if (is_chip_enabled)
		return tick64_get() > CHIP_BOOT_TIME;
	else
		return false;
}

void chip_destroy(void)
{
	nvic_disable_irq(PCIE_RESET_NVIC);
	exti_interrupt_disable(PCIE_RESET_EXTI);
	exti_reset_request(PCIE_RESET_EXTI);
	timer_stop();
	is_chip_ready = 0;
}
void sys_rst_enable(void)
{
	/* PB0 is adc pin in this pcb, next pcb version will change other pin,
	 * so, now hw version is not work, becasuse this pin is used
	 * to control pcie switch prst
	 */
	gpio_bit_set(SYS_RST_X_H_BM0_PORT, SYS_RST_X_H_BM0_PIN);
	gpio_bit_set(SYS_RST_X_H_BM1_PORT, SYS_RST_X_H_BM1_PIN);

	/* count from 0 when first time enabled */

	if (!is_chip_enabled)
		tick64_set(0);

	is_chip_enabled = true;
}

void sys_rst_disable(void)
{
	is_chip_enabled = false;

	gpio_bit_reset(SYS_RST_X_H_BM0_PORT, SYS_RST_X_H_BM0_PIN);
	gpio_bit_reset(SYS_RST_X_H_BM1_PORT, SYS_RST_X_H_BM1_PIN);
}

int chip_enable(void)
{
	return is_chip_enabled;
}

void c2c_rst_enbale(void)
{
	gpio_bit_set(BM0_C2C2_L0_ERST_H_PORT, BM0_C2C2_L0_ERST_H_PIN);
	gpio_bit_set(BM1_C2C4_L0_ERST_H_PORT, BM1_C2C4_L0_ERST_H_PIN);
}

void set_vddr_wake_status(int chip, int flag)
{
	if (flag){
		if (chip == 0)
			gpio_bit_set(BM0_VDDR_WAKE_PORT, BM0_VDDR_WAKE_PIN);
		else
			gpio_bit_set(BM1_VDDR_WAKE_PORT, BM1_VDDR_WAKE_PIN);
	} else {
		if (chip == 0)
			gpio_bit_reset(BM0_VDDR_WAKE_PORT, BM0_VDDR_WAKE_PIN);
		else
			gpio_bit_reset(BM0_VDDR_WAKE_PORT, BM0_VDDR_WAKE_PIN);
	}

	chip_vddr_status &= 1 << chip;
}

int get_chip_vddr_status(int chip)
{
	return (chip_vddr_status & 1 << chip);
}

