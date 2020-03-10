#ifndef __ADC_C__
#define __ADC_C__

int adc_init(void);
void adc_read(unsigned long *current, unsigned long *voltage);

#endif
