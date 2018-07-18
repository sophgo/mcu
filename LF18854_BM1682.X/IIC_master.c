/*******************************************************************************
PIC16LF18854 BM1682 CORE BOARD
IIC MASTER CODE 
Author: Patrick Chen(CPF) 
BITMAIN 
DATE: 05/05/2018
/****************************************************************/
#include"User_define.h"
#include <xc.h>

void I2CMasterStart (void)
{
	SSP2IF = 0;									
	SSP2CON2bits.SEN = 1;									
	while (!SSP2IF);
	SSP2IF = 0;								
}

void I2CMasterRestart (void)
{
	SSP2IF = 0;
	SSP2CON2bits.RSEN = 1;	
	while (!SSP2IF);
	SSP2IF = 0;
}

void I2CMasterStop (void)
{
	SSP2IF = 0;
	SSP2CON2bits.PEN = 1;
	while (!SSP2IF);
	SSP2IF = 0;
}

unsigned char I2CMasterRead (unsigned char go_on)
{
	SSP2IF = 0;
	SSP2CON2bits.RCEN = 1;
	while (!SSP2IF);

	SSP2CON2bits.ACKDT = (go_on & 0x01);
	SSP2CON2bits.ACKEN = 1;	

	SSP2IF = 0;	
	while (!SSP2IF);
	SSP2IF = 0;
	return (SSP2BUF);
}

unsigned char I2CMasterWrite (unsigned char data)
{
	SSP2CON1bits.WCOL = 0;
	SSP2CON1bits.SSPOV = 0;
	SSP2IF = 0;	
	SSP2BUF = data;		

	while (!SSP2IF);
	SSP2IF = 0;
	return (SSP2CON2bits.ACKSTAT);
}

unsigned int IIC_read_byte(unsigned int dev_addr, unsigned int reg_addr)
{
	I2CMasterStart ();
	I2CMasterWrite (dev_addr);
	I2CMasterWrite (reg_addr);
	I2CMasterRestart ();
	I2CMasterWrite (dev_addr + 1);
//	I2C_Array[0x0F] = I2CMasterRead (0x00);
    return I2CMasterRead(reg_addr);
}
unsigned int IIC_write_byte(unsigned int dev_addr, unsigned int reg_addr, unsigned int data)
{
	I2CMasterStart ();
	I2CMasterWrite (dev_addr);
	I2CMasterWrite (reg_addr);
	I2CMasterWrite (data);
	I2CMasterStop ();
}

void TMP461_initial()
{
    IIC_write_byte(0x98, 0x0C, 0xEC);//1682 under temperature alert 0xEC = -20C
    IIC_write_byte(0x98, 0x0D, 0xEC);//tmp under temperature alert 0xEC = -20C
    IIC_write_byte(0x98, 0x20, 0x3C);//TMP461 over temperature alert 0x3c = 60C
    IIC_write_byte(0x98, 0x22, 0x0F);//NUMBER OF OUT-OF-LIMIT MEASUREMENTS REQUIRED TO ASSERT ALERT  = 4
}