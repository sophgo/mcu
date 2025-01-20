#ifndef PWM_H
#define PWM_H

void pwm_set_duty_cycle(uint32_t duty_cycle);
void init_pwm(void);
void tim22_pwm_init(void);
void tim2_pwm_init(void);
void pwm_set_t2_duty_cycle(uint32_t duty_cycle);
#endif
