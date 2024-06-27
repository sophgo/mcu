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

#define PCB_ADC_CHANNEL	0
#define BOM_ADC_CHANNEL	1
#define I12V_ADC_CHANNEL	4
#define NTCTMP_ADC_CHANNEL	6

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
static int ntc_tmp;

void set_hardware_version(unsigned int pcb_ver_val, unsigned int bom_ver_val)
{
	pcb_ver = pcb_ver_val;
	bom_ver = bom_ver_val;
}

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
	372, 745, 1117, 1489, 1862, 2234, 2607, 2979, 3351, 3724, 4096
};

const static unsigned short tmp_table[] = {
	28,31,34,38,42,46,50,55,60,66,72,78,85,92,100,108,117,126,136,
	147,158,170,183,196,210,225,241,258,276,295,315,336,358,381,405,
	430,456,483,511,540,571,603,636,670,705,741,778,817,857,898,940,
	983,1027,1072,1118,1165,1212,1260,1309,1358,1408,1458,1509,1560,
	1612,1664,1716,1768,1820,1872,1924,1976,2027,2078,2129,2179,2229,
	2278,2327,2375,2422,2469,2515,2560,2604,2647,2690,2732,2773,2813,
	2852,2890,2927,2963,2998,3033,3067,3100,3132,3163,3193,3222,3250,
	3277,3304,3330,3355,3379,3403,3426,3448,3469,3490,3510,3529,3548,
	3566,3583,3600,3616,3632,3647,3662,3676,3690,3703,3716,3728,3740,
	3752,3763,3774,3784,3794,3804,3813,3822,3831,3840,3848,3856,3864,
	3871,3878,3885,3892,3899,3905,3911,3917,3923,3928,3933,3938,3943,
	3948,3953,3958,3962,3966,3970,3974,3978,3982,3986,3989,3992,3995,
	3998,4001,4004,4007,4010
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
	/* step 1: real-voltage = 3.3 * adc / 2^12 /100
	 *	3.3v: stm32 vcc
	 *	100: 100 times op-amp
	 *	2^12: stm32 ADC resolution 12bit
	 * step 2: current = real-voltage / 0.005
	 *	0.005: 5mO resistor
	 * step 3: convert ampere to mili-ampere
	 */

	/*
	 *I_12V = (adc * 3.3) / (4096*100*0.005)
	 */
	return 3300UL * adc / 2048;
}

static void adc2tmp(unsigned short adc)
{
	int i = 0;
	for (i = 0; i < ARRAY_SIZE(tmp_table); ) {
		if (adc > tmp_table[i])
			i++;
		else
			break;
	}

	ntc_tmp = i - 50;
}

int get_ntc_tmp(void)
{
	return ntc_tmp;
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
	 * 4: 12V current
	 * 6: ntc monitor
	 */
	uint8_t channels[] = {PCB_ADC_CHANNEL, BOM_ADC_CHANNEL,\
	I12V_ADC_CHANNEL, NTCTMP_ADC_CHANNEL};

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
	adc_set_sample_time_on_all_channels(ADC1, ADC_SMPTIME_160DOT5);
	adc_power_on(ADC1);

	pcb_ver = adc2ver(adc_read());
	bom_ver = adc2ver(adc_read());

	last_time = tick_get();
	filter_init(&i12v, adc_read());

	adc2tmp(adc_read());

	/* donnot get version again */
	channels[0] = I12V_ADC_CHANNEL;
	adc_set_regular_sequence(ADC1, 1, channels);

	loop_add(mon_process);
}

