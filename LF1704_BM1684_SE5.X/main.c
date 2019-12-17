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
#define POWER_STATUS_ON			1

unsigned char g_power_status = POWER_STATUS_OFF;

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


	power_on();
	power_led_on();
	g_power_status = POWER_STATUS_ON;
	while(1)
	{
		asm("CLRWDT");	// clear watch dog
		__delay_ms(5);
	}
}

#define TIMER0_100MS	100
unsigned short TIMER0_100MS_count = TIMER0_100MS;
unsigned short time = 0;
#define POWER_ON_DELAY  15
#define POWER_OFF_DELAY 40

unsigned char keydelay = 0;
void interrupt ISR(void) 
{
	if (TIMER0_isr())//timer 0 interrupt
	{
		if (TIMER0_100MS_count-- == 0)
		{
            TIMER0_100MS_count = TIMER0_100MS;
#if 1
            time++;
			if (g_power_status == POWER_STATUS_ON)
			{
				if (isunder_temp())
				{
					thermal_on();
					if(time % 10 < 5)
	                {
	                    thermal_led_on();
	                }
	                else
	                {
	                    thermal_led_off();
	                }
				}
				else if (isover_temp())
				{
					thermal_off();
					thermal_led_on();
				}
				else
				{
					thermal_off();
					thermal_led_off();
				}

			}
            else
            {
				if (isunder_temp() || isover_temp())
				{
					thermal_off();
					thermal_led_on();
				}
				else
				{
					thermal_off();
					thermal_led_off();
				}
            }
#endif
			if (ispower_key_down())
			{
				keydelay++;
			}
			else
			{
				keydelay = 0;
			}
			if (g_power_status == POWER_STATUS_ON)
			{
				if (keydelay > POWER_OFF_DELAY)
				{
					keydelay = 0;

					power_off();
					power_led_off();
					g_power_status = POWER_STATUS_OFF;
				}
			}
			else
			{
				if (keydelay > POWER_ON_DELAY)
				{
					keydelay = 0;

					power_on();
					power_led_on();
					g_power_status = POWER_STATUS_ON;
				}
			}
		}
	}

}// end of ISR 
