#include <i2c_master.h>
#include <i2c_slave.h>
#include <pic.h>
#include <libopencm3/cm3/nvic.h>

#define PIC_ADDR		0x24
#define PIC_I2C			I2C2
/* smbus timeout, count in ms */
#define PIC_SMBTO		1

#define ERROR_RETRY		10

static int is_pic_available;

int pic_available(void)
{
	return is_pic_available;
}

int pic_probe(void)
{
	uint8_t tmp;
	int err;

	err = i2c_master_smbus_read_byte(PIC_I2C, PIC_ADDR, PIC_SMBTO,
					 PIC_REG_BOARD_TYPE, &tmp);

	return is_pic_available = (err == 0);
}

int pic_write(uint8_t reg, uint8_t val)
{
	int err;
	int i = 0;

	nvic_disable_irq(NVIC_I2C1_IRQ);

	do {
		err = i2c_master_smbus_write_byte(
			PIC_I2C, PIC_ADDR, PIC_SMBTO, reg, val);
		++i;
	} while (err && i <= ERROR_RETRY);

	nvic_enable_irq(NVIC_I2C1_IRQ);

	return err ? -1 : 0;
}


int pic_read(uint8_t reg)
{
	uint8_t tmp;
	int err;
	int i = 0;

	nvic_disable_irq(NVIC_I2C1_IRQ);

	do {
		err = i2c_master_smbus_read_byte(PIC_I2C, PIC_ADDR, PIC_SMBTO,
						 reg, &tmp);
		++i;
	} while (err && i <= ERROR_RETRY);

	nvic_enable_irq(NVIC_I2C1_IRQ);

	return err ? -1 : tmp;
}


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

void pic_init(struct i2c_slave_ctx *i2c)
{
	i2c_slave_register(i2c, &slave);
}
