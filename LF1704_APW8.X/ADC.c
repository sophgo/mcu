#include "ADC.h"
#include <xc.h>
#define _XTAL_FREQ 16000000
/**
  Section: ADC Module APIs
*/

void ADC_init(unsigned char pref,unsigned char nref)
{
    // set the ADC to the options selected in the User Interface
    
    // GO_nDONE stop; ADON enabled; CHS AN0; 
    //ADCON0bits.ADON = 1;
	
	
    ADCON1bits.ADFM		= 1;	// Right justified.
	ADCON1bits.ADCS		= 0b000;// FOSC/2
    ADCON1bits.ADNREF	= nref;	// VREF- is connected to
    ADCON1bits.ADPREF	= pref;	// VREF+ is connected to

    // TRIGSEL no_auto_trigger; 
    ADCON2bits.TRIGSEL	= 0b0000;//No auto-conversion trigger selected
    
    // ADRESL 0; 
    ADRESL = 0x00;
    
    // ADRESH 0; 
    ADRESH = 0x00;
    
}
static void delay48us(int time)
//delay about 5us @ 16MHz CLK
{ 
    long int i=0;
    for(i=0;i<time ; i++) ; 
}

void ADC_start(unsigned char channel)
{
    // select the A/D channel
    ADCON0bits.CHS = channel;    
    // Turn on the ADC module
    ADCON0bits.ADON = 1;
	
    // Acquisition time delay
    __delay_ms(1);

    // Start the conversion
    ADCON0bits.GO_nDONE = 1;
    
}

unsigned char ADC_isfinish()
{
    // Start the conversion
    return (!ADCON0bits.GO_nDONE);
}

unsigned short ADC_result(void)
{
    // Conversion finished, return the result
    return ((ADRESH&0x03) << 8) + ADRESL;
}
// result 0-1023
unsigned short ADC_run(unsigned char  channel)
{
	ADC_start(channel);
	
    // Wait for the conversion to finish
    while (!ADC_isfinish())
    {
    }
    
    // Conversion finished, return the result
    return ADC_result();
}
