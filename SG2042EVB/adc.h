#ifndef __ADC_C__
#define __ADC_C__

int adc_init(void);
uint8_t get_pcb_version(void);
unsigned long adc_read(unsigned int channel);
unsigned long adc_read_I_DDR_VDD_0V8(void);
unsigned long adc_read_I_DDR01_VDDQ_1V2(void);
unsigned long adc_read_I_DDR23_VDDQ_1V2(void);
unsigned long adc_read_I_VQPS18(void);
unsigned long adc_read_I_VDD_PLL_0V8(void);
unsigned long adc_read_I_VDD_PCIE_D_0V8(void);
unsigned long adc_read_I_VDD_PCIE_C_0V8(void);
unsigned long adc_read_I_5V(void);
unsigned long adc_read_I_VDD_12V(void);
unsigned long adc_read_I_VDD_RGMII_1V8(void);
unsigned long adc_read_I_VDD_EMMC_1V8(void);
unsigned long adc_read_I_VDD_EMMC_3V3(void);
unsigned long adc_read_I_VDDC(void);
unsigned long adc_read_I_VDD_PCIE_H_1V8(void);
unsigned long adc_read_I_VDDIO18(void);

#endif
