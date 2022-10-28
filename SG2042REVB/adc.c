#include <gd32e50x_adc.h>
#include <system.h>
#include <common.h>
#include <stdio.h>
#include <tick.h>
#include <project.h>
#include <stdarg.h>
#include <timer.h>

#define I_DDR_VDD_0V8_CHANNEL	ADC_CHANNEL_0
#define I_DDR01_VDDQ_1V2_CHANNEL	ADC_CHANNEL_1
#define I_VDD_EMMC_1V8_CHANNEL	ADC_CHANNEL_2
#define I_VDD_EMMC_3V3_CHANNEL	ADC_CHANNEL_3
#define I_VDD_PLL_0V8_CHANNEL	ADC_CHANNEL_7
#define I_DDR01_VPP_2V5_CHANNEL	ADC_CHANNEL_8
#define I_DDR01_VTT_0V6_CHANNEL	ADC_CHANNEL_9
#define PCB_VRE0_CHANNEL	ADC_CHANNEL_10
#define BOM_VER0_CHANNEL	ADC_CHANNEL_11
#define I_VDD_12V_CHANNEL	ADC_CHANNEL_14
#define I_5V_CHANNEL		ADC_CHANNEL_15

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

/* averge value in 100ms */
static unsigned long adc_averge_read(unsigned int channel)
{
	int adc_read_time = 20;
	unsigned long adc_sum = 0U;

	for (int i = 0; i < adc_read_time; i++) {
		adc_sum += adc_read(channel);
		timer_udelay(500);
	}

	return (adc_sum / adc_read_time);
}

/* see config.xlsx, sheet adv2ver */
static unsigned short version_table[] = {
	267, 739, 1127, 1483, 1852, 2242, 2616, 2971, 3331, 3707
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
	bom_ver = adc2ver(adc_read(BOM_VER0_CHANNEL));
}

unsigned long adc_read_I_DDR_VDD_0V8(void)
{
	unsigned long adc_data = adc_averge_read(I_DDR_VDD_0V8_CHANNEL);

	return (adc_data * 160 / 4096);
}

unsigned long adc_read_I_DDR01_VDDQ_1V2(void)
{
	unsigned long adc_data = adc_averge_read(I_DDR01_VDDQ_1V2_CHANNEL);

	return (adc_data * 2400 / 4096);
}

unsigned long adc_read_I_VDD_EMMC_1V8(void)
{
	unsigned long adc_data = adc_averge_read(I_VDD_EMMC_1V8_CHANNEL);

	return (adc_data * 360 / 4096);
}

unsigned long adc_read_I_VDD_EMMC_3V3(void)
{
	unsigned long adc_data = adc_averge_read(I_VDD_EMMC_3V3_CHANNEL);

	return (adc_data * 660 / 4096);
}

unsigned long adc_read_I_VDD_PLL_0V8(void)
{
	unsigned long adc_data = adc_averge_read(I_VDD_PLL_0V8_CHANNEL);

	return (adc_data * 160 / 4096);
}

unsigned long adc_read_I_DDR01_VPP_2V5(void)
{
	unsigned long adc_data = adc_averge_read(I_DDR01_VPP_2V5_CHANNEL);

	return (adc_data * 500 / 4096);
}

unsigned long adc_read_I_DDR01_VTT_0V6(void)
{
	unsigned long adc_data = adc_averge_read(I_DDR01_VTT_0V6_CHANNEL);

	return (adc_data * 120 / 4096);
}
