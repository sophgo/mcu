/*
 * File:   Initial_TIMER.c
 * Author: taiqiang.cao
 *
 * Created on June 3, 2019, 2:39 PM
 */

#include"xc.h"
#include"User_define.h"


void Initial_TIMER()//overflow every second
{
    //overflow cannot clear by HW
    T0CON1bits.T0CS = 0b100;//LFINTOSC  31~32kHz
	//T0CON1bits.T0CKPS = 0b1110;//1:16384 Prescaler
	//TMR0H = 1;//8bit mode TMR0H as comparator T = (TMR0H+1)/(32K/16384)
	
    T0CON1bits.T0CKPS = 0b0011;//1:8 Prescaler
    TMR0H = 249;//8bit mode TMR0H as comparator T = (TMR0H+1)/(32K/128)
    T0CON0bits.T0OUTPS = 0;//1:1 Postscaler
    TMR0IE = 1;
    T0EN = 1;    
}