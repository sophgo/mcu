#include <pca9848.h>
#include <stdio.h>
#include <common.h>
#include <gd32e50x_i2c.h>
#include <system.h>
#include <debug.h>

//#define PCA9848_SLAVE_ADDR	0x71

static struct {
	int i2c;
	unsigned int mask;
	unsigned char slave_addr;
} ctx[] = {
	{.i2c = I2C1, .mask = 0, .slave_addr = 0x73},
};

void pca9848_init(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(ctx); ++i) {
		ctx[i].mask = 0;
		if (i2c_master_smbus_write(ctx[i].i2c, ctx[i].slave_addr, 1, 0))
			dbg_printf("pca9548a smbus write failed\n");
	}
}

void pca9848_set(int id, unsigned int mask)
{ 
	if (ctx[id].mask != mask) {
		if (i2c_master_smbus_write(ctx[id].i2c, ctx[id].slave_addr, 1, mask))
			dbg_printf("pca9548a smbus write failed\n");
		ctx[id].mask = mask;
	}
}
