/*******************************************************************************
PIC16F1508 BM1682 SINGLE CHIP
TIMER AND NCO CODE 
Author: Patrick Chen(CPF) 
BITMAIN 
DATE: 21/03/2018
/****************************************************************/
#include"xc.h"
#include"User_define.h"
//#include"delay.h"
#define I2C_slave_address 0x2E//Master read as 0x17

 void Initial_sys()
{
     OSCCON = 0b01111010; // set 4MHZ internal clock
     while(OSCSTATbits.HFIOFR==0);// wait colck stable
//     APFCONbits.T1GSEL=0;//T1G function is on RA4

     EN_RGMII_3V3 = 1;//for new single chip
     EN_VDD_IO_1V8 = 0;
     EN_B_DDR4_1V2 = 1;//for Single & new dual chip
     EN_ISL68127 = 1;
     EN_B_3V3 = 0; 
     MCU_ERR_INT = 0;
     SYS_RST = 0;
     //set port A
     TRISAbits.TRISA0 = 0;// RA0 is output LED0
     TRISAbits.TRISA1 = 1;// RA1 is input RST_REQ
     TRISAbits.TRISA2 = 1;// RA2 is input B_TEMP_ALR_N
     TRISAbits.TRISA3 = 1;// RA3 is input POWER_EN
     TRISAbits.TRISA4 = 1;// RA2 is input FAN_TACH//0 for debug only
     TRISAbits.TRISA5 = 1;// RA5 is output PG_DDR4_1V2
     ANSELA = 0; // set all port is digital port
     WPUA = 0;// weak up is disable
     OPTION_REGbits.nWPUEN = 0;//Globle pull up
     IOCAP=0;//disable positive edge interrupt
     IOCAN=0;//disable negetive edge interrupt
     IOCAF=0;//Clear interrupt flag

     //set PORTB
     TRISBbits.TRISB4=1; // set for I2C
     TRISBbits.TRISB6=1; // set for I2C
     TRISBbits.TRISB5=1;//RB5 is input TWARN_VDD_C 
     TRISBbits.TRISB7=1; //RB7 is input PG_VDD_C 
     ANSELB=0;
     WPUB=0;
     IOCBP=0;//disable positive edge interrupt
     IOCBN=0;//disable positive edge interrupt
     IOCBF=0;//Clear interrupt flag
     
     //set PORTC   
     EN_B_DDR4_1V2 = 1;//for Single & new dual chip
//     EN_ISL68127 = 1;
     LATCbits.LATC4 = 1;
     LATCbits.LATC5 = 1;
     TRISCbits.TRISC1=0;//RC1 is output MCU_ERR_INT
     TRISCbits.TRISC2=0;//RC2 is output EN_B_3V3
     TRISCbits.TRISC3=0;//RC3 is output EN_VDD_IO_1V8
     TRISCbits.TRISC4=0;//RC4 is output EN_B_DDR4_1V2
     TRISCbits.TRISC5=0;//RC5 is output EN_ISL68127
     TRISCbits.TRISC6=0;//RC6 is output EN_RGMII_3V3
     TRISCbits.TRISC7=0;//RC7 is output SYS_RST
     ANSELC=0;
     ANSELCbits.ANSC0=1; // RC0 is analog input for comparator C2IN+
     EN_B_DDR4_1V2 = 1;////for Single & new dual chip
     EN_ISL68127 = 1;


     
	 //IIC config 
     SSP1STAT = 0b00000000;       // Slew rate control disabled for standard
                                // speed mode (100 kHz and 1 MHz)
     SSP1CON1 = 0b00100110; 		// Enable serial port, I2C slave mode,  7-bit address
     SSP1CON2bits.SEN = 0;        // Clock stretching is enabled
     SSP1CON3bits.BOEN = 0;       // SSPBUF is updated and NACK is generated cpf
     SSP1ADD = I2C_slave_address; // Load the slave address

     SSP1IF = 0;                  // Clear the serial port interrupt flag
     BCL1IF = 0;                  // Clear the bus collision interrupt flag
     BCL1IE = 1;                  // Enable bus collision interrupts
     PIE1bits.SSP1IE = 1;                  // Enable serial port interrupts
     
     
     PEIE = 1;                   // Enable peripheral interrupts
     C2IE = 1;                   //Enable Compare2
     GIE = 1; 					// Enable global interrupts
//     EN1_ISL68124 = 0;
//     delayms(5);
//     delay5us(100);
//     EN1_ISL68124 = 1;
     
//     EN1_ISL68124 = 0;
     
}
