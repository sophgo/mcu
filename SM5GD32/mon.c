#include <gd32e50x_adc.h>
#include <system.h>
#include <common.h>
#include <stdio.h>
#include <tick.h>
#include <project.h>
#include <stdarg.h>
#include <loop.h>
#include <timer.h>

static unsigned int pcb_ver;
static unsigned int bom_ver;

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
	186, 658, 1207, 1759, 2340
};

static int adc2ver(unsigned short adc)
{
	int i;
	for (i = 0; i < ARRAY_SIZE(version_table); ++i)
		if (adc < version_table[i])
			return i;
	return i;
}

void mon_init(void)
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

	adc_regular_channel_config(ADC0, 0, ADC_CHANNEL_0,
				   ADC_SAMPLETIME_55POINT5);
	adc_resolution_config(ADC0, ADC_RESOLUTION_12B);

	adc_external_trigger_config(ADC0, ADC_REGULAR_CHANNEL, ENABLE);
	adc_external_trigger_source_config(ADC0, ADC_REGULAR_CHANNEL,
					   ADC0_1_2_EXTTRIG_REGULAR_NONE);

	adc_enable(ADC0);
	timer_udelay(1);

	pcb_ver = adc2ver(adc_read(ADC_CHANNEL_0));
	bom_ver = adc2ver(adc_read(ADC_CHANNEL_1));
}

