#ifndef __DAC_H__
#define __DAC_H__

#define DAC_PSEL_VDD		0b00
#define DAC_PSEL_VREF_P		0b01
#define DAC_PSEL_FVR_BUF2	0b10

#define DAC_NSEl_VSS		0b0
#define DAC_PSEL_VREF_N		0b1

void DAC1_init(unsigned char pref,unsigned char nref);
void DAC1_set(unsigned char dacsel);
#endif //__DAC_H__
