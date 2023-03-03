#include "mcc_generated_files/mcc.h"
#include "tick.h"

#define POWER_ON_DELAY      500
#define POWER_OFF_DELAY     4000

static uint8_t is_power_on;

void power_on(void)
{
    POWER_ON_SetHigh();
    POWER_LED_SetHigh();
    IO_RC5_SetHigh();
    is_power_on = true;
}

void power_off(void)
{
    POWER_ON_SetLow();
    POWER_LED_SetLow();
    IO_RC5_SetLow();
    is_power_on = false;
}

bool is_power_key_down(void)
{
    if (POWER_KEY_GetValue() == 0)
        return false;
    __delay_ms(10);
    if (POWER_KEY_GetValue() == 0)
        return false;
    __delay_ms(10);
    if (POWER_KEY_GetValue() == 0)
        return false;
    __delay_ms(10);
    if (POWER_KEY_GetValue() == 0)
        return false;
    return true;
}

void power_ctrl(void)
{
    uint32_t start_tick, elapse;
    static uint8_t last_key_status = 0;
    uint8_t current_key_status;

    current_key_status = is_power_key_down();

    /* detect raising edge */
    if (!(current_key_status == 1 && last_key_status == 0)) {
        last_key_status = current_key_status;
        return;
    }

    last_key_status = current_key_status;

    start_tick = tick_get();
    
    while (is_power_key_down()) {
        elapse = tick_get() - start_tick;
        if (is_power_on) {
            /* power off soc */
            if (elapse >= POWER_OFF_DELAY) {
                power_off();
                break;
            }
        } else {
            /* power on soc */
            if (elapse >= POWER_ON_DELAY) {
                power_on();
                break;
            }
        }
    }
}


