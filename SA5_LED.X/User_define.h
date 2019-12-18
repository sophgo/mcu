/*******************************************************************************
PIC16LF18854 BM1682 CORE BOARD
USER DEFINE HEADER CODE 
Author: Patrick Chen(CPF) 
BITMAIN 
DATE: 05/05/2018
/****************************************************************/
#ifndef USER_DEFINE_H_
#define USER_DEFINE_H_
#include"xc.h"
#include"delay.h"

#define    TMR0_Value     177
#define __16F18854

#define SYS_GOOD PORTAbits.RA0
#define SYS_FAULT PORTAbits.RA1
#define PWR_GOOD PORTAbits.RA2
#define PWR_FAULT PORTAbits.RA3
#define ETH_GOOD PORTAbits.RA4
#define ETH_FAULT PORTAbits.RA5

#define LED_BD_PWR PORTBbits.RB0
#define LED_BD_UID PORTBbits.RB3
#define FAN_GOOD PORTBbits.RB4
#define FAN_FAULT PORTBbits.RB5

#define HDD_GOOD PORTCbits.RC0
#define HDD_FAULT PORTCbits.RC1
#define LED_PWR_P PORTCbits.RC2
#define LED_PWR_N PORTCbits.RC5
#define LED_UID_N PORTCbits.RC6
#define LED_UID_P PORTCbits.RC7

//#define MCU_ERR_INT LATA4
//#define EN_B_3V3 LATA5
//#define TWARN_VDD_C PORTAbits.RA6
//#define EN_VDD_IO_1V8 LATA7
//#define SYS_RST LATC0
//#define EN0_ISL68127 LATC1
//#define EN1_ISL68127 LATC2
//#define EN_RGMII_3V3 LATC5

#define INDEX_VERSION		0x07
#define INDEX_HDD	0x00
#define INDEX_ETH	0x01
#define INDEX_SYS	0x02
#define INDEX_FAN	0x03
#define INDEX_PWR	0x04
#define INDEX_PWR_KEY	0x05
#define INDEX_UID_KEY	0x06

#define RX_ELMNTS	16

volatile unsigned char I2C_Array[RX_ELMNTS] =
//led0, led1, led2, led3, led4, key_power, key_uid, version
{0x00, 0x00, 0x05, 0x00, 0x00,  0x00,      0x01,   0x05,
//flick frequence
0x00,
};
unsigned char Timer_Status = 0;

#define I2C_slave_address (0x18 << 1)	//MCU as a slave,Master read as 0x17

#define STATUS_POWERUP		1
#define STATUS_POWERDOWN	0
extern int status;

volatile unsigned int one_hot_led;

#define BIT0   (1 << 0)
#define BIT1   (1 << 1)
#define BIT2   (1 << 2)
#define BIT3   (1 << 3)
#define BIT4   (1 << 4)
#define BIT5   (1 << 5)
#define BIT6   (1 << 6)
#define BIT7   (1 << 7)
#define BIT8   (1 << 8)
#define BIT9   (1 << 9)



//extern int uart_send = 0;
//extern int test_reboot = 1;

void Reset(void);
void Power_Up(void);
void Power_Down(void);
void Initial_FVR(void);
void Initial_sys(void);
void Initial_TIMER();
unsigned int IIC_read_byte(unsigned int dev_addr, unsigned int reg_addr);
unsigned int IIC_write_byte(unsigned int dev_addr, unsigned int reg_addr, unsigned int data);
#endif //define USER_DEFINE_H_