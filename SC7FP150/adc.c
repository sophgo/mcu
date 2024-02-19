#include <gd32e50x_adc.h>
#include <system.h>
#include <common.h>
#include <stdio.h>
#include <tick.h>
#include <project.h>
#include <stdarg.h>
#include <timer.h>

#define CURRENT_PCIE_3V3_CHANNEL ADC_CHANNEL_13
#define CURRENT_SYS_12V_CHANNEL ADC_CHANNEL_0
#define CURRENT_PCIE_12V_CHANNEL ADC_CHANNEL_10
#define PROD_VER_CHANNEL ADC_CHANNEL_11
#define HW_VER_CHANNEL ADC_CHANNEL_9

static unsigned int pcb_ver;
static unsigned int bom_ver;

static unsigned int i12v_atx;
static unsigned int i12v_pcie;
static unsigned int i3v3_pcie;

uint8_t get_pcb_version(void)
{
	return pcb_ver;
}

uint8_t get_bom_version(void)
{
	return bom_ver;
}

uint8_t get_hardware_version(void)
{
	return (pcb_ver << 4) | bom_ver;
}

/* channel: ADC_CHANNEL_x */
static unsigned long adc_read(unsigned int channel)
{
	adc_regular_channel_config(ADC0, 0, channel, ADC_SAMPLETIME_55POINT5);

	adc_software_trigger_enable(ADC0, ADC_REGULAR_CHANNEL);

	while (ADC_CTL1(ADC0) & ADC_CTL1_SWRCST)
		;

	/* software triggered, busy status */
	while (!(ADC_STAT(ADC0) & ADC_STAT_EOC))
		;

	return adc_regular_data_read(ADC0);
}

/* see config.xlsx, sheet adv2ver */
const static unsigned short version_table[] = {
	0, 372, 793, 1310, 1658, 2048, 2458, 2731, 3121, 3571, 3901
};

static int adc2ver(unsigned short adc)
{
	int i;
	for (i = 0; i < ARRAY_SIZE(version_table); ++i)
		if (adc < version_table[i])
			return i;
	return i;
}

void adc_init(void)
{
	/* 0: PCB version
	 * 1: BOM version
	 */

	/* init adc and dma */
	adc_disable(ADC0);
	adc_enable(ADC0);

	/* adc need 14 adc clock to stable */
	/* wait 1us for safe */
	timer_udelay(1);

	adc_calibration_enable(ADC0);

	adc_disable(ADC0);

	adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);
	adc_mode_config(ADC_MODE_FREE);

	adc_regular_channel_config(ADC0, 0, HW_VER_CHANNEL,
				   ADC_SAMPLETIME_55POINT5);
	adc_resolution_config(ADC0, ADC_RESOLUTION_12B);

	adc_external_trigger_config(ADC0, ADC_REGULAR_CHANNEL, ENABLE);
	adc_external_trigger_source_config(ADC0, ADC_REGULAR_CHANNEL,
					   ADC0_1_2_EXTTRIG_REGULAR_NONE);

	adc_enable(ADC0);
	timer_udelay(1);

	pcb_ver = adc2ver(adc_read(HW_VER_CHANNEL));
	bom_ver = adc2ver(adc_read(PROD_VER_CHANNEL));
}

unsigned long adc_read_sys_i12v()
{
	unsigned long adc_data = adc_read(CURRENT_SYS_12V_CHANNEL);

	return (adc_data*36000/4096);
}
	/* adc-voltage = (adc-value / 2^12) * 1.8
	 * 100 times OP, and 0.3mO resistor
	 */
unsigned long adc_read_pcie_i12v()
{
	unsigned long adc_data = adc_read(CURRENT_PCIE_12V_CHANNEL);

	return (adc_data*6000/4096);
}

	/*
	 * I = (adc-value / 2^12) * 3.6A
	 * I = (adc-value / 2^12) * 3600mA
	 * p=3.3*I=12*(I/4)
	 */

unsigned long adc_read_pcie_i3v3()
{
	unsigned long adc_data = adc_read(CURRENT_PCIE_3V3_CHANNEL);

	return (adc_data*900/4096);
}

unsigned long adc_read_i12v(void)
{
	i12v_atx = adc_read_sys_i12v();
	i12v_pcie = adc_read_pcie_i12v();
	i3v3_pcie = adc_read_pcie_i3v3();

	return i12v_atx + i12v_pcie + i3v3_pcie;
}

unsigned int get_i12v_atx(void)
{
	return i12v_atx;
}

unsigned int get_i12v_pcie(void)
{
	return i12v_pcie;
}

unsigned int get_i3v3_pcie(void)
{
	return i3v3_pcie * 4;
}