/*******************************************************************************
PIC16LF18854 BM1682 CORE BOARD
SYSTEM INITIAL CODE 
Author: Patrick Chen(CPF) 
BITMAIN 
DATE: 05/05/2018
/****************************************************************/
#include"xc.h"
#include"base.h"

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

	power_off();
	thermal_off();
	power_led_off();
	thermal_led_off();
	//set port A
	TRISAbits.TRISA0 = 1;	// RA0 is input IIC ICSPDAT
	TRISAbits.TRISA1 = 1;	// RA1 is input IIC ICSPCLK
	TRISAbits.TRISA2 = 1;	// RA2 is input adapter_in
	TRISAbits.TRISA4 = 0;	// RA4 is output TRM_LED
	TRISAbits.TRISA5 = 1;	// RA5 is input OVER_TEMP
	
	ANSELA=0;				// andlog select
	WPUA = 0;				// weak pull-up is disable
	
	//set PORTB


	//set PORTC
	TRISCbits.TRISC0 = 1;	//RC0 is input POWER_KEY
	TRISCbits.TRISC1 = 0;	//RC1 is output	THERMAL_OFF
	PORTCbits.RC1 = 1;		//close thermal on init
	TRISCbits.TRISC2 = 0;	//RC2 is output	POWER_ON
	TRISCbits.TRISC3 = 1;	//RC3 is input UNDER_TEMP
	TRISCbits.TRISC4 = 1;	//RC4 is input PMEB
	TRISCbits.TRISC5 = 0;	//RC5 is output	POWER_LED

	ANSELC=0;				// andlog select
	WPUC=0;					// weak pull-up is disable
	
 	//////////////////////////////////////////////////////
	// Interrupt
	//////////////////////////////////////////////////////    
#if 1
	//     OPTION_REGbits.nWPUEN = 0;//Global pull up
	
	IOCAP=0;				// PORTA POSITIVE EDGE REGISTER
	IOCAN=0;				// PORTA NEGATIVE EDGE REGISTER
	IOCAF=0;				// PORTA FLAG REGISTER
	//IOCAPbits.IOCAP0	= 1;
	//IOCANbits.IOCAN0	= 1;
	//IOCAFbits.IOCAF0	= 0;

	IOCCP=0;				// PORTC POSITIVE EDGE REGISTER
	IOCCN=0;				// Negative Edge Enable bits
	IOCCF=0;				//Clear interrupt flag
	

	INTCONbits.PEIE		= 1;			// Peripheral Interrupt Enable bit

	// Timer0 interrupt
	INTCONbits.TMR0IE	= 1;			// Timer0 Overflow Interrupt Enable bit
	INTCONbits.INTE		= 0;			//
	INTCONbits.IOCIE	= 1;			// Interrupt-on-Change Enable bit

	INTCONbits.GIE		= 1;			// Global Interrupt Enable 
#endif
	

#if 0
	//////////////////////////////////////////////////////
	// PERIPHERAL PIN SELECT
	//////////////////////////////////////////////////////
	PPSLOCKbits.PPSLOCKED = 0x00; // unlock PPS 
	
	// IIC slave
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
