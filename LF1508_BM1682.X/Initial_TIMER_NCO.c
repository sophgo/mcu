#include"xc.h"
#include"User_define.h"
void Initial_TMR1_FAN()
{
 //TIMER1 GATE TOGGLE MODE Measure the frequency
    T1CONbits.TMR1CS=00;//Timer1 clock source is instruction clock (FOSC/4)
    T1CONbits.T1CKPS=0b11;//1:8 Prescale value
    TMR1GE=1;
    T1GPOL=1;
    T1GTM=1;//0 for debug
    T1GSPM=1;//cpf
    T1GCONbits.T1GSS=0b00;//Timer1 gate pin (T1G)
    TMR1GIF = 0;
    TMR1H = 0b0;
    TMR1L = 0b0;
    TMR1IE = 1;
    TMR1GIE = 1;
    T1CONbits.TMR1ON=1;   // turn on TMR1
    Timer_Status = 1;
  }
void Initial_TMR1_TIME()//not use, may course some problem, sometimes colision with Fan speed measurement
{
 //TIMER1 GATE TOGGLE MODE Measure the frequency
    T1CONbits.TMR1CS=11;//Timer1 clock source is LFINTOSC
    T1CONbits.T1CKPS=0b00;//1:1 Prescale value
    TMR1GE=0;
    T1GCONbits.T1GSS=0b01;//T0_overflow
    TMR1GIF = 0;
    TMR1H = 0x82;
    TMR1L = 0xFF;
    TMR1IE = 1;
    T1CONbits.TMR1ON=1;   // turn on TMR1
    Timer_Status = 2;    
  }

void Initial_NCO()
{
    //TMR2's match as LC1's input,LC1's output as NCO1's input
    //overflow cannot clear by HW
    TMR2IE = 0;//don't need to enable TMR2IE
//    T2CONbits.T2OUTPS = 0b1111;
    T2CONbits.T2CKPS = 0b11;
    PR2 = 0x01;//T2_match frequency = 31.25KHz
    T2CONbits.TMR2ON = 1;
    
    //LC1 OUT config
    CLC1SEL0bits.LC1D1S = 0b111;//LCx_in[7] input is T2_match from lcxd1
//    CLC1SEL0bits.LC1D2S = 0b001;
//    CLC1GLS0bits.LC1G1D1T = 1;//lcxd1 in
    CLC1GLS0 = 0x02;//lcxd1 in. every bit should be set, regardless use or not. default value is uncertain
    CLC1GLS1 = 0x00;//logic 1 for gate 2
    CLC1POLbits.G2POL =1;    
    CLC1GLS2 = 0x00;//logic 1 for gate 3
    CLC1POLbits.G3POL =1;
    CLC1GLS3 = 0x00;//logic 1 for gate 4  
    CLC1POLbits.G4POL =1;
    CLC1CONbits.LC1MODE = 0b010;//4 input AND,input is 1,1,T2_match,1
    LC1EN = 1;
    LC1OE = 1;
    LC1POL = 1;    
    N1EN = 1;
    NCO1CLKbits.N1CKS = 0b10; //LC1_out
//    NCO1CLKbits.N1CKS = 0b01; //FOSC
    NCO1INC = 0x21;//INCREMENT REGISTER 34, 
    NCO1IE = 1;
    
    
}