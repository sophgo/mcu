/*******************************************************************************
PIC16F1508 BM1682 SINGLE CHIP
MAIN CODE 
Author: Patrick Chen(CPF) 
BITMAIN 
DATE: 21/03/2018
/****************************************************************/
#include <pic.h> 
#include <xc.h>
#include"User_define.h"
#include "Flash.h"
#include "HEFlash.h"


//__PROG_CONFIG (1,0x3FDC);
//__PROG_CONFIG (2,0x3FFF);

// CONFIG1
#pragma config FOSC = INTOSC    // INTOSC oscillator: I/O function on CLKIN pin
#pragma config WDTE = OFF        // Watchdog Timer Enable (WDT enabled)
#pragma config PWRTE = OFF       // Power-up Timer Enable (PWRT enabled)//CPF disable
#pragma config MCLRE = ON       // MCLR/VPP pin function is MCLR disable
#pragma config CP = OFF          // Flash Program Memory Code Protection enabled
#pragma config BOREN = ON       // Brown-out Reset enabled
#pragma config CLKOUTEN = OFF   // CLKOUT function is disabled.//ON for debug only
                                // I/O or oscillator function on the CLKOUT pin
#pragma config IESO = OFF        // Internal/External Switch-over mode is enabled
#pragma config FCMEN = OFF       // Fail-Safe Clock Monitor is enabled

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection off
#pragma config STVREN = ON      // Stack Overflow/Underflow will cause a Reset
#pragma config BORV = LO        // Brown-out Reset Voltage (Vbor),
                                // low trip point selected.
#pragma config LVP = ON         // Low-voltage programming enabled


// array for master to write to, reserved
volatile unsigned char I2C_Array_Rx[RX_ELMNTS] = 0;
//unsigned char I2C_Array_Rx[RX_ELMNTS] = 0;	

unsigned char index_i2c = 0;     // index pointer
unsigned char junk = 0;          // used to place unnecessary data
unsigned char clear = 0x00;      // used to clear array location once
                                 // it has been read
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
//        asm("CLRWDT");          // clear WDT
        switch(I2C_Array[INDEX_INSTRUCTION])
        {
//            case(0x2C):
//                Initial_TMR1_FAN();
//                T1GGO_nDONE = 1;
 //               break;
            case(0x12)://reboot
                GIE = 0;
                IOCIE = 0;
                Power_Down();
                delayms(1000);
                Power_Up();
                delayms(200);
                GIE = 1;
                IOCIE = 1;
                I2C_Array[INDEX_INSTRUCTION] = 0;
                break;
            case(0x66)://BM1682 reset
                GIE = 0;
                IOCIE = 0;
                SYS_RST = 0;
                delayms(200);
                SYS_RST = 1;
                I2C_Array[INDEX_RESET_COUNT]++;
                GIE = 1;
                IOCIE = 1;
                I2C_Array[INDEX_INSTRUCTION] = 0;
                break;                
            case(0xF7)://System power down
                GIE = 0;
                IOCIE = 0;
                Power_Down();
                GIE = 1;
                IOCIE = 1;
                I2C_Array[INDEX_INSTRUCTION] = 0;
                break;
            case(0x86)://Clear abnormal status
                MCU_ERR_INT = 0;
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
        
    }
}

void interrupt ISR(void) 
{
    if (SSP1IF)                              // check to see if SSP interrupt I2C
    {
        if (SSPSTATbits.R_nW)               // Master read (R_nW = 1)
        {
            if (!SSPSTATbits.D_nA)        // Last byte was an address (D_nA = 0)
            {
                SSPBUF = I2C_Array[index_i2c++]; // load with value from array
                SSPCON1bits.CKP = 1;             // Release CLK
            }
            if (SSPSTATbits.D_nA)               // Last byte was data (D_nA = 1)
            {
                SSPBUF = I2C_Array[index_i2c++]; // load with value from array
                SSPCON1bits.CKP = 1;             // Release CLK
            }

        }
        if (!SSPSTATbits.R_nW) //  Master write (R_nW = 0)
        {
            if (!SSPSTATbits.D_nA) // Last byte was an address (D_nA = 0)
            {
                first = 1; //last byte was address, next will be data location
                junk = SSPBUF;                  // read buffer to clear BF
                SSPCON1bits.CKP = 1;            // Release CLK
            }
            if (SSPSTATbits.D_nA)               // Last byte was data (D_nA = 1)
            {
                if (first) 
                {
                    index_i2c = SSPBUF;      // load index with array location
                    first = 0;               // now clear this since we have 
                }                            //location to read from/write to
                
                else
                {
                    if (index_i2c < RX_ELMNTS)       // make sure index is not
                    {                                //out of range of array
                        I2C_Array[index_i2c++] = SSPBUF; //load array with data
                    } 
                    else
                    {
                        junk = SSPBUF; //array location not valid, discard data
                    }
                }
                if (SSPCON1bits.WCOL)           // Did a write collision occur?
                {
                    SSPCON1bits.WCOL = 0;       //  clear WCOL
                    junk = SSPBUF;              // dummy read to clear BF bit
                }
                SSPCON1bits.CKP = 1;            // Release CLK
            }
        }
    }
    if (BCL1IF)                                  // Did a bus collision occur?
    {
        junk = SSPBUF;                      // dummy read SSPBUF to clear BF bit
        BCL1IF = 0;                          // clear bus collision Int Flag bit
        SSPCON1bits.CKP = 1;                // Release CLK
    }
    SSP1IF = 0;                              // clear SSPIF flag bit
    if(NCO1IF)//NCO overflow interrupt
    {
        if(MCU_ERR_INT)
            LED0 = ~LED0;
        Sencond_Count++;
        I2C_Array[INDEX_TIME_L] = Sencond_Count;
        I2C_Array[INDEX_TIME_H] = Sencond_Count >> 8;
        NCO1IF = 0;
    }    
    if (C2IF)//12V down or up
    {
        if (C2OUT == 1)//power down
        {
            I2C_Array[15] = 0x0F;
            r1 = HEFLASH_writeBlock( 0,I2C_Array+9 , 2);
            Power_Down();
            
            C2IF = 0;
        }

        else if (C2OUT == 0 && I2C_Array[15] == 0x0F)//voltage too low to normal, reboot
            {   
                delayms(50);
                if(C2OUT == 1)
                {
                    I2C_Array[0] = 0xB0;
                    MCU_ERR_INT = 1;
                    Power_Up();
                }
                C2IF = 0;
            }

        C2IF = 0;//Clear interrupt bit
            

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

