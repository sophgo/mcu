/*
 * File:   Initial_sys.c
 * Author: taiqiang.cao
 *
 * Created on June 3, 2019, 2:39 PM
 */

#include"xc.h"
#include"User_define.h"
//#include"delay.h"

void Initial_sys()
{
	//////////////////////////////////////////////////////
	// CLK
	//////////////////////////////////////////////////////
 	//HFINTOSC Frequency Selection bits:16MHz 
    OSCFRQbits.HFFRQ = 0b101;	
    
    // set new osc and div
    OSCCON1bits.NOSC = 0b110;	//select NOSC HFINTOSC
    OSCCON1bits.NDIV = 0b0000;
	
	//HFINTOSC is explicitly enabled
    OSCENbits.HFOEN = 1;
	// wait colck stable
	while(OSCSTATbits.HFOR == 0);//HFINTOSC Oscillator Ready bit


	//MFINTOSC Oscillator Manual Request Enable bit
	OSCENbits.MFOEN = 1;
	// wait colck stable
	while(OSCSTATbits.MFOR == 0);//MFINTOSC Oscillator Ready bit
	
    //////////////////////////////////////////////////////
	// POWER
	//////////////////////////////////////////////////////

	//     APFCONbits.T1GSEL=0;//T1G function is on RA4

//	EN_RGMII_3V3 = 0;
//	EN_VDD_IO_1V8 = 0;
//	EN0_ISL68127 = 1;
//	EN1_ISL68127 = 1;
//	EN_B_3V3 = 0; 
//	MCU_ERR_INT = 1;
//	SYS_RST = 0;
	//////////////////////////////////////////////////////
	// IO
	//////////////////////////////////////////////////////

	//set port A
	TRISAbits.TRISA0 = 0;	// RA0 is output SYS_GOOD
	TRISAbits.TRISA1 = 0;	// RA1 is output SYS_FAULT
    TRISAbits.TRISA2 = 0;	// RA1 is output PWR_GOOD
	TRISAbits.TRISA3 = 0;	// RA3 is output PWR_FAULT
	TRISAbits.TRISA4 = 0;	// RA4 is output ETH_GOOD
	TRISAbits.TRISA5 = 0;	// RA5 is output ETH_FAULT
    ANSELA=0;
	WPUA = 0;				// weak pull-up is disable
	
	//set PORTB
	TRISBbits.TRISB0 = 1;	// set for LED_BD_PWR
	TRISBbits.TRISB1 = 1;	// set for I2C LED_BD_SCL_F
	TRISBbits.TRISB2 = 1;	// set for I2C LED_BD_SDA_F
	TRISBbits.TRISB3 = 1;	// set for LED_BD_UID
	TRISBbits.TRISB4 = 0;	// set for FAN_GOOD
	TRISBbits.TRISB5 = 0;	// set for FAN_FAULT
    TRISBbits.TRISB6 = 1;	// set for ICSPCLK
	TRISBbits.TRISB7 = 0;	//RB7 is ICSPDAT 
	ANSELB=0;
	WPUB=0;

	//set PORTC
	TRISCbits.TRISC0 = 0;	//RC4 is output HDD_GOOD
	TRISCbits.TRISC1 = 0;	//RC4 is output HDD_FAULT
	TRISCbits.TRISC2 = 0;	//RC4 is output LED_PWR_P
	TRISCbits.TRISC3 = 1;	//RC4 is input MCU_I2C1_SCL
	TRISCbits.TRISC4 = 1;	//RC4 is input MCU_I2C1_SDA
	TRISCbits.TRISC5 = 0;	//RC5 is output LED_PWR_N
	TRISCbits.TRISC6 = 0;	//RC6 is input LED_UID_N
	TRISCbits.TRISC7 = 0;	//RC7 is input LED_UID_P
	ANSELC=0;
	WPUB=0;
 	//////////////////////////////////////////////////////
	// IO Interrupt
	//////////////////////////////////////////////////////    
	//     OPTION_REGbits.nWPUEN = 0;//Global pull up
	IOCAP=0;				//disable positive edge interrupt
	IOCAN=0;				//disable negetive edge interrupt
	IOCAF=0;				//Clear interrupt flag


	IOCBP=0;//disable positive edge interrupt
	IOCBN=0;//disable positive edge interrupt
	IOCBF=0;//Clear interrupt flag
     

//	MCU_ERR_INT = 1;//clear Error indicate bit
     
	PPSLOCKbits.PPSLOCKED = 0x00; // unlock PPS 
	SSP1DATPPS = 0x14;
	RC4PPS = 0x15; // RC4->MSSP:SDA1
	SSP1CLKPPS = 0x13;
	RC3PPS = 0x14; // RC3->MSSP:SCL1
	SSP2DATPPS = 0x0A;//!!!!SB
	RB1PPS = 0x16; // RB1->MSSP:SCL2
	SSP2CLKPPS = 0x09;//!!!!SB
	RB2PPS = 0x17; // RB2->MSSP:SDA2

	// IIC2 Slave
	SSP2STAT = 0b10000000;       // Slew rate control disabled for standard
	                    // speed mode (100 kHz and 1 MHz)
	SSP2CON1 = 0b00100110; 		// Enable serial port, I2C slave mode,  7-bit address
	SSP2CON2bits.SEN = 0;        // Clock stretching is enabled
	SSP2CON3bits.BOEN = 0;       // SSPBUF is updated and NACK is generated cpf
    SSP2ADD = I2C_slave_address; // Load the slave address
	SSP2IF = 0;                  // Clear the serial port interrupt flag
	BCL2IF = 0;                  // Clear the bus collision interrupt flag
	BCL2IE = 1;                  // Enable bus collision interrupts
	SSP2IE = 1;                  // Enable serial port interrupts

	//uart recv Interrupt enable
	PIE3bits.RCIE = 1;			//USART Receive Interrupt Enable bit
	
	SSP2BUF = 0;
	PEIE = 1;                   // Enable peripheral interrupts
	C2IE = 1;                   //Enable Compare2 
	GIE = 1; 					// Enable global interrupts


	
}
