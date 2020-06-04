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

inline int is_power_key_down(void)
{
    uint32_t down, up;

    if (!_is_power_key_down())
        return 0;

    down = 0;
    up = 0;

    /* timer1 is 1ms fixed time timeout */
    /* it is configured by MCC */
    TMR1_StartTimer();
    while (!TMR1_HasOverflowOccured()) {
        if (_is_power_key_down())
            ++down;
        else
            ++up;
    }
    TMR1_StopTimer();

    return down > up ? 1 : 0;
}

inline int is_reset_key_down(void)
{
    uint32_t down, up;

    if (!_is_reset_key_down())
        return 0;

    down = 0;
    up = 0;

    /* timer1 is 1ms fixed time timeout */
    /* it is configured by MCC */
    TMR1_StartTimer();
    while (!TMR1_HasOverflowOccured()) {
        if (_is_reset_key_down())
            ++down;
        else
            ++up;
    }
    TMR1_StopTimer();

    return down > up ? 1 : 0;
}
