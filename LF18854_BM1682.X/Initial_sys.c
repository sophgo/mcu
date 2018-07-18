/*******************************************************************************
PIC16LF18854 BM1682 CORE BOARD
SYSTEM INITIAL CODE 
Author: Patrick Chen(CPF) 
BITMAIN 
DATE: 05/05/2018
/****************************************************************/
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

    //////////////////////////////////////////////////////
	// ??
	//////////////////////////////////////////////////////

	//     APFCONbits.T1GSEL=0;//T1G function is on RA4

	EN_RGMII_3V3 = 0;
	EN_VDD_IO_1V8 = 0;
	EN0_ISL68127 = 1;
	EN1_ISL68127 = 1;
	EN_B_3V3 = 0; 
	MCU_ERR_INT = 1;
	SYS_RST = 0;
	//////////////////////////////////////////////////////
	// IO
	//////////////////////////////////////////////////////

	//set port A
	TRISAbits.TRISA0 = 1;	// RA0 is input B1_TEMP_ALR_N
	TRISAbits.TRISA1 = 1;	// RA1 is input PG_B1_VDD_C_0.9V
	TRISAbits.TRISA3 = 1;	// RA3 is input PG_B1_VDDP_1.2V
	TRISAbits.TRISA4 = 0;	// RA4 is output MCU_ERR_INT
	TRISAbits.TRISA5 = 0;	// RA5 is output EN_B_3.3V_MCU
	TRISAbits.TRISA6 = 1;	// RA5 is input TWARN_VDD_C
	TRISAbits.TRISA7 = 0;	// RA5 is output EN_VDD_IO_1.8V_MCU
	ANSELA=0;
	ANSELAbits.ANSA2 = 1;	// RA2 is analog input for comparator C2IN+	
	ANSELA=0b00000100;
	WPUA = 0;				// weak pull-up is disable
	
	//set PORTB
	TRISBbits.TRISB0 = 1;	// set for UART_SEL
	TRISBbits.TRISB1 = 1;	// set for I2C
	TRISBbits.TRISB2 = 1;	// set for I2C
	TRISBbits.TRISB3 = 1;	// set for MCU_IIC_ADDR2
	TRISBbits.TRISB4 = 1;	// set for MCU_IIC_ADDR1
	TRISBbits.TRISB5 = 1;	// set for MCU_IIC_ADDR0
	TRISBbits.TRISB7 = 0;	//RB7 is OUTPUT LED 
	ANSELB=0;
	WPUB=0;

	//set PORTC
	TRISCbits.TRISC0 = 0;	//RC4 is output B1_SYS_RST_MCU
	TRISCbits.TRISC1 = 0;	//RC4 is output EN0_ISL68127
	TRISCbits.TRISC2 = 0;	//RC4 is output EN1_ISL68127
	TRISCbits.TRISC3 = 1;	//RC4 is input MCU_I2C1_SCL
	TRISCbits.TRISC4 = 1;	//RC4 is input MCU_I2C1_SDA
	TRISCbits.TRISC5 = 0;	//RC5 is output EN_RGMII_3V3
	TRISCbits.TRISC6 = 1;	//RC6 is input B1_UART0_RX_TO_MCU
	TRISCbits.TRISC7 = 0;	//RC7 is input B1_UART0_TX
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
     

	MCU_ERR_INT = 1;//clear Error indicate bit
     
	PPSLOCKbits.PPSLOCKED = 0x00; // unlock PPS 
	SSP1DATPPS = 0x14;
	RC4PPS = 0x15; // RC4->MSSP:SDA1
	SSP1CLKPPS = 0x13;
	RC3PPS = 0x14; // RC3->MSSP:SCL1
	SSP2DATPPS = 0x0A;//!!!!SB
	RB1PPS = 0x16; // RB1->MSSP:SCL2
	SSP2CLKPPS = 0x09;//!!!!SB
	RB2PPS = 0x17; // RB2->MSSP:SDA2

	// IIC1 Slave
	SSP1STAT = 0b00000000;       // Slew rate control disabled for standard
	                    // speed mode (100 kHz and 1 MHz)
	SSP1CON1 = 0b00100110; 		// Enable serial port, I2C slave mode,  7-bit address
	SSP1CON2bits.SEN = 0;        // Clock stretching is enabled
	SSP1CON3bits.BOEN = 0;       // SSPBUF is updated and NACK is generated cpf
	SSP1ADD = I2C_slave_address + (RB5 << 1) + (RB4 << 2) + (RB3 << 3); // Load the slave address
	SSP1IF = 0;                  // Clear the serial port interrupt flag
	BCL1IF = 0;                  // Clear the bus collision interrupt flag
	BCL1IE = 1;                  // Enable bus collision interrupts
	PIE3bits.SSP1IE = 1;                  // Enable serial port interrupts



	//IIC2 master config
	SSP2STAT = 0b10000000; 
	SSP2CON1 = 0b00101000;//0x28;
	SSP2CON3 = 0x00;  
	//SSP2ADD = 0x27;
	SSP2ADD = 0x09;		//osc/40=2
	//     SSP2CON3bits.SDAHT = 1;						// Minimum of 300 ns hold time on SDA
										// after the falling edge of SCL

 	//////////////////////////////////////////////////////
	// Interrupt
	//////////////////////////////////////////////////////  
	SSP2IF = 0;                  // Clear the serial port interrupt flag
	BCL2IF = 0;                  // Clear the bus collision interrupt flag
	BCL2IE = 1;                  // Enable bus collision interrupts
	PIE3bits.SSP2IE = 0;		//Synchronous Serial Port (MSSP2) Interrupt Enable bit
	
	//uart recv Interrupt enable
	PIE3bits.RCIE = 1;			//USART Receive Interrupt Enable bit
	
	SSP2BUF = 0;
	PEIE = 1;                   // Enable peripheral interrupts
	C2IE = 1;                   //Enable Compare2 
	GIE = 1; 					// Enable global interrupts
}
