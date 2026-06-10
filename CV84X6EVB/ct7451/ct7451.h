/*
 * ct7451.h: CT7451 温度传感器驱动
 */
#ifndef __CT7451_H__
#define __CT7451_H__

#define CT7451_SLAVE_ADDR       0x4C    /* 7-bit addr, 8-bit = 0x99 */

void ct7451_init(void);
void ct7451_process(void);

extern int ct7451_remote_temp;
extern int ct7451_local_temp;

#endif /* __CT7451_H__ */
