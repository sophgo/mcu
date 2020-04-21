#include "mcc_generated_files/mcc.h"
#include "common.h"

volatile uint32_t tick;

static void tick_inc(void)
{
    ++tick;
}

void tick_init(void)
{
    TMR0_SetInterruptHandler(tick_inc);
}

uint32_t tick_get(void)
{
    uint32_t t;
    di();
    t = tick;
    ei();
    return t;
}

