/**
 * @file   main.c
 * @brief  Basic PWM(Pulse-width modulation) output example.
 */
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>
#include <debug.h>
#include <stdint.h>
#include <tick.h>
#include <pwm.h>

#define VDDC_PWM_FREQUENCY (1000000) /*  Hz PWM frequency */
#define TPU_PWM_FREQUENCY (2000000) /*  Hz PWM frequency */
#define rcc_apb1_frequency (32 * 1000 * 1000) /* 32 MHz */

// vddc
void tim22_pwm_init(void)
{ 
    // Reset TIM22 peripheral
    rcc_periph_clock_enable(RCC_TIM22);
    rcc_periph_reset_pulse(RST_TIM22);

    // Set TIM22 prescaler to have a 1MHz timer frequency
    timer_set_prescaler(TIM22, ((rcc_apb1_frequency / VDDC_PWM_FREQUENCY) - 1));
    // Set TIM22 period to 1000 ticks (for 1MHz PWM frequency)
    timer_set_period(TIM22, 1000 - 1);
    // Set the duty cycle to 35% (350 ticks)
    timer_set_oc_value(TIM22, TIM_OC2, 350);
    // Configure TIM22 channel 2 in PWM mode 1
    timer_set_oc_mode(TIM22, TIM_OC2, TIM_OCM_PWM1);
    timer_enable_oc_output(TIM22, TIM_OC2);
    // Enable TIM22 counter
    timer_enable_counter(TIM22); 
}

void pwm_set_duty_cycle(uint32_t duty_cycle)
{
    timer_set_oc_value(TIM22, TIM_OC2, duty_cycle);
}

// tpu
void tim2_pwm_init(void)
{
    // Reset TIM2 peripheral
    rcc_periph_clock_enable(RCC_TIM2);
    rcc_periph_reset_pulse(RST_TIM2);

    // Set TIM2 prescaler to have a 1MHz timer frequency
    timer_set_prescaler(TIM2, ((rcc_apb1_frequency / TPU_PWM_FREQUENCY) - 1));
    // Set TIM2 period to 1000 ticks (for 1MHz PWM frequency)
    timer_set_period(TIM2, 1000 - 1);
    // Set the duty cycle to 40% (400 ticks)
    timer_set_oc_value(TIM2, TIM_OC1, 400);
    // Configure TIM2 channel 1 in PWM mode 1
    timer_set_oc_mode(TIM2, TIM_OC1, TIM_OCM_PWM1);
    timer_enable_oc_output(TIM2, TIM_OC1);
    // Enable TIM2 counter
    timer_enable_counter(TIM2);
}

void pwm_set_t2_duty_cycle(uint32_t duty_cycle)
{
    timer_set_oc_value(TIM2, TIM_OC1, duty_cycle);
}

void init_pwm(void)
{
    tim22_pwm_init();
    // vddc
    pwm_set_duty_cycle(380);
    tim2_pwm_init();
    // tpu
    pwm_set_t2_duty_cycle(700);
}
