#ifndef __CT7451_H__
#define __CT7451_H__

void ct7451_init(int idx);
void ct7451_get_temp(int *remote, int *local);
void ct7451_process(void);

#endif /* __CT7451_H__ */
