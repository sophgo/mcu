#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/rcc.h>

int adc_init(void)
{
	uint8_t channels[] = {13};

	rcc_periph_clock_enable(RCC_ADC1);
	/* init adc and dma */
	adc_power_off(ADC1);
	adc_set_clk_source(ADC1, ADC_CFGR2_CKMODE_PCLK_DIV2);
	adc_calibrate(ADC1);
	adc_set_single_conversion_mode(ADC1);
	adc_enable_discontinuous_mode(ADC1);
	adc_set_right_aligned(ADC1);
	adc_set_resolution(ADC1, 0);
	adc_set_regular_sequence(ADC1, sizeof(channels), channels);
	/* set conversion speed to fastest */
	adc_set_sample_time_on_all_channels(ADC1, ADC_SMPTIME_001DOT5);
	adc_power_on(ADC1);

	return 0;
}

void adc_read(unsigned long *current, unsigned long *voltage)
{
	/* channel 13 */
	ADC_CR(ADC1) |= ADC_CR_ADSTART;
	/* software triggered, busy status */
	while (ADC_CR(ADC1) & ADC_CR_ADSTART)
		;
	/* adc-voltage = (adc-value / 2^12) * 1.8
	 */
	*current = ADC_DR(ADC1);

	*voltage = 12 * 1000;
}

