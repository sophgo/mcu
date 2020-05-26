#include <xc.h>
#include "mcc_generated_files/mcc.h"
#include "common.h"

int8_t is_heater_on;

inline void heater_on(void)
{
	HEATER_SetLow();
	thermal_led_on();
	is_heater_on = true;
}
inline void heater_off(void)
{
	HEATER_SetHigh();
	thermal_led_off();
	is_heater_on = false;
}

