#include "base.h"
// input
// OVER_TEMP	low
// UNDER_TEMP	low
// POWER_KEY	high
// PMEB			
#define		GPIO_OVER_TEMP_IN	PORTAbits.RA5
#define		GPIO_UNDER_TEMP_IN	PORTCbits.RC3
#define		GPIO_POWER_KEY_IN	PORTCbits.RC0

// output
// POWER_LED	light	high
// TRM_LED		light	low
// THERMAL_OFF	active	low
// POWER_ON		on		high
#define		GPIO_PWRLED_EN		PORTCbits.RC5
#define		GPIO_TRMLED_nEN		PORTAbits.RA4
#define		GPIO_THERMAL_nEN	PORTCbits.RC1
#define		GPIO_PWRON_EN		PORTCbits.RC2

char isover_temp()
{
	return !GPIO_OVER_TEMP_IN;
}

char isunder_temp()
{
	return !GPIO_UNDER_TEMP_IN;
}

char ispower_key_down()
{
	return GPIO_POWER_KEY_IN;
}

void power_on()
{
	GPIO_PWRON_EN = 1;//power up
}

void power_off()
{
	GPIO_PWRON_EN = 0;//power down;
}

void thermal_on()
{
	GPIO_THERMAL_nEN = 0;
}

void thermal_off()
{
	GPIO_THERMAL_nEN = 1;
}

void power_led_on()
{
	GPIO_PWRLED_EN = 1;
}

void power_led_off()
{
	GPIO_PWRLED_EN = 0;
}

void thermal_led_on()
{
	GPIO_TRMLED_nEN = 0;
}

void thermal_led_off()
{
	GPIO_TRMLED_nEN = 1;
}

