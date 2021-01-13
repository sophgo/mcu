#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/rcc.h>
#include <stdint.h>
#include <common.h>

static uint8_t hw_ver;

#define ADC_CHANNEL_I12V	13
#define ADC_CHANNEL_HWVER	15

/* see config.xlsx, sheet adv2ver */
const static unsigned short version_table[] = {
	2417, 3051, 3520
};

static int adc2ver(unsigned short adc)
{
	int i;
	for (i = 0; i < ARRAY_SIZE(version_table); ++i)
		if (adc < version_table[i])
			return i;
	return i;
}

static unsigned long adc_read(void)
{
	/* channel 13 */
	ADC_CR(ADC1) |= ADC_CR_ADSTART;
	/* software triggered, busy status */
	while (ADC_CR(ADC1) & ADC_CR_ADSTART)
		;
	return ADC_DR(ADC1);
}

unsigned long adc_read_i12v(void)
{
	/* adc-voltage = (adc-value / 2^12) * 1.8
	 * 100 times OP, and 0.5mO resistor
	 */
	return (adc_read() * 180 * 1000) / (4096 * 5);
}

int adc_init(void)
{
	uint8_t channels[1];

	rcc_periph_clock_enable(RCC_ADC1);
	/* init adc and dma */
	adc_power_off(ADC1);
	adc_set_clk_source(ADC1, ADC_CFGR2_CKMODE_PCLK_DIV2);
	adc_calibrate(ADC1);
	adc_set_single_conversion_mode(ADC1);
	adc_enable_discontinuous_mode(ADC1);
	adc_set_right_aligned(ADC1);
	adc_set_resolution(ADC1, 0);
	channels[0] = ADC_CHANNEL_HWVER;
	adc_set_regular_sequence(ADC1, 1, channels);
	/* set conversion speed to fastest */
	adc_set_sample_time_on_all_channels(ADC1, ADC_SMPTIME_001DOT5);
	adc_power_on(ADC1);

	hw_ver = adc2ver(adc_read());

	/* donnot get version again */
	adc_power_off(ADC1);
	channels[0] = ADC_CHANNEL_I12V;
	adc_set_regular_sequence(ADC1, 1, channels);
	adc_power_on(ADC1);

	return 0;
}

uint8_t adc_get_hw_ver(void)
{
	return hw_ver;
}
