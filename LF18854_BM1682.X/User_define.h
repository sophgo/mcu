/*******************************************************************************
PIC16LF18854 BM1682 CORE BOARD
USER DEFINE HEADER CODE 
Author: Patrick Chen(CPF) 
BITMAIN 
DATE: 05/05/2018
/****************************************************************/
#include"xc.h"
#define    TMR0_Value     177
#define __16F18854

#define B_TEMP_ALR_N PORTAbits.RA0
#define PG_VDD_C PORTAbits.RA1
#define PG_DDR4_1V2 PORTAbits.RA3
#define MCU_ERR_INT LATA4
#define EN_B_3V3 LATA5
#define TWARN_VDD_C PORTAbits.RA6
#define EN_VDD_IO_1V8 LATA7
#define LED0 PORTBbits.RB7  
#define SYS_RST LATC0
#define EN0_ISL68127 LATC1
#define EN1_ISL68127 LATC2
#define EN_RGMII_3V3 LATC5


#define RX_ELMNTS	64

volatile unsigned char I2C_Array[RX_ELMNTS] =
{0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x66,0x55,0x44,0x33,0x22,0x11,0xFA,
0xEA,0xDA,0xCA,0xBA,0xFB,0xFC,0xFD,0xFE,
0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08};
unsigned char Timer_Status = 0;

#define I2C_slave_address 0x60//MCU as a slave,Master read as 0x17


void delay5us(int time);
void delayms(long int time);
void delays(int time);
void Power_Up(void);
void Power_Down(void);
void Initial_FVR(void);
void Initial_sys(void);
void Initial_TIMER();
unsigned int IIC_read_byte(unsigned int dev_addr, unsigned int reg_addr);
unsigned int IIC_write_byte(unsigned int dev_addr, unsigned int reg_addr, unsigned int data);
