/*
 * adc.h:
 *
 * (C) Copyright 2023 Sophgo Technology
 * Author: Donghao Zhao <donghao.zhao@sophgo.com>
 */

#ifndef __ADC_H__
#define __ADC_H__

#include <common/common.h>

void adc_init(void);
unsigned long adc_read(unsigned int channel);

#endif /* __ADC_H__ */
