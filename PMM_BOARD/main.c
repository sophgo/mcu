/*
 * main.c :
 *
 * (C) Copyright 2023 Sophgo Technology
 * Author: Donghao Zhao <donghao.zhao@sophgo.com>
 */

#include <gd32e50x_gpio.h>
#include <gd32e50x_misc.h>
#include <tick/tick.h>
#include <system/system.h>
#include <timer/timer.h>
#include <pin.h>
#include <common/common.h>
#include <project.h>
#include <stdio.h>
#include <debug.h>
#include <adc.h>

/* Uncomment DEBUG_HUMAN if you want human-readable debug print */
//#define DEBUG_HUMAN
#define V_ADC_GND		20	/* Offset ADC value for GND */
#define V_ADC_PC		4.7	/* Offset ADC value for PC */
#define SCALAR_THEORETICAL	100	/* Theoretical amplifier factor */

int main(void)
{
	unsigned short adc_data[16], upload_data;
	float upload_data_float;
	system_init();

	while (1) {
		/* The start frame, higher 8 bits then lower 8 bits */
		uart_putc((uint8_t)(0xfeff >> 8));
		uart_putc((uint8_t)(0xfeff & 0x00ff));

		/* Data acquired from all 16 ADC channels */
		/* ADC 0 */
		adc_data[0] = (unsigned short)adc_read((unsigned long)0);
		upload_data_float = (float)adc_data[0] - V_ADC_GND - V_ADC_PC;
		upload_data_float = upload_data_float;
		upload_data = upload_data_float / 1;
#ifndef DEBUG_HUMAN
		uart_putc((uint8_t)(upload_data >> 8));
		uart_putc((uint8_t)(upload_data & 0x00ff));
#endif
#ifdef DEBUG_HUMAN
		printf("ADC[%d]: [Origin] %u [Adjust] %u\n", 0, adc_data[0], upload_data);
#endif
		/* ADC 1 */
		adc_data[1] = (unsigned short)adc_read((unsigned long)1);
		upload_data_float = (float)adc_data[1] - V_ADC_GND - V_ADC_PC;
		upload_data_float = upload_data_float;
		upload_data = upload_data_float / 1;
#ifndef DEBUG_HUMAN
		uart_putc((uint8_t)(upload_data >> 8));
		uart_putc((uint8_t)(upload_data & 0x00ff));
#endif
#ifdef DEBUG_HUMAN
		printf("ADC[%d]: [Origin] %u [Adjust] %u\n", 1, adc_data[1], upload_data);
#endif
		/* ADC 2 */
		adc_data[2] = (unsigned short)adc_read((unsigned long)2);
		upload_data_float = (float)adc_data[2] - V_ADC_GND - V_ADC_PC
			+ V_ADC_PC;
		upload_data_float = upload_data_float;
		upload_data = upload_data_float / 1;
#ifndef DEBUG_HUMAN
		uart_putc((uint8_t)(upload_data >> 8));
		uart_putc((uint8_t)(upload_data & 0x00ff));
#endif
#ifdef DEBUG_HUMAN
		printf("ADC[%d]: [Origin] %u [Adjust] %u\n", 2, adc_data[2], upload_data);
#endif
		/* ADC 3 */
		adc_data[3] = (unsigned short)adc_read((unsigned long)3);
		upload_data_float = (float)adc_data[3] - V_ADC_GND - V_ADC_PC;
		// Below +8 is magic number measured by Liming Zhang
		upload_data_float = upload_data_float + 8;
		upload_data = upload_data_float / 1;
#ifndef DEBUG_HUMAN
		uart_putc((uint8_t)(upload_data >> 8));
		uart_putc((uint8_t)(upload_data & 0x00ff));
#endif
#ifdef DEBUG_HUMAN
		printf("ADC[%d]: [Origin] %u [Adjust] %u\n", 3, adc_data[3], upload_data);
#endif
		/* ADC 4 */
		adc_data[4] = (unsigned short)adc_read((unsigned long)4);
		upload_data_float = (float)adc_data[4] - V_ADC_GND - V_ADC_PC;
		upload_data_float = (upload_data_float / 93) * SCALAR_THEORETICAL;
		upload_data = upload_data_float / 1;
#ifndef DEBUG_HUMAN
		uart_putc((uint8_t)(upload_data >> 8));
		uart_putc((uint8_t)(upload_data & 0x00ff));
#endif
#ifdef DEBUG_HUMAN
		printf("ADC[%d]: [Origin] %u [Adjust] %u\n", 4, adc_data[4], upload_data);
#endif
		/* ADC 5 */
		adc_data[5] = (unsigned short)adc_read((unsigned long)5);
		upload_data_float = (float)adc_data[5] - V_ADC_GND - V_ADC_PC;
		upload_data_float = upload_data_float;
		upload_data = upload_data_float / 1;
#ifndef DEBUG_HUMAN
		uart_putc((uint8_t)(upload_data >> 8));
		uart_putc((uint8_t)(upload_data & 0x00ff));
#endif
#ifdef DEBUG_HUMAN
		printf("ADC[%d]: [Origin] %u [Adjust] %u\n", 5, adc_data[5], upload_data);
#endif
		/* ADC 6 */
		adc_data[6] = (unsigned short)adc_read((unsigned long)6);
		upload_data_float = (float)adc_data[6] - V_ADC_GND - V_ADC_PC;
		upload_data_float = upload_data_float;
		upload_data = upload_data_float / 1;
#ifndef DEBUG_HUMAN
		uart_putc((uint8_t)(upload_data >> 8));
		uart_putc((uint8_t)(upload_data & 0x00ff));
#endif
#ifdef DEBUG_HUMAN
		printf("ADC[%d]: [Origin] %u [Adjust] %u\n", 6, adc_data[6], upload_data);
#endif
		/* ADC 7 */
		adc_data[7] = (unsigned short)adc_read((unsigned long)7);
		upload_data_float = (float)adc_data[7] - V_ADC_GND - V_ADC_PC;
		upload_data_float = upload_data_float;
		upload_data = upload_data_float / 1;
#ifndef DEBUG_HUMAN
		uart_putc((uint8_t)(upload_data >> 8));
		uart_putc((uint8_t)(upload_data & 0x00ff));
#endif
#ifdef DEBUG_HUMAN
		printf("ADC[%d]: [Origin] %u [Adjust] %u\n", 7, adc_data[7], upload_data);
#endif
		/* ADC 8 */
		adc_data[8] = (unsigned short)adc_read((unsigned long)8);
		upload_data_float = (float)adc_data[8] - V_ADC_GND - V_ADC_PC;
		upload_data_float = (upload_data_float / 93) * SCALAR_THEORETICAL;
		upload_data = upload_data_float / 1;
#ifndef DEBUG_HUMAN
		uart_putc((uint8_t)(upload_data >> 8));
		uart_putc((uint8_t)(upload_data & 0x00ff));
#endif
#ifdef DEBUG_HUMAN
		printf("ADC[%d]: [Origin] %u [Adjust] %u\n", 8, adc_data[8], upload_data);
#endif
		/* ADC 9 */
		adc_data[9] = (unsigned short)adc_read((unsigned long)9);
		upload_data_float = (float)adc_data[9] - V_ADC_GND - V_ADC_PC;
		upload_data_float = upload_data_float;
		upload_data = upload_data_float / 1;
#ifndef DEBUG_HUMAN
		uart_putc((uint8_t)(upload_data >> 8));
		uart_putc((uint8_t)(upload_data & 0x00ff));
#endif
#ifdef DEBUG_HUMAN
		printf("ADC[%d]: [Origin] %u [Adjust] %u\n", 9, adc_data[9], upload_data);
#endif
		/* ADC 10 */
		adc_data[10] = (unsigned short)adc_read((unsigned long)10);
		upload_data_float = (float)adc_data[10] - V_ADC_GND - V_ADC_PC;
		upload_data_float = (upload_data_float / 48) * SCALAR_THEORETICAL;
		upload_data = upload_data_float / 1;
#ifndef DEBUG_HUMAN
		uart_putc((uint8_t)(upload_data >> 8));
		uart_putc((uint8_t)(upload_data & 0x00ff));
#endif
#ifdef DEBUG_HUMAN
		printf("ADC[%d]: [Origin] %u [Adjust] %u\n", 10, adc_data[10], upload_data);
#endif
		/* ADC 11 */
		adc_data[11] = (unsigned short)adc_read((unsigned long)11);
		upload_data_float = (float)adc_data[11] - V_ADC_GND - V_ADC_PC;
		upload_data_float = (upload_data_float / 92) * SCALAR_THEORETICAL;
		upload_data = upload_data_float / 1;
#ifndef DEBUG_HUMAN
		uart_putc((uint8_t)(upload_data >> 8));
		uart_putc((uint8_t)(upload_data & 0x00ff));
#endif
#ifdef DEBUG_HUMAN
		printf("ADC[%d]: [Origin] %u [Adjust] %u\n", 11, adc_data[11], upload_data);
#endif
		/* ADC 12 */
		adc_data[12] = (unsigned short)adc_read((unsigned long)12);
		upload_data_float = (float)adc_data[12] - V_ADC_GND - V_ADC_PC;
		upload_data_float = (upload_data_float / 92) * SCALAR_THEORETICAL;
		upload_data = upload_data_float / 1;
#ifndef DEBUG_HUMAN
		uart_putc((uint8_t)(upload_data >> 8));
		uart_putc((uint8_t)(upload_data & 0x00ff));
#endif
#ifdef DEBUG_HUMAN
		printf("ADC[%d]: [Origin] %u [Adjust] %u\n", 12, adc_data[12], upload_data);
#endif
		/* ADC 13 */
		adc_data[13] = (unsigned short)adc_read((unsigned long)13);
		upload_data_float = (float)adc_data[13] - V_ADC_GND - V_ADC_PC;
		upload_data_float = (upload_data_float / 92) * SCALAR_THEORETICAL;
		upload_data = upload_data_float / 1;
#ifndef DEBUG_HUMAN
		uart_putc((uint8_t)(upload_data >> 8));
		uart_putc((uint8_t)(upload_data & 0x00ff));
#endif
#ifdef DEBUG_HUMAN
		printf("ADC[%d]: [Origin] %u [Adjust] %u\n", 13, adc_data[13], upload_data);
#endif
		/* ADC 14 */
		adc_data[14] = (unsigned short)adc_read((unsigned long)14);
		upload_data_float = (float)adc_data[14] - V_ADC_GND - V_ADC_PC;
		upload_data_float = upload_data_float;
		upload_data = upload_data_float / 1;
#ifndef DEBUG_HUMAN
		uart_putc((uint8_t)(upload_data >> 8));
		uart_putc((uint8_t)(upload_data & 0x00ff));
#endif
#ifdef DEBUG_HUMAN
		printf("ADC[%d]: [Origin] %u [Adjust] %u\n", 14, adc_data[14], upload_data);
#endif
		/* ADC 15 */
		adc_data[15] = (unsigned short)adc_read((unsigned long)15);
		upload_data_float = (float)adc_data[15] - V_ADC_GND - V_ADC_PC;
		upload_data_float = upload_data_float;
		upload_data = upload_data_float / 1;
#ifndef DEBUG_HUMAN
		uart_putc((uint8_t)(upload_data >> 8));
		uart_putc((uint8_t)(upload_data & 0x00ff));
#endif
#ifdef DEBUG_HUMAN
		printf("ADC[%d]: [Origin] %u [Adjust] %u\n", 15, adc_data[15], upload_data);
#endif

#ifdef DEBUG_HUMAN
		printf("\n");
		timer_delay_ms(1000);
#endif
	}
	return 0;
}
