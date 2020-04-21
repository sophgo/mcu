#include "mcc_generated_files/mcc.h"
#include "common.h"

struct ntc_entry {
    int temp;
    uint16_t adc;
};

#define ADC_TEMP_TABLE_SIZE     (20)

const struct ntc_entry adc_temp_table[ADC_TEMP_TABLE_SIZE] = {
    {-20, 633},
    {-15, 518},
    {-10, 423},
    {-5, 346},
    {0, 284},
    {5, 233},
    {10, 192},
    {15, 158},
    {20, 131},
    {25, 109},
    {30, 91},
    {35, 76},
    {40, 64},
    {45, 54},
    {50, 46},
    {55, 39},
    {60, 33},
    {65, 28},
    {70, 24},
    {75, 21},
};

static inline int temp_get()
{
    uint16_t adc;
    int i, delta;
    struct ntc_entry *p;

    adc = ADC_GetConversion(0);
    for (i = 0; i < ADC_TEMP_TABLE_SIZE; ++i) {
        if (adc <= adc_temp_table[i].adc)
            break;
    }

    /* temperature under flow */
    if (i == ADC_TEMP_TABLE_SIZE)
        return adc_temp_table[0].temp;
    /* temperature over flow */
    if (i == ADC_TEMP_TABLE_SIZE - 1)
        return adc_temp_table[i].temp;

    /* more accuracy */
    delta = adc_temp_table[i].adc - adc_temp_table[i + 1].adc;
    
    return (adc_temp_table[i].adc - adc) * 5 / delta + adc_temp_table[i].temp;
}

void temp_ctrl(void)
{
#if 0
    if (temp_get() < 5)
        heater_on();
    else
        heater_off();
#else
    volatile int temp;
    temp = temp_get();
#endif
}


