#include <i2c_slave.h>
#include <flash.h>
#include <checksum.h>
#include <stdlib.h>
#include <string.h>
#include <project.h>

#define REG_CMD			(0x03)
#define REG_STAGE		(0x3c)

#define REG_LOG			(0x62)
#define REG_CALC_CKSUM		(0x63)
#define REG_CKSUM_OFF		(0x64)
#define REG_CKSUM_LEN		(0x68)
#define REG_CKSUM		(0x6c)
#define REG_OFFSET		(0x7c)
#define REG_DATA		(0x80)
#define REG_FLUSH		(0xff)

#define CMD_POWER_OFF		(0x02)
#define CMD_RESET		(0x03)
#define CMD_REBOOT		(0x07)       // power is always on

/* smbus index to host endian with give type */
#define S2HT(base, idx, type, var, data)				\
	do {								\
		int total_byte = sizeof(type);				\
		int idx_byte = idx - base;				\
		int shift = (total_byte - 1 - idx_byte) * 8;		\
		var &= ~(0xff << shift);				\
		var |= ((uint8_t)data) << shift;			\
	} while (0)

#define H2ST(base, idx, type, var)					\
	({								\
	 int total_byte = sizeof(type);					\
	 int idx_byte = idx - base;					\
	 int shift = (total_byte - 1 - idx_byte) * 8;			\
	 (uint8_t)((((type)var) >> shift) & 0xff);			\
	 })

volatile uint8_t upgrade_data[128];
volatile uint32_t flash_off;
volatile uint8_t calc_cksum;
volatile uint32_t cksum_off;
volatile uint32_t cksum_len;
volatile uint8_t cksum[16];

#define LOG_BUF_LEN	(1024)
#define LOG_BUF_MASK	(LOG_BUF_LEN - 1)

volatile uint8_t log_buf[LOG_BUF_LEN];
volatile uint8_t log_wr_idx;
volatile uint8_t log_rd_idx;

void upgrader_log(const char *log)
{
	const char *p;

	if (log == NULL)
		return;
	for (p = log; *p; ++p) {
		log_buf[log_wr_idx] = *p;
		log_wr_idx = (log_wr_idx + 1) & LOG_BUF_MASK;
	}
}

static struct upgrader_ctx {
	int set_idx;
	int idx;
	int flush_data;
} upgrader_ctx1, upgrader_ctx3;

struct efie {
	uint32_t	offset;
	uint32_t	length;
	uint8_t		checksum[16];
	uint8_t		reserved[104];
} __attribute__((packed));

static inline void idx_set(struct upgrader_ctx *ctx, uint8_t idx)
{
	ctx->idx = idx & 0xff;
}
static inline void idx_inc(struct upgrader_ctx *ctx)
{
	ctx->idx = (ctx->idx + 1) & 0xff;
}
static inline int idx_get(struct upgrader_ctx *ctx)
{
	return ctx->idx;
}

static void upgrader_match(void *priv, int dir)
{
	struct upgrader_ctx *ctx = priv;
	if (dir == I2C_SLAVE_WRITE){
		ctx->set_idx = 1;
	}
}

static void flush_data(void)
{
	flash_program(flash_off & ~(128 - 1), (void *)upgrade_data, 128);
}

void upgrader_calc_cksum(void)
{
	if (calc_cksum == 0)
		return;

	checksum((void *)cksum,
		 (void *)(FLASH_PROGRAM_START + cksum_off),
		 cksum_len);
	calc_cksum = 0;
}

static void upgrader_cmd_process(uint8_t cmd)
{

	switch (cmd) {
	case CMD_POWER_OFF:
	case CMD_RESET:
	case CMD_REBOOT:
		NVIC_SystemReset();
		break;
	}
}

static void upgrader_write(void *priv, uint8_t data)
{
	struct upgrader_ctx *ctx = priv;
	if (ctx->set_idx) {
		idx_set(ctx, data);
		ctx->set_idx = 0;
		return;
	}

	int idx = idx_get(ctx);

	switch (idx) {
	case REG_CALC_CKSUM:
		calc_cksum = 1;
		break;
	case REG_CKSUM_OFF ... REG_CKSUM_OFF + 3:
		S2HT(REG_CKSUM_OFF, idx, uint32_t, cksum_off, data);
		break;
	case REG_CKSUM_LEN ... REG_CKSUM_LEN + 3:
		S2HT(REG_CKSUM_LEN, idx, uint32_t, cksum_len, data);
		break;
	case REG_DATA ... REG_FLUSH:
		upgrade_data[idx - REG_DATA] = data;
		if (idx == REG_FLUSH)
			ctx->flush_data = 1;
		break;
	case REG_OFFSET ... REG_OFFSET + 3:
		S2HT(REG_OFFSET, idx, uint32_t, flash_off, data);
		break;
	case REG_LOG:
		log_rd_idx = data;
		break;
	case REG_CMD:
		upgrader_cmd_process(data);
		break;
	default:
		break;
	}
	idx_inc(ctx);
}

static uint8_t upgrader_read(void *priv)
{
	uint8_t ret = 0;
	struct upgrader_ctx *ctx = priv;
	int idx = idx_get(ctx);

	switch (idx) {
	case REG_STAGE:
		ret = 2;
		break;
	case REG_CALC_CKSUM:
		ret = calc_cksum;
		break;
	case REG_CKSUM_OFF ... REG_CKSUM_OFF + 3:
		ret = H2ST(REG_CKSUM_OFF, idx, uint32_t, cksum_off);
		break;
	case REG_CKSUM_LEN ... REG_CKSUM_LEN + 3:
		ret = H2ST(REG_CKSUM_LEN, idx, uint32_t, cksum_len);
		break;
	case REG_CKSUM ... REG_CKSUM + 15:
		ret = *(uint8_t *)(cksum + (idx - REG_CKSUM));
		break;
	case REG_DATA ... REG_FLUSH:
		ret = *(uint8_t *)(FLASH_PROGRAM_START +
				flash_off + (idx - REG_DATA));
		break;
	case REG_OFFSET ... REG_OFFSET + 3:
		ret = H2ST(REG_OFFSET, idx, uint32_t, flash_off);
		break;
	case REG_LOG:
		ret = log_buf[log_rd_idx++];
		break;
	}
	idx_inc(ctx);
	return ret;
}


static void upgrader_stop(void *priv)
{
	struct upgrader_ctx *ctx = priv;
	if (ctx->flush_data) {
		flush_data();
		ctx->flush_data = 0;
	}
}

static struct i2c_slave_op slave1 = {
	.addr = 0x38,
	.mask = 0x07,
	.match = upgrader_match,
	.write = upgrader_write,
	.read = upgrader_read,
	.stop = upgrader_stop,
	.priv = &upgrader_ctx1,
};

static struct i2c_slave_op slave3 = {
	.addr = 0x17,
	.match = upgrader_match,
	.write = upgrader_write,
	.read = upgrader_read,
	.stop = upgrader_stop,
	.priv = &upgrader_ctx3,
};

uint32_t project;
struct i2c_slave_ctx i2c1;
struct i2c_slave_ctx i2c3;

static void i2c1_init(uint8_t addr, uint8_t mask)
{
	i2c1.id = 1;
	memset(&upgrader_ctx1, 0x00, sizeof(upgrader_ctx1));
	slave1.addr = addr;
	slave1.mask = mask;

	i2c_slave_init(&i2c1, (void *)I2C1_BASE);
	i2c_slave_register(&i2c1, &slave1);
	i2c_slave_start(&i2c1);
}

static void i2c3_init(uint8_t addr, uint8_t mask)
{
	i2c3.id = 3;
	memset(&upgrader_ctx3, 0x00, sizeof(upgrader_ctx3));
	slave3.addr = addr;
	slave3.mask = mask;

	i2c_slave_init(&i2c3, (void *)I2C3_BASE);
	i2c_slave_register(&i2c3, &slave3);
	i2c_slave_start(&i2c3);
}

void upgrader_init(void)
{
	switch(project) {
	case EVB:
	case SC5H3:
	case SC5H:
	case SM5ME:
	case SM5MP:
	case SM5MS:
	case SM5MA:
	case SE5LITE:
	case SM5SE6M:
	case BM1684XEVB:
	case SM7M:
	case SM7SE6M:
	case SM7MQY:
	case SE7Q:
	case SM7M_MP_1_1:
	case SM7_HK:
	case SE7:
	case SM7MSE6M:
	case SM7CUSTV1:
		i2c1_init(0x17, 0x00);
		break;
	default:
		/* sa5, sm5p, sm5s, se5 type */
		i2c1_init(0x38, 0x07);
		i2c3_init(0x17, 0x00);
		break;
	}
}

