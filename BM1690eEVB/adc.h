#ifndef __ADC_C__
#define __ADC_C__

int adc_init(void);
uint8_t get_pcb_version(void);
void set_pcb_version(uint8_t version);
uint8_t get_hardware_version(void);
unsigned long adc_read(unsigned int channel);
unsigned long adc_read_atx_12v_current(void);
unsigned long adc_read_pcie_3v3_current(void);
unsigned long adc_read_pcie_12v_current(void);
unsigned long adc_read_i12v(void);
unsigned int get_i12v_atx(void);
unsigned int get_i12v_pcie(void);
unsigned int get_i3v3_pcie(void);

#endif
