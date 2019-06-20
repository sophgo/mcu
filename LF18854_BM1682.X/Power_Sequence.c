/*******************************************************************************
PIC16LF18854 BM1682 CORE BOARD
POWER SEQUENCE CODE 
Author: Patrick Chen(CPF) 
BITMAIN 
DATE: 05/05/2018
/****************************************************************/
#include"User_define.h"
#include"xc.h"
int status = STATUS_POWERDOWN;
// work with __delay_ms __delay_us
#define _XTAL_FREQ 16000000
void Reset(void)
{
	SYS_RST = 0;
	__delay_ms(20);
	SYS_RST = 1;
}
//3.3V EN后要等30ms以上再EN ISL68127
void Power_Up(void)
{
	SYS_RST = 0;
	EN_VDD_IO_1V8 = 1;
	__delay_ms(1);
	EN_RGMII_3V3 = 1;
	__delay_ms(5);
	EN0_ISL68127 = 0;//EN0_ISL68127_3.3V->0.9V
	__delay_ms(5);
	EN1_ISL68127 = 0;//EN1_ISL68127_3.3V->1.2V
	__delay_ms(5);
	SYS_RST = 1;
	__delay_ms(5);
	Reset();
	LED0 = 1;
	status = STATUS_POWERUP;
}

void Power_Down(void)
{
	status = STATUS_POWERDOWN;
	LED0 = 0;
	SYS_RST = 0;
	__delay_ms(5);    
	EN_RGMII_3V3 = 0;
	__delay_ms(5);
	EN_VDD_IO_1V8 = 0;
	__delay_ms(5);
	EN1_ISL68127 = 1;
	__delay_ms(5);
	EN0_ISL68127 = 1;
}