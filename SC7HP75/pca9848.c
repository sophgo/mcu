#include <pca9848.h>
#include <stdio.h>
#include <common.h>
#include <gd32e50x_i2c.h>
#include <system.h>

#define PCA9848_SLAVE_ADDR	0x71

static struct {
	int i2c;
	unsigned int mask;
} ctx[1] = {
	{.i2c = I2C0, .mask = 0},
};

void pca9848_init(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(ctx); ++i) {
		ctx[i].mask = 0;
		if (i2c_master_smbus_write(ctx[i].i2c, PCA9848_SLAVE_ADDR, 1, 0))
			printf("tca9548a smbus write failed\n");
	}
}

void pca9848_set(int id, unsigned int mask)
{
	if (ctx[id].mask != mask) {
		if (i2c_master_smbus_write(ctx[id].i2c, PCA9848_SLAVE_ADDR, 1, mask))
			printf("tca9548a smbus write failed\n");
		ctx[id].mask = mask;
	}
}
