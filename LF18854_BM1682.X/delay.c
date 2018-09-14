/*******************************************************************************
PIC16LF18854 BM1682 CORE BOARD
DELAY CODE
Author: Patrick Chen(CPF) 
BITMAIN 
DATE: 05/05/2018
/****************************************************************/
#include"User_define.h"
/*
void delay5us(int time)
//delay about 5us @ 16MHz CLK
{ 
    long int i=0;
    for(i=0;i<time ; i++) ; 
}

void delayms(long int time)
{ 
    long int i=0,j =0;
//    j = time *1000;
    for(i=0;i<time;i++)
        for(j=0; j<160; j++);
}

void delays(int time)
{ 
    long int i=0,j =0;
    j = time *1000;
    for(i=0;i<j;i++) 
        delayms(1); 
}
*/