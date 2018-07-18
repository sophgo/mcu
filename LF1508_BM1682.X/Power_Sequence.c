#include"User_define.h"
#include"xc.h"
/*******************************************************************************
PIC16F1508 BM1682 SINGLE CHIP
POWER SEQUENCE CODE 
Author: Patrick Chen(CPF) 
BITMAIN 
DATE: 21/03/2018
/****************************************************************/
void Power_Up(void)
{
    SYS_RST = 0;
//    EN_B_3V3 = 1;//not controlled by MCU
    delayms(16);
    EN_ISL68127 = 0;
    delayms(5);
    EN_B_DDR4_1V2 = 0;//for Single & new dual chip
    delayms(5);
    EN_VDD_IO_1V8 = 1;
    delayms(5);
    EN_RGMII_3V3 = 0;//for new single chip
    delayms(3);
    SYS_RST = 1;

}

void Power_Down(void)
{
    SYS_RST = 0;
    delayms(3);    
    EN_RGMII_3V3 = 1;//for new single chip
    delayms(5);
    EN_VDD_IO_1V8 = 0;
    delayms(5);
    EN_B_DDR4_1V2 = 1;//for Single & new dual chip
    delayms(5);
    EN_ISL68127 = 1;
    delayms(5);
    EN_B_3V3 = 0;
//    LED0 = 0;
}