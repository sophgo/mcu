/*
 *  spi_io.h
 *  Author: Nelson Lombardo (C) 2015
 *  e-mail: nelson.lombardo@gmail.com
 *  License at the end of file.
 */

#ifndef _SPI_IO_H_
#define _SPI_IO_H_

#include "integer.h"        /* Type redefinition for portability */

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/rcc.h>

#define NSS_PORT	GPIOA
#define NSS_PIN		GPIO15
#define SPI		SPI1
#define RCC_SPI		RCC_SPI1

/******************************************************************************
 Module Public Functions - Low level SPI control functions
******************************************************************************/

static inline void SPI_Init (void)
{
	rcc_periph_clock_enable(RCC_SPI);
	spi_reset(SPI);
	spi_init_master(SPI,
			SPI_CR1_BAUDRATE_FPCLK_DIV_2,
			SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
			SPI_CR1_CPHA_CLK_TRANSITION_1,
			SPI_CR1_DFF_8BIT,
			SPI_CR1_MSBFIRST);
	spi_enable_software_slave_management(SPI);
	spi_set_nss_high(SPI);
	spi_enable(SPI);
}

#define SPI_DELAY()							\
	do {								\
		asm volatile ("nop; nop; nop; nop;");			\
		asm volatile ("nop; nop; nop; nop;");			\
		asm volatile ("nop;");					\
	} while (0)

static inline void spi_write_sector(void *data)
{
	unsigned int i;

	while(SPI_SR(SPI) & SPI_SR_BSY);
	for (i = 0; i < 512; ++i) {
		SPI_DR(SPI) = ((uint8_t *)data)[i];
		SPI_DELAY();
	}
	while(SPI_SR(SPI) & SPI_SR_BSY);
	SPI_DR(SPI);
}

static inline BYTE SPI_RW (BYTE d);

static inline void spi_after_write(void)
{
	unsigned int i;

	do {
		while(SPI_SR(SPI) & SPI_SR_BSY);
		for (i = 0; i < 16; ++i) {
			SPI_DR(SPI) = 0xff;
			SPI_DELAY();
		}
		while(SPI_SR(SPI) & SPI_SR_BSY);
		SPI_DR(SPI);
	} while (SPI_RW(0xff) == 0);
	return;
}

static inline BYTE SPI_RW (BYTE d) {
	uint8_t dr;

	while(SPI_SR(SPI) & SPI_SR_BSY);
	SPI_DR(SPI) = d;
	while(!(SPI_SR(SPI) & SPI_SR_RXNE));
	dr = SPI_DR(SPI);

	return dr;
}

static inline void SPI_Release (void) {
}

static inline void SPI_CS_Low (void)
{
	gpio_clear(NSS_PORT, NSS_PIN);
}

static inline void SPI_CS_High (void)
{
	gpio_set(NSS_PORT, NSS_PIN);
}

static inline void SPI_Freq_High (void)
{
#if 0
	/* 16M */
	spi_set_baudrate_prescaler(SPI, SPI_CR1_BAUDRATE_FPCLK_DIV_2);
#endif
}

static inline void SPI_Freq_Low (void)
{
#if 0
	/* 256K */
	spi_set_baudrate_prescaler(SPI, SPI_CR1_BAUDRATE_FPCLK_DIV_128);
#endif
}

void SPI_Timer_On (WORD ms);
BOOL SPI_Timer_Status (void);
void SPI_Timer_Off (void);


#endif

/*
The MIT License (MIT)

Copyright (c) 2015 Nelson Lombardo

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
