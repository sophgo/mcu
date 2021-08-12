#include <axp15060.h>
#include <chip.h>

int pmic_aldo_2_on(void)
{
	axp15060_channel_on(AXP15060_ALDO2);
	return 0;
}

void pmic_aldo_2_off(void)
{
	axp15060_channel_off(AXP15060_ALDO2);
}

int pmic_dcdc_6_on(void)
{
	axp15060_channel_on(AXP15060_DCDC6);
	return 0;
}

void pmic_dcdc_6_off(void)
{
	axp15060_channel_off(AXP15060_DCDC6);
}

int pmic_dcdc_5_on(void)
{
	axp15060_channel_on(AXP15060_DCDC5);
	return 0;
}

void pmic_dcdc_5_off(void)
{
	axp15060_channel_off(AXP15060_DCDC5);
}

int pmic_dcdc_1234_on(void)
{
	axp15060_channel_on(AXP15060_DCDC1);
	axp15060_channel_on(AXP15060_DCDC2);
	axp15060_channel_on(AXP15060_DCDC3);
	axp15060_channel_on(AXP15060_DCDC4);
	return 0;
}

void pmic_dcdc_1234_off(void)
{
	axp15060_channel_off(AXP15060_DCDC1);
	axp15060_channel_off(AXP15060_DCDC2);
	axp15060_channel_off(AXP15060_DCDC3);
	axp15060_channel_off(AXP15060_DCDC4);
}

int pmic_aldo_1_on(void)
{
	axp15060_channel_on(AXP15060_ALDO1);
	return 0;
}

void pmic_aldo_1_off(void)
{
	axp15060_channel_off(AXP15060_ALDO1);
}

int sys_rst_deassert_on(void)
{
	chip_enable();
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
	chip_disable();
}

