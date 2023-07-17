/* Sophgo Technology. Created by Donghao on 7/13/23. */


#ifndef __ADC_H__
#define __ADC_H__


#include <common.h>
#include <include.h>
#include <chip.h>


#define ADC_NODE_SUM 3


extern struct adc_nodes const adc_node[ADC_NODE_SUM];


void adc_init(void);
uint8_t get_pcb_version(void);
void set_pcb_version(uint8_t version);
uint8_t get_bom_version(void);
uint8_t get_hardware_version(void);
unsigned long adc_read(unsigned int channel);


#endif /* __ADC_H__ */
