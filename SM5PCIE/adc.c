#include <stdint.h>
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/rcc.h>

int adc_setup(void)
{
	rcc_periph_clock_enable(RCC_ADC1);
	uint8_t channels[] = {0, 1, 5, 6, 7};
	/* init adc and dma */
	adc_power_off(ADC1);
	adc_calibrate(ADC1);
	adc_enable_dma(ADC1);
	adc_enable_dma_circular_mode(ADC1);
	adc_set_continuous_conversion_mode(ADC1);
	adc_disable_discontinuous_mode(ADC1);
	adc_set_right_aligned(ADC1);
	adc_set_resolution(ADC1, ADC_RESOLUTION_8BIT);
	adc_set_regular_sequence(ADC1, sizeof(channels), channels);
	/* about 10us per-sample, we have 5way, so 50us per-round */
	adc_set_sample_time_on_all_channels(ADC1, ADC_SMPTIME_160DOT5);
	adc_power_on(ADC1);

	return 0;
}

void adc_start(void)
{
	adc_start_conversion_regular(ADC1);
}


