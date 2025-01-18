#include <gd32e50x_adc.h>
#include <system.h>
#include <common.h>
#include <stdio.h>
#include <tick.h>
#include <project.h>
#include <stdarg.h>
#include <timer.h>
#include <debug.h>

#define I_5V_CHANNEL	ADC_CHANNEL_8
#define I_VDD_12V_CHANNEL	ADC_CHANNEL_9
#define PCB_VRE0_CHANNEL	ADC_CHANNEL_10

static unsigned int pcb_ver;

uint8_t get_pcb_version(void)
{
	return pcb_ver;
}

void set_pcb_version(uint8_t version)
{
	pcb_ver = version;
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
static unsigned short version_table[] = {
	50, 201, 364, 502, 645, 789, 921, 1066, 1220, 1414, 1586, 1829, 2053,\
	2284, 2464, 2608, 2781, 2923, 3088, 3343
};

static int adc2ver(unsigned short adc)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(version_table); ++i)
		if (adc < version_table[i])
			return i;

	return 0;
}

void adc_init(void)
{
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

	adc_regular_channel_config(ADC0, 0, PCB_VRE0_CHANNEL,
				   ADC_SAMPLETIME_55POINT5);
	adc_resolution_config(ADC0, ADC_RESOLUTION_12B);

	adc_external_trigger_config(ADC0, ADC_REGULAR_CHANNEL, ENABLE);
	adc_external_trigger_source_config(ADC0, ADC_REGULAR_CHANNEL,
					   ADC0_1_2_EXTTRIG_REGULAR_NONE);

	adc_enable(ADC0);
	timer_udelay(1);

	pcb_ver = adc2ver(adc_read(PCB_VRE0_CHANNEL));
}

/* I_12V= (ADC_Code* 3.3 * 1000) / (4096*100*0.0005) */
unsigned long adc_read_atx_12v_current(void)
{
	unsigned long adc_data = adc_read(I_VDD_12V_CHANNEL);

	return (adc_data * 66000 / 4096);	
}

/* I_3.3V = (ADC_Code * 3.3 * 1000) / (4096*100*0.0005) */
unsigned long adc_read_atx_5v_current(void)
{
	unsigned long adc_data = adc_read(I_5V_CHANNEL);

	return (adc_data * 66000 / 4096);
}
