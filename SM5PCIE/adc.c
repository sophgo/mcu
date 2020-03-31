#include <stdint.h>
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/rcc.h>

int adc_setup(void)
{
	rcc_periph_clock_enable(RCC_ADC1);
	uint8_t channels[] = {0, 1, 4, 5, 6, 7};
	/* init adc and dma */
	adc_power_off(ADC1);
	/* adc working at 16M clock, sync mode */
	adc_set_clk_source(ADC1, ADC_CLKSOURCE_PCLK_DIV2);
	/* apb clock 32M */
	adc_set_clk_prescale(ADC_CCR_PRESC_DIV1);

	adc_calibrate(ADC1);
	adc_enable_dma(ADC1);
	adc_enable_dma_circular_mode(ADC1);

	/* triggerd by timer */
	adc_set_single_conversion_mode(ADC1);
	adc_enable_discontinuous_mode(ADC1);
	/* triggerd by timer, so faster conversion */
	adc_set_sample_time_on_all_channels(ADC1, ADC_SMPTIME_001DOT5);
	/* ADC_CFGR1_EXTSEL_VAL set to 0 means accept trigger by TIM6 */
	adc_enable_external_trigger_regular(ADC1, ADC_CFGR1_EXTSEL_VAL(0),
					    ADC_CFGR1_EXTEN_RISING_EDGE);

	adc_set_right_aligned(ADC1);
	adc_set_resolution(ADC1, ADC_RESOLUTION_8BIT);
	adc_set_regular_sequence(ADC1, sizeof(channels), channels);
	adc_power_on(ADC1);
	/* wait timer trigger */
	adc_start_conversion_regular(ADC1);

	return 0;
}

void adc_stop(void)
{
	adc_power_off(ADC1);
}


