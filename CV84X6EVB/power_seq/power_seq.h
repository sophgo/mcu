/*
 * power_seq.h: CV84x6 上电时序控制
 *
 * -u: gap(us) 后依次拉高 6 个引脚
 * -d: 6 个引脚立即全部拉低
 * -w: 仅设置 5 个间隔 (us)，需再执行 -u
 *
 * (C) Copyright 2024 Sophgo Technology
 */
#ifndef __POWER_SEQ_H__
#define __POWER_SEQ_H__

#define POWER_SEQ_NUM		6
#define POWER_SEQ_GAP_NUM	(POWER_SEQ_NUM - 1)
#define POWER_SEQ_DEFAULT_GAP_US	2000U	/* 默认 2ms */
#define POWER_SEQ_FIRST_GAP_US		80000U	/* 第一个间隔默认 80ms (EN12V->PWREN1) */

void power_seq_init(void);
void power_seq_on(void);
void power_seq_off(void);
void power_seq_set_gaps_us(const unsigned int gaps[POWER_SEQ_GAP_NUM]);
void power_seq_get_gaps_us(unsigned int gaps[POWER_SEQ_GAP_NUM]);
int power_seq_status(void);

#endif /* __POWER_SEQ_H__ */
