/* 
 * File:   User_define.h
 * Powered by: Pengfei Chen(Patrick)
 *
 * V0.1
 * Created on Jan 02, 2018
 * Intial
 */
#include "xc.h"

#define VERSION			0x07

#define    TMR0_Value     177
#define __16F1508
//LED control
#define LED0 PORTAbits.RA0 				
#define RST_REQ PORTAbits.RA1 			
//B1_TEMP_ALR_N
#define B_TEMP_ALR_N PORTAbits.RA2		
//MCLR    use MCLR function to control Power EN, do not need config more
//#define POWER_EN PORTAbits.RA3 		
//FAN_TACH
#define FAN_TACH PORTAbits.RA4 
//PG_B1_VDDP_1.2V
#define PG_DDR4_1V2 PORTAbits.RA5 		
//MCU_I2C_SDA
//PORTBbits.RB4							
//TWARN_VDD_C
#define TWARN_VDD_C PORTBbits.RB5		
//MCU_I2C_SCL
//PORTBbits.RB6							
//PG_B1_VDD_C_0.9V
#define PG_VDD_C PORTBbits.RB7			
#if 1
//PORTCbits.RC0
#define MCU_ERR_INT LATC1
#define EN_B_3V3 LATC2
#define EN_VDD_IO_1V8 LATC3				
#define EN_B_DDR4_1V2 LATC4
#define EN_ISL68127 LATC5 
#define EN_RGMII_3V3 LATC6 
#define SYS_RST LATC7
#else
//PORTCbits.RC0							
//MCU_ERR_INT 
#define MCU_ERR_INT PORTCbits.RC1		
//EN_B_3.3V_MCU
#define EN_B_3V3 PORTCbits.RC2 			
//EN_VDD_IO_1.8V_MCU
#define EN_VDD_IO_1V8 PORTCbits.RC3		
//EN1_ISL68124
#define EN_B_DDR4_1V2 PORTCbits.RC4		
//EN0_ISL68124
#define EN_ISL68127 PORTCbits.RC5 		
//EN_RGMII_3.3V_MCU
#define EN_RGMII_3V3 PORTCbits.RC6		
//B1_SYS_RST_MCU
#define SYS_RST PORTCbits.RC7			
#endif

#define INDEX_VERSION		0
#define INDEX_INSTRUCTION	1
#define INDEX_FAN_SPEEDS_L	2
#define INDEX_FAN_SPEEDS_H	3
#define INDEX_SYS_VOL_ST	4
#define INDEX_SYS_TMP_ST	5
#define INDEX_12V_ST		6
#define INDEX_RESET_COUNT	7
#define INDEX_FAN_ST		8
#define INDEX_TIME_L		9
#define INDEX_TIME_H		10
#define INDEX_LTIME_L		11
#define INDEX_LTIME_H		12
#define INDEX_DOG_TIME_OUT	13
#define INDEX_POWERDOWN_REASON 15

#define POWERDOWN_REASON_TMP	0x0E
#define POWERDOWN_REASON_POWER	0x0F
#define	POWERDOWN_REASON_DOG	0x11
#define POWERDOWN_REASON_REBOOT	0x12
#define POWERDOWN_REASON_RESET	0x66
#define POWERDOWN_REASON_POWERDOWN	0xF7




#define RX_ELMNTS	32
//void Initial_sys(void);
//void Initial_FVR(void);
// array for master to read from
volatile unsigned char I2C_Array[RX_ELMNTS] =
{VERSION,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
unsigned char Timer_Status = 0;


void delay5us(int time);
void delayms(long int time);
void delays(int time);
void Reset(void);
void Power_Up(void);
void Power_Down(void);
void Initial_FVR(void);
void Initial_sys(void);
void Initial_TMR1_FAN(void);
void Initial_TMR1_TIME(void);
void Initial_NCO();