#include <mp5475.h>
#include <debug.h>
#include <pin.h>
#include <common.h>
#include <chip.h>
#include <timer.h>

/* PMIC OUTA */
int b1_vdd_1_8v_outa_on(void)
{
	/* add customer code here */
	mp5475_buck_on(MP5475B1, 0);
	return 0;
}

void b1_vdd_1_8v_outa_off(void)
{
	/* add customer code here */
	mp5475_buck_off(MP5475B1, 0);
}

int b2_vdd_1_8v_outa_on(void)
{
	/* add customer code here */
	mp5475_buck_on(MP5475B2, 0);
	return 0;
}

void b2_vdd_1_8v_outa_off(void)
{
	/* add customer code here */
	mp5475_buck_off(MP5475B2, 0);
}

int b3_vdd_1_8v_outa_on(void)
{
	/* add customer code here */
	mp5475_buck_on(MP5475B3, 0);
	return 0;
}

void b3_vdd_1_8v_outa_off(void)
{
	/* add customer code here */
	mp5475_buck_off(MP5475B3, 0);
}

/* PMIC OUTC */
int b1_ddr_vddq_outc_on(void)
{
	/* add customer code here */
	mp5475_buck_on(MP5475B1, 2);
	return 0;
}

void b1_ddr_vddq_outc_off(void)
{
	/* add customer code here */
	mp5475_buck_off(MP5475B1, 2);
}

int b2_ddr_vddq_outc_on(void)
{
	/* add customer code here */
	mp5475_buck_on(MP5475B2, 2);
	return 0;
}

void b2_ddr_vddq_outc_off(void)
{
	/* add customer code here */
	mp5475_buck_off(MP5475B2, 2);
}

int b3_ddr_vddq_outc_on(void)
{
	/* add customer code here */
	mp5475_buck_on(MP5475B3, 2);
	return 0;
}

void b3_ddr_vddq_outc_off(void)
{
	/* add customer code here */
	mp5475_buck_off(MP5475B3, 2);
}

/* PMIC OUTD */
int b1_vdd_phy_outd_on(void)
{
	/* add customer code here */
	mp5475_buck_on(MP5475B1, 3);
	return 0;
}

void b1_vdd_phy_outd_off(void)
{
	/* add customer code here */
	mp5475_buck_off(MP5475B1, 3);
}

int b2_vdd_phy_outd_on(void)
{
	/* add customer code here */
	mp5475_buck_on(MP5475B2, 3);
	return 0;
}

void b2_vdd_phy_outd_off(void)
{
	/* add customer code here */
	mp5475_buck_off(MP5475B2, 3);
}

int b3_vdd_phy_outd_on(void)
{
	/* add customer code here */
	mp5475_buck_on(MP5475B3, 3);
	return 0;
}

void b3_vdd_phy_outd_off(void)
{
	/* add customer code here */
	mp5475_buck_off(MP5475B3, 3);
}

/* PMIC OUTB */
int b1_ddr_vddqlp_outb_on(void)
{
	/* add customer code here */
	mp5475_buck_on(MP5475B1, 1);
	return 0;
}

void b1_ddr_vddqlp_outb_off(void)
{
	/* add customer code here */
	mp5475_buck_off(MP5475B1, 1);
}

int b2_ddr_vddqlp_outb_on(void)
{
	/* add customer code here */
	mp5475_buck_on(MP5475B2, 1);
	return 0;
}

void b2_ddr_vddqlp_outb_off(void)
{
	/* add customer code here */
	mp5475_buck_off(MP5475B2, 1);
}

int b3_ddr_vddqlp_outb_on(void)
{
	/* add customer code here */
	mp5475_buck_on(MP5475B3, 1);
	return 0;
}

void b3_ddr_vddqlp_outb_off(void)
{
	/* add customer code here */
	mp5475_buck_off(MP5475B3, 1);
}

int pmic_outa_on(void)
{
	/* add customer code here */
	b1_vdd_1_8v_outa_on();
	b2_vdd_1_8v_outa_on();
	b3_vdd_1_8v_outa_on();

	return 0;
}

void pmic_outa_off(void)
{
	/* add customer code here */
	b1_vdd_1_8v_outa_off();
	b2_vdd_1_8v_outa_off();
	b3_vdd_1_8v_outa_off();

}

int pmic_outd_on(void)
{
	/* add customer code here */
	b1_vdd_phy_outd_on();
	b2_vdd_phy_outd_on();
	b3_vdd_phy_outd_on();
	return 0;
}

void pmic_outd_off(void)
{
	/* add customer code here */
	b1_vdd_phy_outd_off();
	b2_vdd_phy_outd_off();
	b3_vdd_phy_outd_off();
}

int pmic_outc_on(void)
{
	/* add customer code here */
	b1_ddr_vddq_outc_on();
	b2_ddr_vddq_outc_on();
	b3_ddr_vddq_outc_on();
	return 0;
}

void pmic_outc_off(void)
{
	/* add customer code here */
	b1_ddr_vddq_outc_off();
	b2_ddr_vddq_outc_off();
	b3_ddr_vddq_outc_off();
}

int  pmic_outb_on(void)
{
	/* add customer code here */
	b1_ddr_vddqlp_outb_on();
	b2_ddr_vddqlp_outb_on();
	b3_ddr_vddqlp_outb_on();
	return 0;
}

void  pmic_outb_off(void)
{
	/* add customer code here */
	b1_ddr_vddqlp_outb_off();
	b2_ddr_vddqlp_outb_off();
	b3_ddr_vddqlp_outb_off();
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

	for (int i = 0; i < 90; i++)
		timer_udelay(1000);

	return 0;
}