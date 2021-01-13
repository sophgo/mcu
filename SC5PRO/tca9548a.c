#include <tca9548a.h>
#include <stdio.h>
#include <common.h>

#define TCA9548A_SLAVE_ADDR	0x70

static struct {
	int i2c;
	unsigned int mask;
} ctx[2] = {
	{.i2c = I2C1, .mask = 0},
	{.i2c = I2C2, .mask = 0},
};

void tca9548a_init(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(ctx); ++i) {
		ctx[i].mask = 0;
		if (i2c_master_smbus_write(ctx[i].i2c, TCA9548A_SLAVE_ADDR,
					   1, 0)) {
			printf("tca9548a smbus write failed\n");
		}
	}
}

void tca9548a_set(int id, unsigned int mask)
{
	if (ctx[id].mask != mask) {
		if (i2c_master_smbus_write(ctx[id].i2c, TCA9548A_SLAVE_ADDR,
					     1, mask)) {
			printf("tca9548a smbus write failed\n");
		}
		ctx[id].mask = mask;
	}
}
