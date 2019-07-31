/*******************************************************************************
PIC16LF18854 BM1682 CORE BOARD
USER DEFINE HEADER CODE 
Author: Patrick Chen(CPF) 
BITMAIN 
DATE: 05/05/2018
/****************************************************************/
#include"xc.h"
#include"delay.h"

#define VERSION			0x0C
#define INTERNAL_VERSION	0x01

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


#define CMD_DEBUG_MODE		0x0D
#define CMD_DOG_ONOFF		0x10
#define CMD_DOG_FEED		0x11
#define CMD_REBOOT			0x12		// include power on/off
#define CMD_RECOVERY		0x15
#define CMD_SET_UTC			0x16
#define CMD_CLEAN_FACTORY_MODE	0x17
#define CMD_SET_FACTORY_MODE	0x18

#define CMD_RESET			0x66		//
#define CMD_CLRERR			0x86
#define CMD_POWERDOWN		0xF7

#define INDEX_VERSION		0x00
#define INDEX_INSTRUCTION	0x01
#define INDEX_TMP_1682		0x02
#define INDEX_TMP_461		0x03
#define INDEX_SYS_VOL_ST	0x04
#define INDEX_SYS_TMP_ST	0x05
#define INDEX_12V_ST		0x06
#define INDEX_RESET_COUNT	0x07
//#define INDEX_FAN_ST		0x08
#define INDEX_TIME_L		0x09
#define INDEX_TIME_H		0x0A
//#define INDEX_LTIME_L		0x0B
//#define INDEX_LTIME_H		0x0C
#define INDEX_DOG_TIME_OUT	0x0D
#define INDEX_MCU_STATUS	0x0E
#define INDEX_POWERDOWN_REASON 0x0F
#define INDEX_UTC_00		0x10
#define INDEX_UTC_01		0x11
#define INDEX_UTC_02		0x12
#define INDEX_UTC_03		0x13
#define INDEX_SETUTC_00		0x14
#define INDEX_SETUTC_01		0x15
#define INDEX_SETUTC_02		0x16
#define INDEX_SETUTC_03		0x17

#define MCU_STATUS_POWER_ON	0x80
#define MCU_STATUS_FACTORY_MODE	0x40

#define MCU_STATUS_DEBUGMOD	0x01

#define POWERDOWN_REASON_TMP	0x0E
#define POWERDOWN_REASON_POWER	0x0F
#define	POWERDOWN_REASON_DOG	0x11
#define POWERDOWN_REASON_REBOOT	0x12
#define POWERDOWN_REASON_RESET	0x66
#define POWERDOWN_REASON_POWERDOWN	0xF7

#define POWERDOWN_REASON_RECOVERY 0x15

#define RX_ELMNTS	64

volatile unsigned char I2C_Array[RX_ELMNTS] =
{VERSION,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
INTERNAL_VERSION,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x54,0xDB,0x09,0x5D,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};

#define I2C_slave_address 0x30	//MCU as a slave,Master read as 0x30 ~ 0x37

#define STATUS_POWERUP		1
#define STATUS_POWERDOWN	0
extern int status;

extern unsigned char factorymode;

extern int uart_send;
extern int test_reboot = 1;

void Reset(void);
void Power_Up(void);
void Power_Down(void);
void Initial_FVR(void);
void Initial_sys(void);
void Initial_TIMER();
unsigned int IIC_read_byte(unsigned int dev_addr, unsigned int reg_addr);
unsigned int IIC_write_byte(unsigned int dev_addr, unsigned int reg_addr, unsigned int data);
