#ifndef __ADC_C__
#define __ADC_C__

int adc_init(void);
uint8_t get_pcb_version(void);
void set_pcb_version(uint8_t version);
unsigned long adc_read(unsigned int channel);
unsigned long adc_read_I_5V(void);
unsigned long adc_read_I_VDD_12V(void);

#endif
