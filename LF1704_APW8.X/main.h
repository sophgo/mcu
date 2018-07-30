/*******************************************************************************
PIC16LF18854 BM1682 CORE BOARD
USER DEFINE HEADER CODE 
Author: Patrick Chen(CPF) 
BITMAIN 
DATE: 05/05/2018
/****************************************************************/
#include"xc.h"
#define VERSION			0x00

#define PIC_EN	LATA5


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

#define POWERDOWN_REASON_TMP	0x0E
#define POWERDOWN_REASON_POWER	0x0F
#define	POWERDOWN_REASON_DOG	0x11
#define POWERDOWN_REASON_REBOOT	0x12
#define POWERDOWN_REASON_RESET	0x66
#define POWERDOWN_REASON_POWERDOWN	0xF7



#define RX_ELMNTS	64


unsigned char Timer_Status = 0;

#define I2C_slave_address 0x10//MCU as a slave,Master read as 0x10

#define STATUS_POWERUP		1
#define STATUS_POWERDOWN	0
extern int status;
void Initial_sys(void);

