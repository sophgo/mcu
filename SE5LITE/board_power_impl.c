#include <mp5475.h>
#include <chip.h>
#include <common.h>
#include <timer.h>

int pmic_init_on(void)
{
	gpio_set(POWER_ON_PORT, POWER_ON_PIN);
	timer_mdelay(10);
	gpio_clear(DVDD_3P3V_EN_PORT, DVDD_3P3V_EN_PIN);
	return mp5475_init();
}

void pmic_init_off(void)
{
	gpio_set(DVDD_3P3V_EN_PORT, DVDD_3P3V_EN_PIN);
	gpio_clear(POWER_ON_PORT, POWER_ON_PIN);
}

int pmic_channel_a_on(void)
{
	return mp5475_buck_on(0);
}

void pmic_channel_a_off(void)
{
	mp5475_buck_off(0);
}

int pmic_channel_d_on(void)
{
	return mp5475_buck_on(3);
}

void pmic_channel_d_off(void)
{
	mp5475_buck_off(3);
}

int pmic_channel_b_on(void)
{
	return mp5475_buck_on(1);
}

void pmic_channel_b_off(void)
{
	mp5475_buck_off(1);
}

int pmic_channel_c_on(void)
{
	return mp5475_buck_on(2);
}

void pmic_channel_c_off(void)
{
	mp5475_buck_off(2);
}

int sys_rst_deassert_on(void)
{
	gpio_set(SYS_RST_PORT, SYS_RST_PIN);
	return 0;
}

void sys_rst_deassert_off(void)
{
}

int sys_rst_assert_on(void)
{
	chip_disable();
	return 0;
}

void sys_rst_assert_off(void)
{
	/* reset chip firstly when power off */
	chip_disable();
}

int u5v_on(void)
{
	gpio_clear(USB_5V_EN_PORT, USB_5V_EN_PIN);
	return 0;
}

void u5v_off(void)
{
	gpio_set(USB_5V_EN_PORT, USB_5V_EN_PIN);
}
