/*******************************************************************************
PIC16LF18854 BM1682 CORE BOARD
SYSTEM INITIAL CODE 
Author: Patrick Chen(CPF) 
BITMAIN 
DATE: 05/05/2018
/****************************************************************/
#include"xc.h"
//#include"delay.h"

void Initial_sys()
{
	//////////////////////////////////////////////////////
	// CLK
	//////////////////////////////////////////////////////
	OSCCONbits.SPLLEN	= 0;		// SPLLEN disabled; 
	OSCCONbits.IRCF		= 0b1111;	// 16 MHz HF
	OSCCONbits.SCS		= 0b00;		// Clock determined by FOSC<2:0> in Configuration Words
	
    // SOSCR disabled; 
    //OSCSTAT = 0x00;
    // TUN 0; 
    //OSCTUNE = 0x00;
	while (OSCSTATbits.HFIOFS == 0);

	
    //////////////////////////////////////////////////////
	// POWER
	//////////////////////////////////////////////////////

	//     APFCONbits.T1GSEL=0;//T1G function is on RA4
	//////////////////////////////////////////////////////
	// IO
	//////////////////////////////////////////////////////

	//set port A
	TRISAbits.TRISA0 = 1;	// RA0 is input IIC ICSPDAT
	TRISAbits.TRISA1 = 1;	// RA1 is input IIC ICSPCLK
	TRISAbits.TRISA2 = 1;	// RA2 is input ADC V-OUT
	TRISAbits.TRISA4 = 1;	// RA4 is input ADC TEMP1
	TRISAbits.TRISA5 = 0;	// RA5 is output PIC_EN
	
	ANSELA=0;				// andlog select
	ANSELAbits.ANSA2 = 1;	// RA2 is analog for V-OUT
	ANSELAbits.ANSA4 = 1;	// RA4 is analog for TEMP1
	
	WPUA = 0;				// weak pull-up is disable
	
	//set PORTB


	//set PORTC
	TRISCbits.TRISC0 = 1;	//RC0 is input SCLL
	TRISCbits.TRISC1 = 1;	//RC1 is input SDAA
	TRISCbits.TRISC2 = 0;	//RC2 is output POPAxOUT DA
	TRISCbits.TRISC3 = 1;	//RC3 is input ADC TEMP2
	TRISCbits.TRISC4 = 1;	//RC4 is input PICEN
	TRISCbits.TRISC5 = 0;	//RC5 is output PWM
	
	ANSELC=0;				// andlog select
	ANSELCbits.ANSC3 = 1;	// RC3 is analog for TEMP2
	WPUC=0;					// weak pull-up is disable
	
 	//////////////////////////////////////////////////////
	// IO Interrupt
	//////////////////////////////////////////////////////    
#if 1
	//     OPTION_REGbits.nWPUEN = 0;//Global pull up
	IOCAP=0;				//disable positive edge interrupt
	IOCAN=0;				//disable negetive edge interrupt
	IOCAF=0;				//Clear interrupt flag


	IOCCP=0;				//disable positive edge interrupt
	IOCCN=0;				//disable positive edge interrupt
	IOCCNbits.IOCCN4 = 1;
	IOCCF=0;				//Clear interrupt flag
	
	IOCIE	= 1;			// Interrupt-on-Change Enable bit
	//PEIE	= 0;			// Peripheral Interrupt Enable bit
	GIE		= 1;			// Global Interrupt Enable 
#endif
	//MCU_ERR_INT = 1;//clear Error indicate bit

#if 1
	//////////////////////////////////////////////////////
	// PERIPHERAL PIN SELECT
	//////////////////////////////////////////////////////
	PPSLOCKbits.PPSLOCKED = 0x00; // unlock PPS 
	
	// IIC
	SSPCLKPPS	= 0b10000;		//RC0->IIC_CLK
	SSPDATPPS	= 0b10001;		//RC1->IIC_DAT
	RC0PPS		= 0b10000;		//IIC_CLK->RC0
	RC1PPS		= 0b10001;		//IIC_SDA->RC1

	

	PPSLOCKbits.PPSLOCKED = 0x01; // lock PPS 
#endif
	// IIC1 Slave

 	//////////////////////////////////////////////////////
	// Interrupt
	//////////////////////////////////////////////////////  
	
}
