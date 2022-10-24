#ifndef __TMP451_H__
#define __TMP451_H__
#include <i2c01_slave.h>

void tmp451_init(struct i2c01_slave_ctx *i2c_slave_ctx);
void tmp451_get_temp(int *board, int *soc);
void tmp451_process(void);

#endif /* DS1307_H_ */
