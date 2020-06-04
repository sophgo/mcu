#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H

#include <xc.h>
#include "mcc_generated_files/mcc.h"


#define _power_on()         POWER_SetHigh()
#define	_power_off()        POWER_SetLow()

extern int8_t is_heater_on;
inline void heater_on(void);
inline void heater_off(void);

#define thermal_led_on()    THERMAL_LED_SetLow()
#define thermal_led_off()   THERMAL_LED_SetHigh()

#define power_led_on()      POWER_LED_SetHigh()
#define power_led_off()     POWER_LED_SetLow()

#define is_under_temp()         (!UNDER_TEMP_GetValue())
#define _is_power_key_down()    (POWER_KEY_GetValue())
#define _is_reset_key_down()    (!RESET_KEY_GetValue())

#define smb_alert_set()         SMBALERT_SetLow()
#define smb_alert_clr()         SMBALERT_SetHigh()

inline int is_power_key_down(void);
inline int is_reset_key_down(void);

static inline void line(unsigned int times)
{
    unsigned int i;

    for (i = 0; i < times; ++i) {
        thermal_led_on();
        __delay_ms(200);    
        thermal_led_off();
        __delay_ms(200);
    }
    __delay_ms(1000);
}

static inline void number(uint32_t number)
{
    int i;
    thermal_led_on();
    __delay_ms(1000);
    thermal_led_off();
    __delay_ms(1000);
    
    for (i = 0; i < 32; i+=4) {
        uint8_t tmp = (number >> i) & 0xf;
        line(tmp);
    }
    
    thermal_led_on();
    __delay_ms(1000);
    thermal_led_off();
    __delay_ms(1000);
}

#endif	/* XC_HEADER_TEMPLATE_H */

