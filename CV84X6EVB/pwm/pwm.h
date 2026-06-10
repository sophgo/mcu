/*
 * pwm.h: PWM 调压输出控制 (TIMER3 CH3, PB9)
 *
 * (C) Copyright 2024 Sophgo Technology
 */
#ifndef __PWM_H__
#define __PWM_H__

void pwm_init(void);
void pwm_set(unsigned int freq_hz, unsigned int duty_permil);
void pwm_set_duty(unsigned int duty);
unsigned int pwm_get_duty(void);
unsigned int pwm_get_freq(void);
void pwm_enable(void);
void pwm_disable(void);

#endif /* __PWM_H__ */