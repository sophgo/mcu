// PIC16LF1704 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1
#pragma config FOSC = INTOSC    // Oscillator Selection Bits (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = NSLEEP    // Watchdog Timer Enable (WDT enabled while running and disabled in Sleep)
#pragma config PWRTE = ON       // Power-up Timer Enable (PWRT enabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable (Brown-out Reset disabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = ON        // Internal/External Switchover Mode (Internal/External Switchover Mode is enabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is enabled)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config PPS1WAY = ON     // Peripheral Pin Select one-way control (The PPSLOCK bit cannot be cleared once it is set by software)
#pragma config ZCDDIS = ON      // Zero-cross detect disable (Zero-cross detect circuit is disabled at POR)
#pragma config PLLEN = ON       // Phase Lock Loop enable (4x PLL is always enabled)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LPBOR = OFF      // Low-Power Brown Out Reset (Low-Power BOR is disabled)
#pragma config LVP = OFF        // Low-Voltage Programming Enable (High-voltage on MCLR/VPP must be used for programming)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.



#include <string.h>
#include <pic.h> 
#include "main.h"
#include "TIMER.h"
#include "base.h"


// work with __delay_ms __delay_us
#define _XTAL_FREQ 16000000

/**************************** MAIN ROUTINE ************************************/



#define POWER_STATUS_OFF		0
#define POWER_STATUS_THERMAL 	1
#define POWER_STATUS_ON			2

unsigned char g_power_status = POWER_STATUS_OFF;

unsigned short g_watchdog_count = 0;

char need_poweron = 1;
char need_poweroff = 0;
unsigned char keydelay = 0;
void main(void)
{
	char ret;

    Initial_sys();

	unsigned char len;

	TIMER0_set(TIMER0_ISR_FREQ_1k);
    
    power_off();
	thermal_off();
	power_led_off();
	thermal_led_off();
    power_led_on();
	while(1)
	{
		asm("CLRWDT");	// clear watch dog

		if (g_power_status == POWER_STATUS_OFF)
		{
			if (need_poweron)
			{
				if (isunder_temp())
				{
					thermal_on();
					g_power_status = POWER_STATUS_THERMAL;
					keydelay = 0;
				}
				else
				{
					power_on();
					g_power_status = POWER_STATUS_ON;
					keydelay = 0;
				}
				need_poweron = 0;
			}
		}
		else if (g_power_status == POWER_STATUS_THERMAL)
		{
			if (need_poweroff)
			{
                thermal_off();
				g_power_status = POWER_STATUS_OFF;
				need_poweroff = 0;
				keydelay = 0;
			}
			if (!isunder_temp())
			{
				thermal_off();
                power_on();
				g_power_status = POWER_STATUS_ON;
			}
		}
		else//POWER_STATUS_ON
		{
			if (need_poweroff)
			{
				power_off();
				g_power_status = POWER_STATUS_OFF;
				need_poweroff = 0;
				keydelay = 0;
			}
		}

	}
}

#define TIMER0_1S	1000
unsigned short TIMER0_1S_count = TIMER0_1S;
unsigned short time = 0;
#define POWER_ON_DELAY  1
#define POWER_OFF_DELAY 3

void interrupt ISR(void) 
{
	if (TIMER0_isr())//timer 0 interrupt
	{
		if (TIMER0_1S_count-- == 0)
		{
            TIMER0_1S_count = TIMER0_1S;
            time++;
			if (g_power_status == POWER_STATUS_THERMAL)
			{
                if(time %2 == 1)
                {
                    thermal_led_on();
                }
                else
                {
                    thermal_led_off();
                }
			}
			else if (g_power_status == POWER_STATUS_ON)
			{
				thermal_led_on();
			}
            else
            {
                thermal_led_off();
            }

			if (ispower_key_down())
			{
				keydelay++;
			}
			else
			{
				keydelay = 0;
			}
			if (g_power_status != POWER_STATUS_OFF)
			{
				if (keydelay > POWER_OFF_DELAY)
				{
					need_poweroff = 1;
				}
			}
			else
			{
				if (keydelay > POWER_ON_DELAY)
				{
					need_poweron = 1;
				}
			}
		}
	}

}// end of ISR 
