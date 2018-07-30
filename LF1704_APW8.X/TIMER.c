#include "TIMER.h"
#include <xc.h>

void TIMER2_set()
{
	
	//	11 = Prescaler is 64
	//	10 = Prescaler is 16
	//	01 = Prescaler is 4
	//	00 = Prescaler is 1
	T2CONbits.T2CKPS	= 0b01;
	//	1111 = 1:16 Postscaler
	//	1110 = 1:15 Postscaler
	//	1101 = 1:14 Postscaler
	//	1100 = 1:13 Postscaler
	//	1011 = 1:12 Postscaler
	//	1010 = 1:11 Postscaler
	//	1001 = 1:10 Postscaler
	//	1000 = 1:9 Postscaler
	//	0111 = 1:8 Postscaler
	//	0110 = 1:7 Postscaler
	//	0101 = 1:6 Postscaler
	//	0100 = 1:5 Postscaler
	//	0011 = 1:4 Postscaler
	//	0010 = 1:3 Postscaler
	//	0001 = 1:2 Postscaler
	//	0000 = 1:1 Postscaler
	T2CONbits.T2OUTPS	= 0b0000;
	T2CONbits.TMR2ON	= 1;
    PR2 = 100;	//16M/4/4/100 == 10K
}

void TIMER_PWM_sel()
{
	//	11 = Reserved
	//	10 = PWMX is based off Timer6
	//	01 = PWMX is based off Timer4
	//	00 = PWMX is based off Timer2
	CCPTMRSbits.P4TSEL	= 0b11;
	CCPTMRSbits.P3TSEL	= 0b00; //PWM3 is based off Timer2
	CCPTMRSbits.C2TSEL	= 0b11;
	CCPTMRSbits.C1TSEL	= 0b11;
}
