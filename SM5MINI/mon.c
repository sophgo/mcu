#include <libopencm3/stm32/adc.h>
#include <system.h>
#include <common.h>
#include <stdio.h>
#include <tick.h>
#include <project.h>
#include <stdarg.h>
#include <loop.h>

/* filter depth 256, acquire interval 10ms
 * so current value is an average of past (256 * 10)ms
 */

/* in ms */
#define ACQUIRE_INTERVAL	10

/* #define FILTER_DISABLE */

#define FILTER_DEPTH_SHIFT	8
#define FILTER_DEPTH		(1 << FILTER_DEPTH_SHIFT)
#define FILTER_DEPTH_MASK	(FILTER_DEPTH - 1)

struct filter {
	unsigned short data[FILTER_DEPTH];
	unsigned long total;
	unsigned long value;
	int p;
};

static unsigned long filter_init(struct filter *f, unsigned long d)
{
	int i;

	f->p = 0;
	f->total = 0;
	for (i = 0; i < FILTER_DEPTH; ++i) {
		f->data[i] = d;
		f->total += d;
	}
	return d;
}

static unsigned long filter_in(struct filter *f, unsigned long d)
{
	f->total -= f->data[f->p];
	f->total += d;
	f->data[f->p] = d;
	f->p = (f->p + 1) & FILTER_DEPTH_MASK;
	f->value = f->total >> FILTER_DEPTH_SHIFT;

#ifdef FILTER_DISABLE
	return d;
#else
	return f->value;
#endif
}

static struct filter i12v;
static unsigned int pcb_ver;
static unsigned int bom_ver;

uint16_t get_current(void)
{
	return i12v.value;
}

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

static unsigned long adc_read(void)
{
	ADC_CR(ADC1) |= ADC_CR_ADSTART;
	/* software triggered, busy status */
	while (ADC_CR(ADC1) & ADC_CR_ADSTART)
		;

	return ADC_DR(ADC1);
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

static uint16_t adc2current(unsigned short adc)
{
	/* step 1: real-voltage = 1.8 * adc / 2^12 /100
	 *	1.8v: stm32 vcc
	 *	100: 100 times op-amp
	 *	2^12: stm32 ADC resolution 12bit
	 * step 2: current = real-voltage / 0.003
	 *	0.003: 3mO resistor
	 * step 3: convert ampere to mili-ampere
	 */
	return 3000UL * adc / 2048;
}

static unsigned long current_time;
static unsigned long last_time;

void mon_process(void)
{
	current_time = tick_get();
	if (current_time - last_time >= ACQUIRE_INTERVAL) {
		filter_in(&i12v, adc2current(adc_read()));
		last_time = current_time;
	}
}

void mon_init(void)
{
	/* 0: PCB version
	 * 1: BOM version
	 * 5: 12V current
	 */
	uint8_t channels[] = {0, 1, 5};

	/* init adc and dma */
	adc_power_off(ADC1);
	adc_set_clk_source(ADC1, ADC_CFGR2_CKMODE_PCLK_DIV2);
	adc_calibrate(ADC1);
	adc_set_single_conversion_mode(ADC1);
	adc_enable_discontinuous_mode(ADC1);
	adc_set_right_aligned(ADC1);
	adc_set_resolution(ADC1, 0);
	adc_set_regular_sequence(ADC1, ARRAY_SIZE(channels), channels);
	/* set conversion speed to fastest */
	adc_set_sample_time_on_all_channels(ADC1, ADC_SMPTIME_001DOT5);
	adc_power_on(ADC1);

	pcb_ver = adc2ver(adc_read());
	bom_ver = adc2ver(adc_read());

	last_time = tick_get();
	filter_init(&i12v, adc_read());

	/* donnot get version again */
	adc_power_off(ADC1);
	channels[0] = 5;
	adc_set_regular_sequence(ADC1, 1, channels);
	adc_power_on(ADC1);

	loop_add(mon_process);
}

