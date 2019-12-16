#include "TIMER.h"
#include <xc.h>

static char TIMER0_VALUE_INIT = 0;
void TIMER0_set(int freq_sel)
{
	//FOSC = 16MHz
	
	// Internal instruction cycle clock (FOSC/4)
	OPTION_REGbits.TMR0CS	= 0;
	
	// Increment on low-to-high transition on T0CKI pin
	OPTION_REGbits.TMR0SE	= 0;

	// Prescaler is assigned to the Timer0 module
	OPTION_REGbits.PSA		= 0;
	// Prescaler Rate Select bits
	// 0b000 1:2
	// 0b001 1:4
	// 0b010 1:8
	// 0b011 1:16
	// 0b100 1:32
	// 0b101 1:64
	// 0b110 1:128
	// 0b111 1:256
	if (freq_sel == TIMER0_ISR_FREQ_1k)
	{
		//16000000/4/16/250 = 1000
		OPTION_REGbits.PS	= 0b011;
		TIMER0_VALUE_INIT	= 6;//256-250;
	}
	else if (freq_sel == TIMER0_ISR_FREQ_200)
	{
		//16000000/4/128/156 = 200
		OPTION_REGbits.PS	= 0b110;
		TIMER0_VALUE_INIT	= 100;//256-156
	}
	else //61Hz  61.03515625
	{
		OPTION_REGbits.PS = 0b111;
		TIMER0_VALUE_INIT = 0;
	}
	TMR0 = TIMER0_VALUE_INIT;
}

char TIMER0_isr()
{
	if (TMR0IF)
	{
		TMR0 = TIMER0_VALUE_INIT;
		TMR0IF = 0;
		return 1;
	}
	return 0;
}


// PWM_frequence = 1 / PWM_period = FOSC / ((PR2 + 1) * 4 * Prescaler)
// PWM_frequence = 16MHz / ((PR2 + 1) * 4 * Prescaler)

// PWM_frequence = 1k	=> Prescaler=16	PR2=16000k/1k/4/16=250
// PWM_frequence = 10k	=> Prescaler=4	PR2=16000k/10k/4/4=100

//TIMER2_PWM_MAXDC < 1024
static int TIMER2_PWM_MAXDC = 0;
int TIMER2_get_PWMMAXDC()
{
	return TIMER2_PWM_MAXDC;
}

void TIMER2_set_match_freq(int match_freq_sel)
{
	
	//	11 = Prescaler is 64
	//	10 = Prescaler is 16
	//	01 = Prescaler is 4
	//	00 = Prescaler is 1
	if (match_freq_sel == TIMER2_MATCH_FREQ_1K)
	{
		T2CONbits.T2CKPS	= 0b10;// Prescaler=16
	}
	else// if (match_freq_sel == TIMER2_MATCH_FREQ_10K)
	{
		T2CONbits.T2CKPS	= 0b01;// Prescaler=4
	}
	
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
	
	if (match_freq_sel == TIMER2_MATCH_FREQ_1K)
	{
		PR2 = 250;
		TIMER2_PWM_MAXDC = 1000;
	}
	else// if (match_freq_sel == TIMER2_MATCH_FREQ_10K)
	{
		PR2 = 100;
		TIMER2_PWM_MAXDC = 400;
	}
}

void TIMER_PWM_sel()
{
	//	11 = Reserved
	//	10 = PWMX is based off Timer6
	//	01 = PWMX is based off Timer4
	//	00 = PWMX is based off Timer2
	CCPTMRSbits.P4TSEL	= 0b11;	//PWM4
	CCPTMRSbits.P3TSEL	= 0b00; //PWM3 is based off Timer2
	CCPTMRSbits.C2TSEL	= 0b11;	//CCP2(PWM2)
	CCPTMRSbits.C1TSEL	= 0b11;	//CCP1(PWM1)
}
