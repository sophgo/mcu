#ifndef __ADC_H__
#define __ADC_H__

#define ADC_CHN_AN0			0b00000
#define ADC_CHN_AN1			0b00001
#define ADC_CHN_AN2			0b00010
#define ADC_CHN_AN3			0b00011
#define ADC_CHN_AN4			0b00100
#define ADC_CHN_AN5			0b00101
#define ADC_CHN_AN6			0b00110
#define ADC_CHN_AN7			0b00111
#define ADC_CHN_AN8			0b01000
#define ADC_CHN_AN9			0b01001
#define ADC_CHN_AN10		0b01010
#define ADC_CHN_AN11		0b01011
#define ADC_CHN_TEMP_INNER	0b11101
#define ADC_CHN_DAC_OUT		0b11110
#define ADC_CHN_FVR_BUF1	0b11111

#define ADC_PSEL_VDD		0b00
#define ADC_PSEL_VREF		0b10
#define ADC_PSEL_AN1		0b11

#define ADC_NSEL_VSS		0b0
#define ADC_NSEL_AN0		0b1


// use VSS and VDD as REF
void ADC_init(unsigned char pref,unsigned char nref);
void ADC_start(unsigned char channel);
unsigned char ADC_isfinish();
unsigned short ADC_result(void);
unsigned short ADC_run(unsigned char  channel);

#endif //__ADC_H__
