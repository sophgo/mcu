#include "IIC.h"
#include <xc.h>
void IIC1_slave_init(int slave_address)
{
	SSP1STATbits.SMP	= 0;	//Slew rate control disabled for Standard Speed mode (100 kHz and 1 MHz)
	SSP1STATbits.CKE	= 0;	//Disable SMBus specific inputs

	SSP1CON1bits.CKP	= 0;	//Holds clock low
	SSP1CON1bits.SSPM	= 0b0110;// I2C Slave mode, 7-bit address

	SSP1CON2bits.GCEN	= 0;	//General call address disabled
	SSP1CON2bits.SEN	= 0;	// Clock stretching is enabled

	SSP1CON3bits.PCIE	= 0;	// Stop detection interrupts are disabled
	SSP1CON3bits.SCIE	= 0;	//Start detection interrupts are disabled
	SSP1CON3bits.BOEN	= 0;	//SSPBUF is only updated when SSPOV is clear
	SSP1CON3bits.SDAHT	= 0;	//Minimum of 100 ns hold time on SDA after the falling edge of SCL
	SSP1CON3bits.SBCDE	= 0;	//Slave bus collision interrupts are disabled
	SSP1CON3bits.AHEN	= 0;	//Address holding is disabled
	SSP1CON3bits.DHEN	= 0;	//Data holding is disabled
	
	SSP1ADD = (slave_address << 1); // Load the slave address
	SSP1IE = 1;
	SSP1IF = 0;                  // Synchronous Serial Port (MSSP) Interrupt Flag bit
	BCL1IF = 0;                  // MSSP Bus Collision Interrupt Flag bit
	BCL1IE = 1;                  // MSSP Bus Collision Interrupt Enable bit


	SSP1CON1bits.SSPEN	= 1;//Synchronous Serial Port Enable bit
}