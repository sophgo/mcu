#include "mcc_generated_files/mcc.h"
#include "common.h"
#include "tick.h"
#include "i2c.h"

#define FORCE_POWER_OFF_TIMEOUT     (20 * 1000)
#define POWER_ON_DELAY              1500
#define POWER_OFF_DELAY             500
#define FORCE_POWER_OFF_DELAY       (POWER_OFF_DELAY + 4500)
#define REBOOT_DELAY                500
#define FACTORY_RESET_DELAY         (REBOOT_DELAY + 10000)
#define THERMAL_LED_FREQ            4

static uint8_t is_power_on;

void power_on(void)
{
	if (is_power_on)
        return;

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
    di();
    if (!is_power_on)
        goto end;

    _power_off();
    power_led_off();
    is_power_on = false;
end:
    ei();
}

void power_ctrl(void)
{
    uint32_t start, elapse;
    static uint8_t last_key_status = 0;
    uint8_t current_key_status;

    current_key_status = is_power_key_down();

    /* detect raising edge */
    if (!(current_key_status == 1 && last_key_status == 0))
        return;

    start = tick_get();

    while (is_power_key_down()) {
        elapse = tick_get() - start;
        if (is_power_on) {
            /* power off soc */
            if (elapse >= FORCE_POWER_OFF_DELAY) {
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

    if (elapse >= POWER_OFF_DELAY) {
        req_power_off();
        /* we have enter power off flow */
        /* let power led flick until STM32 request power off */
        /* if no response in 20 seconds from STM32, force power off */
        start = tick_get();

        while (is_power_on) {
            elapse = tick_get() - start;
            if (elapse > FORCE_POWER_OFF_TIMEOUT) {
                power_off();
                break;
            }

            di();
            /* led control */
            if (elapse & 1024)
                power_led_on();
            else 
                power_led_off();
            ei();
        }
    }
}

void reset_ctrl(void)
{
    uint32_t start, elapse = 0;
    static uint8_t last_key_status = 0;

    /* detect reset signal */
    if (!is_reset_key_down())
        return;

    start = tick_get();

    while (is_reset_key_down()) {
        elapse = tick_get() - start;
        if (elapse >= FACTORY_RESET_DELAY) {
            req_factory_reset();
            return;
        }
    }
    if (elapse >= REBOOT_DELAY)
        req_reboot();
}
