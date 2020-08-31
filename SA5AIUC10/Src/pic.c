#include <i2c.h>
#include <pic.h>
#include <i2c_slave.h>

volatile int is_pic_available;

static struct pic_ctx {
	int set_idx;
	uint8_t idx;
} pic_ctx;

static void pic_i2c_slave_match(void *priv, int dir)
{
	if (dir == I2C_SLAVE_WRITE)
		pic_ctx.set_idx = 1;
}

static void pic_i2c_slave_write(void *priv, uint8_t data)
{
	if (pic_ctx.set_idx) {
		pic_ctx.idx = data;
		pic_ctx.set_idx = 0;
		return;
	}
	pic_write(pic_ctx.idx, data);

	++pic_ctx.idx;
}

static uint8_t pic_i2c_slave_read(void *priv)
{
	int err;

	err = pic_read(pic_ctx.idx);
	++pic_ctx.idx;
	return err < 0 ? 0 : err;
}

static struct i2c_slave_op slave = {
	.addr = 0x6e,	/* pic common slave address */
	.match = pic_i2c_slave_match,
	.write = pic_i2c_slave_write,
	.read = pic_i2c_slave_read,
};

void pic_i2c_slave_init(void)
{
	i2c_slave_register(&i2c_ctx3, &slave);
}
