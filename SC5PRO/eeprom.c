#include <i2c_slave.h>
#include <libopencm3/stm32/flash.h>

#define EEPROM_BASE_ADDR	0x08080C00
#define EEPROM_BANK_SIZE	0x0C00
#define EEPROM_SIZE		(EEPROM_BANK_SIZE)

static inline uint8_t eeprom_read_byte(uint32_t offset)
{
	return *((volatile uint8_t *)(EEPROM_BASE_ADDR + offset));
}

static inline void eeprom_write_byte(uint32_t offset, uint8_t data)
{
	eeprom_program_byte(EEPROM_BASE_ADDR + offset, data);
}

static struct eeprom_ctx {
	int set_idx;
	int idx;
	unsigned char tmp;
} ctx;

static void eeprom_match(void *priv, int dir)
{
	if (dir == I2C_SLAVE_WRITE) {
		ctx.set_idx = 2;
	}
}

static inline void idx_set_lo(uint8_t idx)
{
	ctx.idx |= idx;
	ctx.idx = ctx.idx % EEPROM_SIZE;
}
static inline void idx_set_hi(uint8_t idx)
{
	ctx.idx = idx << 8;
}
static inline void idx_inc(void)
{
	ctx.idx = (ctx.idx + 1) % EEPROM_SIZE;
}

static void eeprom_write(void *priv, uint8_t data)
{
	switch (ctx.set_idx) {
	case 2:
		ctx.tmp = data;
		ctx.set_idx = 1;
		return;
	case 1:
		idx_set_hi(ctx.tmp);
		idx_set_lo(data);
		ctx.set_idx = 0;
		return;
	default:
		break;
	}
	eeprom_write_byte(ctx.idx, data);
	idx_inc();
}

static uint8_t eeprom_read(void *priv)
{
	uint8_t tmp;
	if (ctx.set_idx == 1)
	{
		idx_set_hi(0);
		idx_set_lo(ctx.tmp);
	}
	tmp = eeprom_read_byte(ctx.idx);
	idx_inc();
	return tmp;
}

static struct i2c_slave_op slave = {
	.addr = 0x6a,
	.match = eeprom_match,
	.write = eeprom_write,
	.read = eeprom_read,
};

extern struct i2c_slave_ctx i2c1_slave_ctx;

void eeprom_init(void)
{
	i2c_slave_register(&i2c1_slave_ctx, &slave);
}

