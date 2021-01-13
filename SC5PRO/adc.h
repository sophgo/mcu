#ifndef __ADC_C__
#define __ADC_C__

int adc_init(void);
unsigned long adc_read_i12v(void);
uint8_t adc_get_hw_ver(void);

#endif
