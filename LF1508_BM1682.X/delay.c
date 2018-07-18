/*******************************************************************************
PIC16F1508 BM1682 SINGLE CHIP
DELAY CODE 
Author: Patrick Chen(CPF) 
BITMAIN 
DATE: 21/03/2018
/****************************************************************/
#include"User_define.h"
void delay5us(int time)
//delay about 5us @ 16MHz CLK
{ 
    long int i=0;
    for(i=0;i<time ; i++) ; 
}

void delayms(long int time)
//?????? time ms??
{ 
    long int i=0,j =0;
//    j = time *1000;
    for(i=0;i<time;i++)
        for(j=0; j<160; j++);
}

void delays(int time)
//?????? time s??
{ 
    long int i=0,j =0;
    j = time *1000;
    for(i=0;i<j;i++) 
        delayms(1); 
}