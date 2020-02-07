#include <i2c_slave.h>

extern struct i2c_slave_ctx i2c1_slave_ctx;

void i2c1_isr(void)
{
	i2c_slave_isr(&i2c1_slave_ctx);
}
