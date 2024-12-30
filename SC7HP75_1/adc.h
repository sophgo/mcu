#ifndef __ADC_C__
#define __ADC_C__

int adc_init(void);
unsigned long adc_read_i12v(void);
uint8_t get_hardware_version(void);
unsigned int get_i12v_atx(void);
unsigned int get_i12v_pcie(void);
unsigned int get_i3v3_pcie(void);
#endif
