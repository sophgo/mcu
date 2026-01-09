#include <gd32f4xx_adc.h>
#include <system.h>
#include <common.h>
#include <timer.h>


#define CURRENT_SYS_12V_CHANNEL ADC_CHANNEL_0
#define CURRENT_PCIE_3V3_CHANNEL ADC_CHANNEL_1
#define CURRENT_PCIE_12V_CHANNEL ADC_CHANNEL_2
#define PROD_VER_CHANNEL ADC_CHANNEL_3
#define HW_VER_CHANNEL ADC_CHANNEL_4

static unsigned int pcb_ver;
static unsigned int bom_ver;
static unsigned int i12v_atx;
static unsigned int i12v_pcie;
static unsigned int i3v3_pcie;


uint8_t get_pcb_ver(void)
{
	return pcb_ver;
}

uint8_t get_bom_ver(void)
{
	return bom_ver;
}

uint8_t get_hardware_version(void)
{
	return (pcb_ver << 4) | bom_ver;
}

static unsigned long adc_read(unsigned int channel)
{
	adc_routine_channel_config(ADC0, 0, channel, ADC_SAMPLETIME_56);

	adc_software_trigger_enable(ADC0, ADC_ROUTINE_CHANNEL);

	while(ADC_CTL1(ADC0) & ADC_CTL1_SWRCST)
		;

	while(!(ADC_STAT(ADC0) & ADC_STAT_EOC))
		;

	return adc_routine_data_read(ADC0);
}

const static unsigned short version_table[] = {
	23, 819, 1229, 1638, 2048, 2458, 2867, 3277, 3686
};

static int adc2ver(unsigned short adc)
{
	int i;
	for(i = 0; i < ARRAY_SIZE(version_table); ++i){
		if(adc < version_table[i]){
			return i;
		}
	}

	return i;
}

void adc_init(void)
{
	adc_disable(ADC0);
	adc_enable(ADC0);

	timer_udelay(1);

	adc_calibration_enable(ADC0);

	adc_disable(ADC0);

	adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);
	adc_sync_mode_config(ADC_SYNC_MODE_INDEPENDENT);
	adc_routine_channel_config(ADC0, 0, HW_VER_CHANNEL, ADC_SAMPLETIME_56);
	adc_resolution_config(ADC0, ADC_RESOLUTION_12B);

	adc_external_trigger_config(ADC0, ADC_ROUTINE_CHANNEL, EXTERNAL_TRIGGER_DISABLE);

	adc_enable(ADC0);
	timer_udelay(1);

	pcb_ver = adc2ver(adc_read(HW_VER_CHANNEL));
	bom_ver = adc2ver(adc_read(PROD_VER_CHANNEL));
}

/* SYS_12V= (ADC_Code * 3.3 * 1000 ) / (4096*100*0.0005)
 * unit：mA
 */
unsigned long adc_read_sys_i12v()
{
	unsigned long adc_data = adc_read(CURRENT_SYS_12V_CHANNEL);

	return (adc_data * 66000 / 4096);
}

/* I_12V= (ADC_Code* 3.3 * 1000) / (4096*100*0.003) */
unsigned long adc_read_pcie_i12v()
{
	unsigned long adc_data = adc_read(CURRENT_PCIE_12V_CHANNEL);

	return (adc_data * 11000 / 4096);
}

/* I_3.3V = (ADC_Code * 3.3 * 1000) / (4096*100*0.005) */
unsigned long adc_read_pcie_i3v3()
{
	unsigned long adc_data = adc_read(CURRENT_PCIE_3V3_CHANNEL);

	return (adc_data * 1650 / 4096);
}

unsigned long adc_read_i12v(void)
{
	i12v_atx = adc_read_sys_i12v();
	i12v_pcie = adc_read_pcie_i12v();
	i3v3_pcie = adc_read_pcie_i3v3();

	return i12v_atx + i12v_pcie + i3v3_pcie;
}

unsigned int get_i12v_atx(void)
{
	return i12v_atx;
}

unsigned int get_i12v_pcie(void)
{
	return i12v_pcie;
}

/* pcie 3v3 real value*/
unsigned int get_i3v3_pcie(void)
{
	return i3v3_pcie * 4;
}
