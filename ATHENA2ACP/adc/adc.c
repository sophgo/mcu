/*
 * adc.c:
 *
 * (C) Copyright 2023 Sophgo Technology
 * Author: Donghao Zhao <donghao.zhao@sophgo.com>
 */

#include <gd32e50x_adc.h>
#include <system/system.h>
#include <common/common.h>
#include <stdio.h>
#include <tick/tick.h>
#include <project.h>
#include <stdarg.h>
#include <timer/timer.h>
#include <adc/adc.h>
#include <debug.h>

#define PCB_VRE0_CHANNEL	ADC_CHANNEL_0
#define BOM_VRE0_CHANNEL	ADC_CHANNEL_1

const struct adc_nodes adc_node[ADC_NODE_SUM] = {
	{ADC_CHANNEL_4},	/* NTC1 */
	{ADC_CHANNEL_5},	/* NTC2 */
	{ADC_CHANNEL_10},	/* I_12V_SYS */
};

static uint8_t pcb_ver = -1;
static uint8_t bom_ver = -1;

uint8_t get_pcb_version(void)
{
	return pcb_ver;
}

void set_pcb_version(uint8_t version)
{
	pcb_ver = version;
}

uint8_t get_bom_version(void)
{
	return bom_ver;
}

/* channel: ADC_CHANNEL_x */
unsigned long adc_read(unsigned int channel)
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
static unsigned short pcb_bom_version_table[] = {
	0, 372, 745, 1117, 1489, 1862, 2234, 2607, 2979, 3351, 3724, 4096
};

static int pcb_bom_adc2ver(unsigned short adc)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(pcb_bom_version_table); ++i)
		if (adc < pcb_bom_version_table[i])
			return (i - 1);

	return 0;
}

void adc_init(void)
{
	/* init adc and dma */
	adc_disable(ADC0);
	adc_enable(ADC0);

	/* adc need 14 adc clock to stable */
	/* wait 1us for safe */
	timer_delay_us(1);

	adc_calibration_enable(ADC0);

	adc_disable(ADC0);

	adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);
	adc_mode_config(ADC_MODE_FREE);

	adc_regular_channel_config(ADC0, 0, PCB_VRE0_CHANNEL,
				   ADC_SAMPLETIME_55POINT5);
	adc_regular_channel_config(ADC0, 0, BOM_VRE0_CHANNEL,
				   ADC_SAMPLETIME_55POINT5);
	adc_resolution_config(ADC0, ADC_RESOLUTION_12B);

	adc_external_trigger_config(ADC0, ADC_REGULAR_CHANNEL, ENABLE);
	adc_external_trigger_source_config(ADC0, ADC_REGULAR_CHANNEL,
					   ADC0_1_2_EXTTRIG_REGULAR_NONE);

	adc_enable(ADC0);
	timer_delay_us(1);

	pcb_ver = pcb_bom_adc2ver(adc_read(PCB_VRE0_CHANNEL));
	bom_ver = pcb_bom_adc2ver(adc_read(BOM_VRE0_CHANNEL));
}
