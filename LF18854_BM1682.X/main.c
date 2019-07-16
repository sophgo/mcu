
/*******************************************************************************
PIC16LF18854 BM1682 CORE BOARD
MAIN CODE 
Author: Patrick Chen(CPF) 
BITMAIN 
DATE: 05/05/2018
/****************************************************************/


// CONFIG1
#pragma config FEXTOSC = OFF    // External Oscillator mode selection bits (Oscillator not enabled)
#pragma config RSTOSC = HFINT1  // Power-up default value for COSC bits (HFINTOSC (1MHz))
#pragma config CLKOUTEN = OFF   // Clock Out Enable bit (CLKOUT function is disabled; i/o or oscillator function on OSC2)
#pragma config CSWEN = ON      // Clock Switch Enable bit (The NOSC and NDIV bits cannot be changed by user software)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable bit (FSCM timer enabled)

// CONFIG2
#pragma config MCLRE = ON       // Master Clear Enable bit (MCLR pin is Master Clear function)
#pragma config PWRTE = ON      // Power-up Timer Enable bit (PWRT disabled)
#pragma config LPBOREN = OFF    // Low-Power BOR enable bit (ULPBOR disabled)
#pragma config BOREN = OFF      // Brown-out reset enable bits (Brown-out reset disabled)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (VBOR) set to 1.9V on LF, and 2.45V on F Devices)
#pragma config ZCD = OFF        // Zero-cross detect disable (Zero-cross detect circuit is disabled at POR.)
#pragma config PPS1WAY = OFF    // Peripheral Pin Select one-way control (The PPSLOCK bit can be set and cleared repeatedly by software)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable bit (Stack Overflow or Underflow will cause a reset)

// CONFIG3
#pragma config WDTCPS = WDTCPS_31// WDT Period Select bits (Divider ratio 1:65536; software control of WDTPS)
#pragma config WDTE = SWDTEN	// WDT operating mode (WDT Disabled, SWDTEN is ignored)
#pragma config WDTCWS = WDTCWS_7// WDT Window Select bits (window always open (100%); software control; keyed access not required)
#pragma config WDTCCS = SC      // WDT input clock selector (Software Control)

// CONFIG4
#pragma config WRT = OFF        // UserNVM self-write protection bits (Write protection off)
#pragma config SCANE = available// Scanner Enable bit (Scanner module is available for use)
#pragma config LVP = ON         // Low Voltage Programming Enable bit (Low Voltage programming enabled. MCLR/Vpp pin function is MCLR.)

// CONFIG5
#pragma config CP = OFF         // UserNVM Program memory code protection bit (Program Memory code protection disabled)
#pragma config CPD = OFF        // DataNVM code protection bit (Data EEPROM code protection disabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <pic.h> 
#include <xc.h>
#include "User_define.h"
#include "uart.h"
#include "recovery.h"
//#include "Flash.h"
//#include "HEFlash.h"

// global err
#define GERR_UART_MAGIC	0x01
#define GERR_UART_HSIZE	0x02
int g_err = 0;

//slave IIC variable
unsigned char index_i2c = 0;     // index pointer
unsigned char junk = 0;          // used to place unnecessary data
unsigned char first = 1;               // used to determine whether data address 
                                       // location or actual data

//to be added

unsigned long Sencond_Count = 0;

#define UART_CMD_MAGIC 0x4D42
// uart ret
#define ERR_UART_OK		0x00
#define ERR_UART_CMD	0x01

//BTMAIN 424D 0C00 0102 66 00 0000 0000Z

//reset
//BTMAIN 424D 0C00 0102 66 00 0000 0000Z

//start watch dog 30s
//BTMAIN 424D 0C00 0102 10 00 1E00 0000Z

//start watch dog 10s
//BTMAIN 424D 0C00 0102 10 00 0A00 0000Z

//stop watch dog
//BTMAIN 424D 0C00 0102 10 00 0000 0000Z

typedef struct 
uart_cmd_t
{
	unsigned short	magic;	//"BM"	// 0x42 0x4D //0x4D42
	unsigned short	hlen;	// head len 0x0C
	unsigned short	mark;	//	ret cmd same as request cmd
	unsigned char	cmd;	//	
	unsigned char	ret;	//	cmd respond 0 success
	unsigned short	value;	// 
	unsigned short	dlen;	//	data len
	//unsigned char	data[0];	//
}uart_cmd;
int needfanspeed = 0;
int needpowerup = 0;
int needbite = 0;

unsigned long last_feed_time	= 0;
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
	__delay_ms(1000);
	Power_Up();
	__delay_ms(200);
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
	WDTCON1bits.WDTCS	= 0b001;//INTOSC/16 (31.25 kHz)
	// Watchdog Timer Prescale Select bits
	WDTCON0bits.WDTPS	= 0b01111;//Interval 32s nominal

	WDTCON0bits.SEN		= 1;//active watch dog

}
void mcu_watch_dog_feed()
{
	asm("CLRWDT");
}

/**************************** MAIN ROUTINE ************************************/
void main(void)
{
	int ret;
	unsigned char uartrecv_buf[16];
	
    Initial_sys();
    Initial_FVR();
    Initial_TIMER();
    Power_Up();
    __delay_ms(1000);//wait 1 second for stable

	uart_init();
	mcu_watch_dog_start();
	
//    if(B_TEMP_ALR_N & PG_DDR4_1V2 & TWARN_VDD_C & PG_VDD_C)//system Power and Temperature OK
    if(PG_DDR4_1V2 & TWARN_VDD_C & PG_VDD_C)//system Power and Temperature OK, temporary disable B_TEMP_ALR_N
    {
        LED0 = 1;
    }
    else
    {
        MCU_ERR_INT = 0;
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
    IOCAP = 0b01001010;//Enable PORTA positive edge interrupt RA0 temporary disable
    IOCAN = 0b01001010;//Enable PORTA negetive edge interrupt
/* 
    //when use pickit, debug only
    IOCAP = 0b00100;//Enable PORTA positive edge interrupt
    IOCAN = 0b00100;//Enable PORTA negetive edge interrupt    
    IOCBP = 0b10100000;//Enable PORTB positive edge interrupt
    IOCBN = 0b10100000;//Enable PORTB positive edge interrupt
 */
    IOCIE = 1;//after all power on, enable IO interrupt
	//unsigned char uart_send_buf[] = "123";
    while(1)                    // main while() loop
    {                           // program will wait here for ISR to be called

		//uart_send_bytes(uart_send_buf,sizeof(uart_send_buf));
		//delayms(40);
		mcu_watch_dog_feed();
		// handle I2C cmd
        switch(I2C_Array[INDEX_INSTRUCTION])
        {
//            case(0x2C):
//                Initial_TMR1_FAN();
//                T1GGO_nDONE = 1;
 //               break;
 			case(CMD_DEBUG_MODE):
				I2C_Array[INDEX_INSTRUCTION] = 0;
				//enable watch dog debug mode
				I2C_Array[INDEX_MCU_STATUS] |= 0x01;
				break;
			case(CMD_DOG_FEED):
				I2C_Array[INDEX_INSTRUCTION] = 0;
				watch_dog_feed();
				break;
            case(CMD_REBOOT)://reboot
	            I2C_Array[INDEX_INSTRUCTION] = 0;
				watch_dog_stop();
				I2C_Array[INDEX_POWERDOWN_REASON] = POWERDOWN_REASON_REBOOT;
				doreboot();
				I2C_Array[INDEX_RESET_COUNT]++;
                break;
            case(CMD_RESET)://BM1682 reset
	            I2C_Array[INDEX_INSTRUCTION] = 0;
	            watch_dog_stop();
				I2C_Array[INDEX_POWERDOWN_REASON] = POWERDOWN_REASON_RESET;
				doreset();
                I2C_Array[INDEX_RESET_COUNT]++;
                break;                
            case(CMD_POWERDOWN)://System power down
	            I2C_Array[INDEX_INSTRUCTION] = 0;
	            watch_dog_stop();
				I2C_Array[INDEX_POWERDOWN_REASON] = POWERDOWN_REASON_POWERDOWN;
				dopowerdown();
                break;
/*
            case(0xCC)://IIC master test
                for(i = 0; i<0x24; i++)
				I2C_Array[0x0D + i] = IIC_read_byte(0x98, i);
                break;
            case(0xDD)://IIC master write test
//                IIC_write_byte(0x98, 0x0F, 0x11);    
                break;           
//*/
            case(CMD_CLRERR)://Clear abnormal status
	            I2C_Array[INDEX_INSTRUCTION] = 0;
                MCU_ERR_INT = 1;
                LED0 = 1;
                break;
            case(CMD_RECOVERY)://recovery mode
	            I2C_Array[INDEX_INSTRUCTION] = 0;
                watch_dog_stop();
                I2C_Array[INDEX_POWERDOWN_REASON] = POWERDOWN_REASON_RECOVERY;
                doreboot();
                I2C_Array[INDEX_RESET_COUNT]++;
                uart_send_recovery();
                break;
        }
		
        //send recovery string 5s
        while (uart_send == 1)
        {
            uart_send_bytes(&recovery,sizeof(recovery));
            uart_issend();
        }

		// handle UART cmd
		uart_cmd *pcmd;
		ret = uart_recv_bmcmd(&uartrecv_buf, sizeof(uartrecv_buf));
		if (ret == sizeof(uart_cmd))
		{
			pcmd = &uartrecv_buf;
			if (pcmd->magic != 0x4D42)
			{
				g_err = GERR_UART_MAGIC;
				goto uart_handle_fin;
			}
			if (pcmd->hlen != sizeof(uart_cmd))
			{
				g_err = GERR_UART_HSIZE;
				goto uart_handle_fin;
			}
			
			switch(pcmd->cmd)
			{
				case(CMD_DOG_ONOFF):
					if (pcmd->value)
					{
						watch_dog_start(pcmd->value);
					}
					else
					{
						watch_dog_stop();
					}
					break;
				case(CMD_DOG_FEED):
					watch_dog_feed();
					ret  = 0;
					break;
				case(CMD_REBOOT)://reboot
					watch_dog_stop();
					I2C_Array[INDEX_POWERDOWN_REASON] = POWERDOWN_REASON_REBOOT;
					doreboot();
					I2C_Array[INDEX_RESET_COUNT]++;
					ret  = 0;
					break;
				case(CMD_RESET)://BM1682 reset
					watch_dog_stop();
					I2C_Array[INDEX_POWERDOWN_REASON] = POWERDOWN_REASON_RESET;
					doreset();
					I2C_Array[INDEX_RESET_COUNT]++;
					ret  = 0;
					break;
				default:
					ret  = ERR_UART_CMD;
			}
			pcmd->ret = ret;
			//uart_send_bmcmd(&uartrecv_buf, sizeof(uart_cmd));
			//uart_send_bytes("\n",1);
		}
uart_handle_fin:
		
		
		if (status == STATUS_POWERUP)
		{
			if(I2C_Array[INDEX_TMP_1682] >= 75 || I2C_Array[INDEX_TMP_461] >= 70)//over temperature warning
			{
				MCU_ERR_INT = 0;
	            I2C_Array[INDEX_SYS_TMP_ST] = I2C_Array[INDEX_SYS_TMP_ST] | 0x04;
			}
			if(I2C_Array[INDEX_TMP_1682] >= 85 || I2C_Array[INDEX_TMP_461] >= 75)//over temperature shutdown
			{
				MCU_ERR_INT = 0;
	            if(SYS_RST == 1)
	            {   
					I2C_Array[INDEX_POWERDOWN_REASON] = POWERDOWN_REASON_TMP;
					dopowerdown();
	                I2C_Array[INDEX_SYS_TMP_ST] = I2C_Array[INDEX_SYS_TMP_ST] | 0x08;

	            }			
			}
			
			if (needfanspeed)
			{
				I2C_Array[INDEX_TMP_1682] = IIC_read_byte(0x98, 0x1);
		        I2C_Array[INDEX_TMP_461] = IIC_read_byte(0x98, 0x0);
				needfanspeed =0;
			}
		}
		else
		{
			I2C_Array[INDEX_TMP_1682] = 0;
			I2C_Array[INDEX_TMP_461] = 0;
		}

		if (needpowerup)
		{
			__delay_ms(50);
			if(CM2CON0bits.C2OUT == 1)
			{
				//I2C_Array[0] = 0xB0;
				MCU_ERR_INT = 0;
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
   

//    SSP2IF = 0;     
//IIC master config ended
    if(TMR0IF)
    {
        if(MCU_ERR_INT == 0)
            LED0 = ~LED0;        
        /*commit by zdh Temporary*/
        //*
        Sencond_Count++;
        I2C_Array[INDEX_TIME_L] = Sencond_Count;
        I2C_Array[INDEX_TIME_H] = Sencond_Count >> 8;
        //I2C_Array[0x02] = IIC_read_byte(0x98, 0x1);
        //I2C_Array[0x03] = IIC_read_byte(0x98, 0x0);
        needfanspeed = 1;
        //*/
        TMR0IF = 0;
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
        if (CM2CON0bits.C2OUT == 1)//power down
        {
            I2C_Array[INDEX_POWERDOWN_REASON] = POWERDOWN_REASON_POWER;
//            r1 = HEFLASH_writeBlock( 0,I2C_Array+9 , 2);
			
			Power_Down();
        }

        else if (CM2CON0bits.C2OUT == 0 && I2C_Array[INDEX_POWERDOWN_REASON] == POWERDOWN_REASON_POWER)//voltage too low to normal, reboot
        {
			needpowerup = 1;
        }

        C2IF = 0;//Clear interrupt bit
    }

/* Temporary disable  
    if(IOCAF0)//BM1682 Temperature too high
    {
        delayms(5);
        if(B_TEMP_ALR_N == 0)
        {
            MCU_ERR_INT = 0;
            I2C_Array[INDEX_SYS_TMP_ST] = I2C_Array[INDEX_SYS_TMP_ST] | 0x01;
        }
        else if(B_TEMP_ALR_N == 1)
        {
             I2C_Array[INDEX_SYS_TMP_ST] = I2C_Array[INDEX_SYS_TMP_ST] & 0xFE;//clear bit
        }
        IOCAF0 = 0;
    }
*/  

    if(IOCAF1)//ISL68127 voltage abnormal //0.9V
    {
        __delay_ms(5);
		if (status == STATUS_POWERUP)
		{
	        if(PG_VDD_C == 0)
	        {
	            MCU_ERR_INT = 0;
	            I2C_Array[INDEX_SYS_VOL_ST] = I2C_Array[INDEX_SYS_VOL_ST] | 0x01;
	        }
	        else if(PG_VDD_C == 1)
	        {
	            I2C_Array[INDEX_SYS_VOL_ST] = I2C_Array[INDEX_SYS_VOL_ST] & 0xFE;
	        }
		}
        IOCAF1 = 0;
    }
    
    if(IOCAF6)//ISL68127 temperature abnormal
    {
        __delay_ms(5);
        if(TWARN_VDD_C == 0)
        {
            MCU_ERR_INT = 0;
            I2C_Array[INDEX_SYS_TMP_ST] = I2C_Array[INDEX_SYS_TMP_ST] | 0x02;
        }
        else if(TWARN_VDD_C == 1)
        {
            I2C_Array[INDEX_SYS_TMP_ST] = I2C_Array[INDEX_SYS_TMP_ST]  & 0xFD;
        }
        IOCAF6 = 0;
    }
	
    if(IOCAF3)//DDR4 1.2V abnormal
    {
        __delay_ms(5);
		if (status == STATUS_POWERUP)
		{
	        if(PG_DDR4_1V2 == 0)
	        {
	            MCU_ERR_INT = 0;
	            I2C_Array[INDEX_SYS_VOL_ST] = I2C_Array[INDEX_SYS_VOL_ST] | 0x02;
	        }
	        else if(PG_DDR4_1V2 == 1)
	        {
	            I2C_Array[4INDEX_SYS_VOL_ST] = I2C_Array[INDEX_SYS_VOL_ST] & 0xFD;
	        }
    	}
        IOCAF3 = 0;
    }
	if (uart_isr())
	{
		//uart recv some data 
		if (watch_dog_isrun())
		{
			watch_dog_feed();
		}
	}

}// end of ISR 
