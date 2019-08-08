/*
 * File:   main.c
 * Author: taiqiang.cao
 *
 * Created on May 28, 2019, 11:10 AM
 */
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

#include <pic.h> 
#include <xc.h>
#include "User_define.h"
#include "Led.h"

// array for master to write to, reserved
volatile unsigned char I2C_Array_Rx[RX_ELMNTS] = 0;
//slave IIC variable
unsigned char index_i2c = 0;     // index pointer
unsigned char junk = 0;          // used to place unnecessary data
unsigned char first = 1;               // used to determine whether data address 
                                       // location or actual data
extern void led_bd_pwr(void);
extern void led_bd_uid(void);

#define MCU_VERSION         0x02

void main(void) {  
    int i = 0;
    
    Initial_sys();
    Initial_TIMER();
    I2C_Array[INDEX_VERSION] = MCU_VERSION;
//    Initial_FVR();
    
    while(1){

        for (i = 0; i < 5; i++) {
                led_func_table[i].set();
        }

        led_bd_pwr();
        led_bd_uid();
        
        set_frequence();
    }

    return;
    
}

void interrupt ISR(void) 
{
    if (SSP2IF)                              // check to see if SSP interrupt I2C
    {
    	// fix IIC stuck bug
    	if (SSP2CON1bits.SSPOV)
    	{
    		SSP2CON1bits.SSPOV = 0;
    	}
        if (SSP2STATbits.R_nW)               // Master read (R_nW = 1)
        {
            if (!SSP2STATbits.D_nA)        // Last byte was an address (D_nA = 0)
            {
                SSP2BUF = I2C_Array[index_i2c++]; // load with value from array
                SSP2CON1bits.CKP = 1;             // Release CLK
            }
            else//if (SSP1STATbits.D_nA)               // Last byte was data (D_nA = 1)
            {
                SSP2BUF = I2C_Array[index_i2c++]; // load with value from array
                SSP2CON1bits.CKP = 1;             // Release CLK
            }

        }
        else //if (!SSP1STATbits.R_nW) //  Master write (R_nW = 0)
        {
            if (!SSP2STATbits.D_nA) // Last byte was an address (D_nA = 0)
            {
                first = 1; //last byte was address, next will be data location
                junk = SSP2BUF;                  // read buffer to clear BF
                SSP2CON1bits.CKP = 1;            // Release CLK
            }
            else//if (SSP1STATbits.D_nA)               // Last byte was data (D_nA = 1)
            {
                if (first) 
                {
                    index_i2c = SSP2BUF;      // load index with array location
                    first = 0;               // now clear this since we have 
                }                            //location to read from/write to
                else
                {
                    if (index_i2c < RX_ELMNTS)       // make sure index is not
                    {                                //out of range of array
                        I2C_Array[index_i2c++] = SSP2BUF; //load array with data
                    } 
                    else
                    {
                        junk = SSP2BUF; //array location not valid, discard data
                    }
                }
                if (SSP2CON1bits.WCOL)           // Did a write collision occur?
                {
                    SSP2CON1bits.WCOL = 0;       //  clear WCOL
                    junk = SSP2BUF;              // dummy read to clear BF bit
                }
                SSP2CON1bits.CKP = 1;            // Release CLK
            }
        }
         SSP2IF = 0;                              // clear SSP1IF flag bit
    }
    
    if (TMR0IF)
    {
        TMR0IF = 0;
        if(I2C_Array[6] == 0)
        {
            LED_UID_P = ~LED_UID_P;
        }

#if 1       
        if (I2C_Array[0] == 4)
        {
            HDD_GOOD = ~HDD_GOOD;
        } else if (I2C_Array[0] == 5) {
            HDD_FAULT = ~HDD_FAULT;
        }
        
        if (I2C_Array[1] == 4)
        {
            ETH_GOOD = ~ETH_GOOD;
        } else if (I2C_Array[1] == 5) {
            ETH_FAULT = ~ETH_FAULT;
        }

        if (I2C_Array[2] == 4)
        {
            SYS_GOOD = ~SYS_GOOD;
        } else if (I2C_Array[2] == 5) {
            SYS_FAULT = ~SYS_FAULT;
        }
        
        if (I2C_Array[3] == 4)
        {
            FAN_GOOD = ~FAN_GOOD;
        } else if (I2C_Array[3] == 5) {
            FAN_FAULT = ~FAN_FAULT;
        }
        
        if (I2C_Array[4] == 4)
        {
            PWR_FAULT = ~PWR_FAULT;
        } else if (I2C_Array[4] == 5) {
            PWR_GOOD = ~PWR_GOOD;
        } 
#endif
    }
   
}// end of ISR

