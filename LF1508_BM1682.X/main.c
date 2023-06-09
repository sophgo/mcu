/*******************************************************************************
PIC16F1508 BM1682 SINGLE CHIP
MAIN CODE 
Author: Patrick Chen(CPF) 
BITMAIN 
DATE: 21/03/2018
/****************************************************************/

// PIC16LF1508 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1
#pragma config FOSC = INTOSC    // Oscillator Selection Bits (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = SWDTEN    // Watchdog Timer Enable (WDT controlled by the SWDTEN bit in the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable (Brown-out Reset disabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = OFF       // Internal/External Switchover Mode (Internal/External Switchover Mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is disabled)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LPBOR = OFF      // Low-Power Brown Out Reset (Low-Power BOR is disabled)
#pragma config LVP = ON         // Low-Voltage Programming Enable (Low-voltage programming enabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <pic.h> 
#include <xc.h>
#include "User_define.h"
#include "Flash.h"
#include "HEFlash.h"
// work with __delay_ms __delay_us
#define _XTAL_FREQ 16000000


// array for master to write to, reserved
volatile unsigned char I2C_Array_Rx[RX_ELMNTS] = 0;
//unsigned char I2C_Array_Rx[RX_ELMNTS] = 0;	
//slave IIC variable
unsigned char index_i2c = 0;     // index pointer
unsigned char junk = 0;          // used to place unnecessary data
unsigned char first = 1;               // used to determine whether data address 
                                       // location or actual data
unsigned char LED_Status = 0;
unsigned long int Sencond_Count = 0;
//unsigned long int MEM_write = 0x1234ABCD;
char MEM_write[4] = {0x12,0x34,0xAB,0xCD};
char MEM_read[4];
//char MEM_p[4];
int r1, r2;
void initialize(void);           // initialize routine


int needpowerup = 0;
int needbite = 0;

unsigned long int	last_feed_time	= 0;
static char			watch_dog_run	= 0;
int watch_dog_isrun()
{
	return watch_dog_run;
}
int watch_dog_isbite()
{
	return Sencond_Count-last_feed_time >  I2C_Array[INDEX_DOG_TIME_OUT];
}
void watch_dog_stop()
{
	I2C_Array[INDEX_DOG_TIME_OUT] = 0;
	watch_dog_run = 0;
}
void watch_dog_start(unsigned char time)
{
	I2C_Array[INDEX_DOG_TIME_OUT] = time;
	last_feed_time = Sencond_Count;
	watch_dog_run = 1;
}
void watch_dog_feed()
{
	last_feed_time = Sencond_Count;
}

void doreboot()
{
	GIE = 0;
	IOCIE = 0;
	Power_Down();
	delayms(1000);
	Power_Up();
	delayms(200);
	GIE = 1;
	IOCIE = 1;
}
void doreset()
{
	GIE = 0;
	IOCIE = 0;
	Reset();
	GIE = 1;
	IOCIE = 1;
}
void dopowerdown()
{
	GIE = 0;
	IOCIE = 0;
	Power_Down();
	GIE = 1;
	IOCIE = 1;
}
void mcu_watch_dog_start()
{
	//Watchdog Timer Clock Select bits
	// force 31 kHz LFINTOSC internal oscillator
	// Watchdog Timer Prescale Select bits
	WDTCONbits.WDTPS	= 0b01100;//Interval 4s nominal

	WDTCONbits.SWDTEN	= 1;//active watch dog

}
void mcu_watch_dog_feed()
{
	asm("CLRWDT");
}

/**************************** MAIN ROUTINE ************************************/
void main(void)
{
    Initial_sys();
    Initial_FVR();
    Initial_TMR1_FAN();
    Initial_NCO();
    Power_Up();
    r2 = HEFLASH_readBlock(I2C_Array+INDEX_LTIME_L, 0, 2);//read last time work time
    delayms(1000);//wait 1 second for stable
//    r1 = HEFLASH_writeBlock( 1,MEM_write+2 , 2);
	mcu_watch_dog_start();
    if(B_TEMP_ALR_N & PG_DDR4_1V2 & TWARN_VDD_C & PG_VDD_C)//system Power and Temperature OK
    {
        LED0 = 1;
    }
    else
    {
        LED_Status = 1;
        MCU_ERR_INT = 1;
        if(!B_TEMP_ALR_N)
        {
            I2C_Array[INDEX_SYS_TMP_ST] = I2C_Array[INDEX_SYS_TMP_ST] | 0x01;
        }
        if(!PG_DDR4_1V2)
        {
            I2C_Array[INDEX_SYS_VOL_ST] = I2C_Array[INDEX_SYS_VOL_ST] | 0x02;
        }
        if(!PG_VDD_C)
        {
            I2C_Array[INDEX_SYS_VOL_ST] = I2C_Array[INDEX_SYS_VOL_ST] | 0x01;
        }
        if(!TWARN_VDD_C)
        {
            I2C_Array[INDEX_SYS_TMP_ST] = I2C_Array[INDEX_SYS_TMP_ST] | 0x02;
        }        
    }
    //when not use pickit
    IOCAP = 0b101010;//Enable PORTA positive edge interrupt RA2 temporary disable
    IOCAN = 0b101010;//Enable PORTA negetive edge interrupt
    IOCBP = 0b10100000;//Enable PORTB positive edge interrupt
    IOCBN = 0b10100000;//Enable PORTB positive edge interrupt
/* 
    //when use pickit, debug only
    IOCAP = 0b00100;//Enable PORTA positive edge interrupt
    IOCAN = 0b00100;//Enable PORTA negetive edge interrupt    
    IOCBP = 0b10100000;//Enable PORTB positive edge interrupt
    IOCBN = 0b10100000;//Enable PORTB positive edge interrupt
 */
    IOCIE = 1;//after all power on, enable IO interrupt

    while(1)                    // main while() loop
    {                           // program will wait here for ISR to be called
		mcu_watch_dog_feed();
		// handle I2C cmd
        switch(I2C_Array[INDEX_INSTRUCTION])
        {
//            case(0x2C):
//                Initial_TMR1_FAN();
//                T1GGO_nDONE = 1;
 //               break;
 			case(0x0D):
				//enable watch dog debug mode
				I2C_Array[INDEX_MCU_STATUS] |= 0x01;
				break;
			case(0x11):
				watch_dog_feed();
				I2C_Array[INDEX_INSTRUCTION] = 0;
				break;
            case(0x12)://reboot
				watch_dog_stop();
				I2C_Array[INDEX_POWERDOWN_REASON] = POWERDOWN_REASON_REBOOT;
				doreboot();
				I2C_Array[INDEX_RESET_COUNT]++;
                I2C_Array[INDEX_INSTRUCTION] = 0;
                break;
            case(0x66)://BM1682 reset
	            watch_dog_stop();
				I2C_Array[INDEX_POWERDOWN_REASON] = POWERDOWN_REASON_RESET;
				doreset();
                I2C_Array[INDEX_RESET_COUNT]++;
                I2C_Array[INDEX_INSTRUCTION] = 0;
                break;                
            case(0xF7)://System power down
	            watch_dog_stop();
				I2C_Array[INDEX_POWERDOWN_REASON] = POWERDOWN_REASON_POWERDOWN;
				dopowerdown();
                I2C_Array[INDEX_INSTRUCTION] = 0;
                break;
            case(0x86)://Clear abnormal status
                MCU_ERR_INT = 0;
                I2C_Array[INDEX_INSTRUCTION] = 0;
                break;
        }
        if(Sencond_Count % 2 == 0)//measure Fan speed every 2s
        {
            if(T1GGO_nDONE == 1)//last measurement error
            {
                MCU_ERR_INT = 1;
                I2C_Array[INDEX_FAN_ST] = 0x01;
            }  
            else
                T1GGO_nDONE = 1;
        }
		if (needpowerup)
		{
			delayms(50);
			if(C2OUT == 1)
			{
				//I2C_Array[0] = 0xB0;
				MCU_ERR_INT = 1;
				Power_Up();
			}
			needpowerup = 0;
		}
		if (needbite == 1)
		{
			if (I2C_Array[INDEX_MCU_STATUS] & 0x01)
			{
				doreset();
			}
			else
			{
				doreboot();
			}
			
			needbite = 0;
		}
   }
}
unsigned char low_power_count = 0;
void interrupt ISR(void) 
{
    if (SSP1IF)                              // check to see if SSP interrupt I2C
    {
    	// fix IIC stuck bug
    	if (SSP1CON1bits.SSPOV)
    	{
    		SSP1CON1bits.SSPOV = 0;
    	}
        if (SSP1STATbits.R_nW)               // Master read (R_nW = 1)
        {
            if (!SSP1STATbits.D_nA)        // Last byte was an address (D_nA = 0)
            {
                SSP1BUF = I2C_Array[index_i2c++]; // load with value from array
                SSP1CON1bits.CKP = 1;             // Release CLK
            }
            else//if (SSP1STATbits.D_nA)               // Last byte was data (D_nA = 1)
            {
                SSP1BUF = I2C_Array[index_i2c++]; // load with value from array
                SSP1CON1bits.CKP = 1;             // Release CLK
            }

        }
        else //if (!SSP1STATbits.R_nW) //  Master write (R_nW = 0)
        {
            if (!SSP1STATbits.D_nA) // Last byte was an address (D_nA = 0)
            {
                first = 1; //last byte was address, next will be data location
                junk = SSP1BUF;                  // read buffer to clear BF
                SSP1CON1bits.CKP = 1;            // Release CLK
            }
            else//if (SSP1STATbits.D_nA)               // Last byte was data (D_nA = 1)
            {
                if (first) 
                {
                    index_i2c = SSP1BUF;      // load index with array location
                    first = 0;               // now clear this since we have 
                }                            //location to read from/write to
                else
                {
                    if (index_i2c < RX_ELMNTS)       // make sure index is not
                    {                                //out of range of array
                        I2C_Array[index_i2c++] = SSP1BUF; //load array with data
                    } 
                    else
                    {
                        junk = SSP1BUF; //array location not valid, discard data
                    }
                }
                if (SSP1CON1bits.WCOL)           // Did a write collision occur?
                {
                    SSP1CON1bits.WCOL = 0;       //  clear WCOL
                    junk = SSP1BUF;              // dummy read to clear BF bit
                }
                SSP1CON1bits.CKP = 1;            // Release CLK
            }
        }
         SSP1IF = 0;                              // clear SSP1IF flag bit
    }
    if (BCL1IF)                                  // Did a bus collision occur?
    {
        junk = SSP1BUF;                      // dummy read SSP1BUF to clear BF bit
        BCL1IF = 0;                          // clear bus collision Int Flag bit
        SSP1CON1bits.CKP = 1;                // Release CLK
    }
    if(NCO1IF)//NCO overflow interrupt
    {
        if(MCU_ERR_INT)
            LED0 = ~LED0;
        Sencond_Count++;
        I2C_Array[INDEX_TIME_L] = Sencond_Count;
        I2C_Array[INDEX_TIME_H] = Sencond_Count >> 8;
        NCO1IF = 0;
		if (watch_dog_isrun())
		{
			if ( I2C_Array[INDEX_DOG_TIME_OUT])
			{
				if ( watch_dog_isbite())
				{
					I2C_Array[INDEX_POWERDOWN_REASON] = POWERDOWN_REASON_DOG;
					needbite = 1;
					watch_dog_stop();

				}
			
			}
			else//request to stop wd
			{
				watch_dog_stop();
			}
		}
		else
		{
			if ( I2C_Array[INDEX_DOG_TIME_OUT])//request to start wd
			{
				watch_dog_start(I2C_Array[INDEX_DOG_TIME_OUT]);

			}
		}
    }
	
    if (C2IF)//12V down or up
    {
        if (C2OUT == 1)//power down
        {
        	/*
			for (low_power_count=0; low_power_count < 3; low_power_count++)
			{
				__delay_ms(1);
				if (C2OUT == 0)
				{
					break;
				}
			}
			if (low_power_count == 3)
			{
				I2C_Array[INDEX_POWERDOWN_REASON] = POWERDOWN_REASON_POWER;
	            r1 = HEFLASH_writeBlock( 0,I2C_Array+INDEX_TIME_L , 2);
	            Power_Down();
			}
			//*/
			C2IF = 0;//Clear interrupt bit
        }
        else 
        {
        	/*
        	if (I2C_Array[INDEX_POWERDOWN_REASON] == POWERDOWN_REASON_POWER)//voltage too low to normal, reboot
	        {
				needpowerup = 1;
        	}
            low_power_count = 0;
			//*/
			C2IF = 0;
        }

    }
    if(IOCAF1)//RST Request   
    {
        delayms(5);
        if(RST_REQ == 0)
        {
            SYS_RST = 0;
            LED0 = 0;
            delayms(20);//wait 1 second for stable
        }
        else if(RST_REQ == 1)
        {
            SYS_RST = 1;
            I2C_Array[INDEX_RESET_COUNT]++;
            delayms(20);//wait 1 second for stable
            LED0 = 1;
        }
        IOCAF1 = 0;
        IOCIF = 0;
    }
    
    if(IOCAF2)//BM1682 Temperature too high
    {
        delayms(5);
        if(B_TEMP_ALR_N == 0)
        {
            MCU_ERR_INT = 1;
            I2C_Array[INDEX_SYS_TMP_ST] = I2C_Array[INDEX_SYS_TMP_ST] | 0x01;
        }
        else if(B_TEMP_ALR_N == 1)
        {
             I2C_Array[INDEX_SYS_TMP_ST] = I2C_Array[INDEX_SYS_TMP_ST] & 0xFE;//clear bit
        }
        IOCAF2 = 0;
    }
    
    if(IOCAF5)//DDR4 1.2V abnormal
    {
        delayms(5);
        if(PG_DDR4_1V2 == 0)
        {
            MCU_ERR_INT = 1;
            I2C_Array[INDEX_SYS_VOL_ST] = I2C_Array[INDEX_SYS_VOL_ST] | 0x02;
        }
        else if(PG_DDR4_1V2 == 1)
        {
            I2C_Array[INDEX_SYS_VOL_ST] = I2C_Array[INDEX_SYS_VOL_ST] & 0xFD;
        }
        IOCAF5 = 0;
    }
    
    if(IOCBF5)//ISL68127 temperature abnormal
    {
        delayms(5);
        if(TWARN_VDD_C == 0)
        {
            MCU_ERR_INT = 1;
            I2C_Array[INDEX_SYS_TMP_ST] = I2C_Array[INDEX_SYS_TMP_ST] | 0x02;
        }
        else if(TWARN_VDD_C == 1)
        {
            I2C_Array[INDEX_SYS_TMP_ST] = I2C_Array[INDEX_SYS_TMP_ST]  & 0xFD;
        }
        IOCBF5 = 0;
    }
    
    if(IOCBF7)//ISL68127 voltage abnormal
    {
        delayms(5);
        if(PG_VDD_C == 0)
        {
            MCU_ERR_INT = 1;
            I2C_Array[INDEX_SYS_VOL_ST] = I2C_Array[INDEX_SYS_VOL_ST] | 0x01;
        }
        else if(PG_VDD_C == 1)
        {
            I2C_Array[INDEX_SYS_VOL_ST] = I2C_Array[INDEX_SYS_VOL_ST] & 0xFE;
        }
        IOCBF7 = 0;
    }
    
    if(TMR1GIF)//fan speed measure
    {
        I2C_Array[INDEX_FAN_SPEEDS_L] = TMR1L;
        I2C_Array[INDEX_FAN_SPEEDS_H] = TMR1H;
        TMR1H = 0b0;
        TMR1L = 0b0;
        I2C_Array[INDEX_FAN_ST] = 0x0;
        TMR1GIF = 0;
    }
    if(TMR1IF)//TM1 overflow, something wrong with FAN
    {
        MCU_ERR_INT = 1;
        TMR1IF = 0;
        I2C_Array[INDEX_FAN_ST] = 0x01;
    }
        
}// end of ISR 

