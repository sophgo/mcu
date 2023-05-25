#ifndef __ADC_C__
#define __ADC_C__

int adc_init(void);
uint8_t get_pcb_version(void);
void set_pcb_version(uint8_t version);
unsigned long adc_read(unsigned int channel);

#endif
