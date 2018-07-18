/*******************************************************************************
PIC16LF18854 BM1682 CORE BOARD
POWER SEQUENCE CODE 
Author: Patrick Chen(CPF) 
BITMAIN 
DATE: 05/05/2018
/****************************************************************/
#include"User_define.h"
#include"xc.h"
void Power_Up(void)
{
    SYS_RST = 0;
    EN_B_3V3 = 1;
    delayms(40);//May 15 update,fulfill the 68127's spec
    EN0_ISL68127 = 0;
    delayms(5);
    EN1_ISL68127 = 0;
    delayms(5);
    EN_VDD_IO_1V8 = 1;
    delayms(5);
    EN_RGMII_3V3 = 1;
    delayms(5);
    SYS_RST = 1;
    LED0 = 1;

}

void Power_Down(void)
{
    LED0 = 0;
    SYS_RST = 0;
    delayms(5);    
    EN_RGMII_3V3 = 0;
    delayms(5);
    EN_VDD_IO_1V8 = 0;
    delayms(5);
    EN1_ISL68127 = 1;
    delayms(5);
    EN0_ISL68127 = 1;
    delayms(5);
    EN_B_3V3 = 0;

}