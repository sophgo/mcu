/*
 * power_seq.h: CV84x6 上电时序控制
 *
 * (C) Copyright 2024 Sophgo Technology
 */
#ifndef __POWER_SEQ_H__
#define __POWER_SEQ_H__

#define POWER_SEQ_NUM	6

void power_seq_init(void);
void power_seq_on(void);
void power_seq_on_with_delays(const unsigned int delays[POWER_SEQ_NUM - 1]);
void power_seq_off(void);
void power_seq_set_delay_ms(unsigned int ms);
unsigned int power_seq_get_delay_ms(void);
int power_seq_status(void);

#endif /* __POWER_SEQ_H__ */