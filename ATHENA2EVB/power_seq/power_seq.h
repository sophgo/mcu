/*
 * power_seq.h: CV84 兼容上电时序 (ATHENA2 引脚映射)
 */
#ifndef __POWER_SEQ_H__
#define __POWER_SEQ_H__

#define POWER_SEQ_NUM		6
#define POWER_SEQ_GAP_NUM	(POWER_SEQ_NUM - 1)
#define POWER_SEQ_DEFAULT_GAP_US	2000U	/* 默认 2ms */

void power_seq_init(void);
void power_seq_on(void);
void power_seq_off(void);
void power_seq_set_gaps_us(const unsigned int gaps[POWER_SEQ_GAP_NUM]);
void power_seq_get_gaps_us(unsigned int gaps[POWER_SEQ_GAP_NUM]);
int power_seq_status(void);

#endif /* __POWER_SEQ_H__ */
