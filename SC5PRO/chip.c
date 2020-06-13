#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>
#include <pin.h>
#include <tick.h>
#include <timer.h>
#include <debug.h>

static volatile int is_chip_ready;

void chip_init(void)
{
}

void chip_listen(void)
{
}

void chip_destroy(void)
{
}

void chip_update(void)
{
}

void exti4_15_isr(void)
{
}

