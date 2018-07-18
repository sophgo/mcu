/*******************************************************************************
PIC16LF18854 BM1682 CORE BOARD
TIMER CODE 
Author: Patrick Chen(CPF) 
BITMAIN 
DATE: 05/05/2018
/****************************************************************/
#include"xc.h"
#include"User_define.h"


void Initial_TIMER()//overflow every second
{
    //overflow cannot clear by HW
    T0CON1bits.T0CS = 0b100;//LFINTOSC
    T0CON1bits.T0CKPS = 0b11110;//1:16834 Prescaler
    TMR0H = 1;//8bit mode TMR0H as comparator T = (TMR0H+1)x0.5
    T0CON0bits.T0OUTPS = 0;//1:1 Postscaler
    TMR0IE = 1;
    T0EN = 1;    
}