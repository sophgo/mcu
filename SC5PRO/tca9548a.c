#include <tca9548a.h>

#define TCA9548A_SLAVE_ADDR	0x70

static struct {
	int i2c;
	unsigned int mask;
} ctx[2] = {
	{.i2c = I2C1, .mask = 0},
	{.i2c = I2C2, .mask = 0},
};

void tca9548a_set(int id, unsigned int mask)
{
	if (ctx[id].mask != mask) {
		i2c_master_smbus_write(ctx[id].i2c, TCA9548A_SLAVE_ADDR,
				       1, mask);
		ctx[id].mask = mask;
	}
}
