#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H

#include <xc.h>
#include "mcc_generated_files/mcc.h"

#define _power_on()         POWER_SetHigh()
#define	_power_off()        POWER_SetLow()

#define heater_on()         HEATER_SetLow()
#define	heater_off()        HEATER_SetHigh()

#define thermal_led_on()    THERMAL_LED_SetLow()
#define thermal_led_off()   THERMAL_LED_SetHigh()

#define power_led_on()      POWER_LED_SetHigh()
#define power_led_off()     POWER_LED_SetLow()

#define is_under_temp()     (!UNDER_TEMP_GetValue())
#define is_power_key_down() (POWER_BUTTON_GetValue())
#define is_all_reset()      (!ALL_RESET_GetValue())

#endif	/* XC_HEADER_TEMPLATE_H */

