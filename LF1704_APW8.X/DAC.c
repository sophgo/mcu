#include "DAC.h"
#include <xc.h>

void DAC1_init(unsigned char pref,unsigned char nref)
{
	DAC1CON0bits.DAC1EN		= 0;//DAC is disable
	DAC1CON0bits.DAC1PSS	= pref;// DAC1 Positive Source Select bits
	DAC1CON0bits.DAC1NSS	= nref;// DAC1 Negative Source Select bits
}

void DAC1_set(unsigned char dacsel)
{
	DAC1CON0bits.DAC1OE1	= 0;// DAC1 Voltage Output 1 Enable bit
	DAC1CON0bits.DAC1OE2	= 0;// DAC1 Voltage Output 2 Enable bit

	DAC1CON1bits.DAC1R		= dacsel;

	DAC1CON0bits.DAC1EN		= 1;//DAC is enabled
}
