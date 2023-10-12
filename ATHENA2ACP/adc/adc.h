/*
 * adc.h:
 *
 * (C) Copyright 2023 Sophgo Technology
 * Author: Donghao Zhao <donghao.zhao@sophgo.com>
 */

#ifndef __ADC_H__
#define __ADC_H__

#include <common/common.h>
#include <chip/chip.h>

#define ADC_NODE_SUM	3

extern const struct adc_nodes adc_node[ADC_NODE_SUM];

void adc_init(void);
uint8_t get_pcb_version(void);
void set_pcb_version(uint8_t version);
uint8_t get_bom_version(void);
unsigned long adc_read(unsigned int channel);

#endif /* __ADC_H__ */
