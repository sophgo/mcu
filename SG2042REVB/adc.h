#ifndef __ADC_C__
#define __ADC_C__

int adc_init(void);
unsigned long adc_read(unsigned int channel);
unsigned long adc_read_i12v(void);
uint8_t get_hardware_version(void);
uint8_t get_pcb_version(void);
#endif
