#include <mp5475.h>
#include <debug.h>
#include <pin.h>
#include <common.h>
#include <chip.h>
#include <timer.h>
int b12_vdd_1_8v_on(void)
{
	/* add customer code here */
	mp5475_buck_on(MP5475B12, 2);
	return 0;
}

void b12_vdd_1_8v_off(void)
{
	/* add customer code here */
	mp5475_buck_off(MP5475B12, 2);
}

int b34_vdd_1_8v_on(void)
{
	/* add customer code here */
	mp5475_buck_on(MP5475B34, 2);
	return 0;
}

void b34_vdd_1_8v_off(void)
{
	/* add customer code here */
	mp5475_buck_off(MP5475B34, 2);
}

int b56_vdd_1_8v_on(void)
{
	/* add customer code here */
	mp5475_buck_on(MP5475B56, 2);
	return 0;
}

void b56_vdd_1_8v_off(void)
{
	/* add customer code here */
	mp5475_buck_off(MP5475B56, 2);
}

int b78_vdd_1_8v_on(void)
{
	/* add customer code here */
	mp5475_buck_on(MP5475B78, 2);
	return 0;
}

void b78_vdd_1_8v_off(void)
{
	/* add customer code here */
	mp5475_buck_off(MP5475B78, 2);
}

int b12_vddq_outa_on(void)
{
	/* add customer code here */
	mp5475_buck_on(MP5475B12, 0);
	return 0;
}

void b12_vddq_outa_off(void)
{
	/* add customer code here */
	mp5475_buck_off(MP5475B12, 0);
}

int b34_vddq_outa_on(void)
{
	/* add customer code here */
	mp5475_buck_on(MP5475B34, 0);
	return 0;
}

void b34_vddq_outa_off(void)
{
	/* add customer code here */
	mp5475_buck_off(MP5475B34, 0);
}

int b56_vddq_outa_on(void)
{
	/* add customer code here */
	mp5475_buck_on(MP5475B56, 0);
	return 0;
}

void b56_vddq_outa_off(void)
{
	/* add customer code here */
	mp5475_buck_off(MP5475B56, 0);
}

int b78_vddq_outa_on(void)
{
	/* add customer code here */
	mp5475_buck_on(MP5475B78, 0);
	return 0;
}

void b78_vddq_outa_off(void)
{
	/* add customer code here */
	mp5475_buck_off(MP5475B78, 0);
}

int b12_vddq_outd_on(void)
{
	/* add customer code here */
	mp5475_buck_on(MP5475B12, 3);
	return 0;
}

void b12_vddq_outd_off(void)
{
	/* add customer code here */
	mp5475_buck_off(MP5475B12, 3);
}

int b34_vddq_outd_on(void)
{
	/* add customer code here */
	mp5475_buck_on(MP5475B34, 3);
	return 0;
}

void b34_vddq_outd_off(void)
{
	/* add customer code here */
	mp5475_buck_off(MP5475B34, 3);
}

int b56_vddq_outd_on(void)
{
	/* add customer code here */
	mp5475_buck_on(MP5475B56, 3);
	return 0;
}

void b56_vddq_outd_off(void)
{
	/* add customer code here */
	mp5475_buck_off(MP5475B56, 3);
}

int b78_vddq_outd_on(void)
{
	/* add customer code here */
	mp5475_buck_on(MP5475B78, 3);
	return 0;
}

void b78_vddq_outd_off(void)
{
	/* add customer code here */
	mp5475_buck_off(MP5475B78, 3);
}

int b12_vddqlp_on(void)
{
	/* add customer code here */
	mp5475_buck_on(MP5475B12, 1);
	return 0;
}

void b12_vddqlp_off(void)
{
	/* add customer code here */
	mp5475_buck_off(MP5475B12, 1);
}

int b34_vddqlp_on(void)
{
	/* add customer code here */
	mp5475_buck_on(MP5475B34, 1);
	return 0;
}

void b34_vddqlp_off(void)
{
	/* add customer code here */
	mp5475_buck_off(MP5475B34, 1);
}

int b56_vddqlp_on(void)
{
	/* add customer code here */
	mp5475_buck_on(MP5475B56, 1);
	return 0;
}

void b56_vddqlp_off(void)
{
	/* add customer code here */
	mp5475_buck_off(MP5475B56, 1);
}

int b78_vddqlp_on(void)
{
	/* add customer code here */
	mp5475_buck_on(MP5475B78, 1);
	return 0;
}

void b78_vddqlp_off(void)
{
	/* add customer code here */
	mp5475_buck_off(MP5475B78, 1);
}

int  pmic_outc_on(void)
{
	/* add customer code here */
	b12_vdd_1_8v_on();
	b34_vdd_1_8v_on();
	b56_vdd_1_8v_on();
	b78_vdd_1_8v_on();

	return 0;
}

void  pmic_outc_off(void)
{
	/* add customer code here */
	b12_vdd_1_8v_off();
	b34_vdd_1_8v_off();
	b56_vdd_1_8v_off();
	b78_vdd_1_8v_off();
}

int  pmic_outa_outd_on(void)
{
	/* add customer code here */
	b12_vddq_outa_on();
	b34_vddq_outa_on();
	b56_vddq_outa_on();
	b78_vddq_outa_on();
	b12_vddq_outd_on();
	b34_vddq_outd_on();
	b56_vddq_outd_on();
	b78_vddq_outd_on();
	return 0;
}

void  pmic_outa_outd_off(void)
{
	/* add customer code here */
	b12_vddq_outa_off();
	b34_vddq_outa_off();
	b56_vddq_outa_off();
	b78_vddq_outa_off();
	b12_vddq_outd_off();
	b34_vddq_outd_off();
	b56_vddq_outd_off();
	b78_vddq_outd_off();
}

int  pmic_outb_on(void)
{
	/* add customer code here */
	b12_vddqlp_on();
	b34_vddqlp_on();
	b56_vddqlp_on();
	b78_vddqlp_on();
	return 0;
}

void  pmic_outb_off(void)
{
	/* add customer code here */
	b12_vddqlp_off();
	b34_vddqlp_off();
	b56_vddqlp_off();
	b78_vddqlp_off();
}

int chip_deassert_n_on(void)
{
	sys_rst_disable();

	return 0;
}

int chip_assert_n_on(void)
{
	BN_SYS_RST_ENABLE(1);
	BN_SYS_RST_ENABLE(2);
	BN_SYS_RST_ENABLE(3);
	BN_SYS_RST_ENABLE(4);
	BN_SYS_RST_ENABLE(5);
	BN_SYS_RST_ENABLE(6);
	BN_SYS_RST_ENABLE(7);
	BN_SYS_RST_ENABLE(8);
	gpio_set(PCIE_SYS_RST_N_PORT, PCIE_SYS_RST_N_PIN);

	for (int i = 0; i < 90; i++)
		timer_udelay(1000);

	return 0;
}