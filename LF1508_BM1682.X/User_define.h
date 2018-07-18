/* 
 * File:   User_define.h
 * Powered by: Pengfei Chen(Patrick)
 *
 * V0.1
 * Created on Jan 02, 2018
 * Intial
 */
#include"xc.h"
#define    TMR0_Value     177
#define __16F1508
#define LED0 PORTAbits.RA0 
#define RST_REQ PORTAbits.RA1 
#define B_TEMP_ALR_N PORTAbits.RA2
//#define POWER_EN PORTAbits.RA3 //use MCLR function to control Power EN, do not need config more
#define FAN_TACH PORTAbits.RA4 
#define PG_DDR4_1V2 PORTAbits.RA5 
#define TWARN_VDD_C PORTBbits.RB5
#define PG_VDD_C PORTBbits.RB7
//#define MCU_ERR_INT PORTCbits.RC1
#define MCU_ERR_INT LATC1
//#define EN_B_3V3 PORTCbits.RC2 
#define EN_B_3V3 LATC2
//#define EN_VDD_IO_1V8 PORTCbits.RC3 
#define EN_VDD_IO_1V8 LATC3
//#define EN_B_DDR4_1V2 PORTCbits.RC4
#define EN_B_DDR4_1V2 LATC4
//#define EN_ISL68127 PORTCbits.RC5 
#define EN_ISL68127 LATC5 
//#define EN_RGMII_3V3 PORTCbits.RC6
#define EN_RGMII_3V3 LATC6 
//#define SYS_RST PORTCbits.RC7
#define SYS_RST LATC7

#define RX_ELMNTS	32
//void Initial_sys(void);
//void Initial_FVR(void);
// array for master to read from
volatile unsigned char I2C_Array[RX_ELMNTS] =
{0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x66,0x55,0x44,0x33,0x22,0x11,0xFA,
0xEA,0xDA,0xCA,0xBA,0xFB,0xFC,0xFD,0xFE,
0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08};
unsigned char Timer_Status = 0;


void delay5us(int time);
void delayms(long int time);
void delays(int time);
void Power_Up(void);
void Power_Down(void);
void Initial_FVR(void);
void Initial_sys(void);
void Initial_TMR1_FAN(void);
void Initial_TMR1_TIME(void);
void Initial_NCO();