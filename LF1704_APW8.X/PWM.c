#include "PWM.h"
#include <xc.h>
void PWM_set(short dutycycle)
{
	PWM3DCH     =	(dutycycle >> 2) & 0xFF;
	PWM3DCL     = 	(dutycycle ) & 0x03;
	//PWM1CONbits.PWM1OUT
	PWM3CONbits.PWM3POL	= 0;// PWM output is active-high
	PWM3CONbits.PWM3EN	= 1;// PWM module is enabled
}
