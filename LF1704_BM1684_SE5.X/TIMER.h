#ifndef __TIMER_H__
#define __TIMER_H__

#define TIMER0_ISR_FREQ_1k	1
#define TIMER0_ISR_FREQ_200	2

void TIMER0_set(int freq_sel);

char TIMER0_isr();


// PWM_period	= (PR2 + 1) * 4 * Prescaler / FOSC
// PWM_pluse_width = PWM3DCHL * Prescaler / FOSC
// PWM_period/ PWM_pluse_width =  (PR2 + 1) * 4 / PWM_MAX_3DHL
// PWM_MAX_3DHL = (PR2 + 1) * 4
int TIMER2_get_PWMMAXDC();

#define TIMER2_MATCH_FREQ_1K	1
#define TIMER2_MATCH_FREQ_10K	2
void TIMER2_set_match_freq(int match_freq_sel);

void TIMER_PWM_sel();


#endif //__TIMER_H__
