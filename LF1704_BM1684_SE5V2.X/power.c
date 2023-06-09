#include "mcc_generated_files/mcc.h"
#include "common.h"
#include "tick.h"

#define POWER_ON_DELAY      1500
#define POWER_OFF_DELAY     4000
#define THERMAL_LED_FREQ    4

static uint8_t is_power_on;

void power_on(void)
{
    if (is_under_temp())
        heater_on();

    while (is_under_temp()) {
        if (tick_get() % (1000 / THERMAL_LED_FREQ) <
            (1000 / THERMAL_LED_FREQ) / 2)
            thermal_led_on();
        else
            thermal_led_off();
    }

    thermal_led_off();

    heater_off();

    _power_on();
    power_led_on();
    is_power_on = true;
}

void power_off(void)
{
    _power_off();
    power_led_off();
    is_power_on = false;
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

void reset_ctrl(void)
{
    if (!is_power_on)
        return;

    if (is_all_reset()) {
        __delay_ms(1);
        power_off();
        __delay_ms(1000);
        power_on();
        __delay_ms(1);
    }
}
