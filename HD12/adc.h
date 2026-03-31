#ifndef __ADC_C__
#define __ADC_C__

uint8_t get_pcb_ver(void);
uint8_t get_bom_ver(void);
uint8_t get_hardware_version(void);

void adc_init(void);
unsigned long adc_read_sys_i12v(void);
unsigned long adc_read_i12v(void);

#endif
