/*
 * pwm.c: PWM 调压输出控制 (TIMER3 CH3, PB9)
 *
 * 使用 TIMER3 通道3 输出 PWM 信号控制 VDDC 电压
 * 默认频率 50kHz, 占空比 0~1000 (0%~100%)
 * APB1 = 50MHz, TIMER3 时钟 = 50MHz
 *
 * pwm <freq> <duty>  频率(Hz) + 占空比(0~1000)
 *
 * (C) Copyright 2024 Sophgo Technology
 */
#include <gd32e50x_timer.h>
#include <gd32e50x_gpio.h>
#include <gd32e50x_rcu.h>
#include <pin.h>
#include <pwm/pwm.h>

static unsigned int pwm_period   = 999;	/* 50kHz default */
static unsigned int pwm_freq_hz  = 50000;
static unsigned int pwm_cur_duty;

void pwm_init(void)
{
	timer_oc_parameter_struct oc_param;
	timer_parameter_struct timer_param;

	rcu_periph_clock_enable(RCU_TIMER3);
	rcu_periph_clock_enable(RCU_GPIOB);

	/* PB9 = TIMER3_CH3, AF push-pull */
	gpio_init(PWM_VDDC_PORT, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, PWM_VDDC_PIN);

	/* TIMER3 基础配置, 默认 50kHz */
	timer_struct_para_init(&timer_param);
	timer_param.prescaler         = 0;
	timer_param.alignedmode       = TIMER_COUNTER_EDGE;
	timer_param.counterdirection  = TIMER_COUNTER_UP;
	timer_param.period            = pwm_period;
	timer_param.clockdivision     = TIMER_CKDIV_DIV1;
	timer_init(TIMER3, &timer_param);

	/* CH3 PWM 模式1 */
	timer_channel_output_struct_para_init(&oc_param);
	oc_param.ocpolarity   = TIMER_OC_POLARITY_HIGH;
	oc_param.outputstate  = TIMER_CCX_ENABLE;
	timer_channel_output_config(TIMER3, TIMER_CH_3, &oc_param);

	timer_channel_output_pulse_value_config(TIMER3, TIMER_CH_3, 0);
	timer_channel_output_mode_config(TIMER3, TIMER_CH_3, TIMER_OC_MODE_PWM1);
	timer_channel_output_shadow_config(TIMER3, TIMER_CH_3, TIMER_OC_SHADOW_DISABLE);

	timer_auto_reload_shadow_enable(TIMER3);
	timer_enable(TIMER3);

	pwm_cur_duty = 0;
}

void pwm_set(unsigned int freq_hz, unsigned int duty_permil)
{
	unsigned int period;

	/* 频率范围 1Hz ~ 50kHz */
	if (freq_hz == 0)
		freq_hz = 1;
	if (freq_hz > 50000)
		freq_hz = 50000;

	/* period = 50MHz / freq_hz - 1 */
	period = 50000000UL / freq_hz;
	if (period < 1)
		period = 1;
	if (period > 65535)
		period = 65535;

	pwm_freq_hz = freq_hz;
	pwm_period  = period;

	/* 先停 timer，重新配置 */
	timer_disable(TIMER3);
	TIMER_CAR(TIMER3) = pwm_period;
	timer_enable(TIMER3);

	/* 占空比 */
	if (duty_permil > 1000)
		duty_permil = 1000;
	pwm_cur_duty = (pwm_period * duty_permil) / 1000;
	timer_channel_output_pulse_value_config(TIMER3, TIMER_CH_3, pwm_cur_duty);

	pwm_cur_duty = duty_permil;
}

void pwm_set_duty(unsigned int duty)
{
	if (duty > 1000)
		duty = 1000;
	pwm_cur_duty = duty;
	timer_channel_output_pulse_value_config(TIMER3, TIMER_CH_3,
		(pwm_period * duty) / 1000);
}

unsigned int pwm_get_duty(void)
{
	return pwm_cur_duty;
}

unsigned int pwm_get_freq(void)
{
	return pwm_freq_hz;
}

void pwm_enable(void)
{
	timer_primary_output_config(TIMER3, ENABLE);
}

void pwm_disable(void)
{
	timer_primary_output_config(TIMER3, DISABLE);
}