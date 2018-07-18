/*******************************************************************************
PIC16LF18854 BM1682 CORE BOARD
FVR CODE
Author: Patrick Chen(CPF) 
BITMAIN 
DATE: 05/05/2018
/****************************************************************/
#include"xc.h"
void Initial_FVR()
{
    FVRCONbits.FVREN=1; //enable FVR
    FVRCONbits.TSEN=0;
    FVRCONbits.CDAFVR=0b01;// set 1024v
    FVRCONbits.ADFVR=0;// trun off AD FVR
    //********initial comparator**********************
    CM2CON0bits.C2ON=1;//enable comparator 2
    CM2CON0bits.C2POL=1;//Comparator output is inverted
    CM2CON0bits.C2SP=1;//Comparator mode in normal power, higher speed
    CM2CON1bits.C2INTP=1;// The CxIF interrupt flag will be set upon a positive going edge of the CxOUT bit CPF
    CM2CON1bits.C2INTN=1;//for 12V voltage recovery, reboot system.
    CM2CON0bits.C2HYS=1;//Enable Hysteresis
    CM2NSELbits.C2NCH = 0b110;///CxVN connects to FVR Voltage reference C2IN-
    CM2PSELbits.C2PCH=0b000;//CxVP connects to CxIN+ pin C2IN+
}
