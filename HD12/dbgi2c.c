#include <i2c_master.h>
#include <pca9848.h>
#include <dbgi2c.h>
#include <timer.h>
#include <common.h>
#include <project.h>
#include <debug.h>
#include <system.h>
#include <chip.h>
#include <tick.h>

#define DBGI2C_ADDR_BASE	0x41
#define DBGI2C_I2C_MASTER	I2C1
#define DBGI2C_I2C_TIMEOUT	1

static uint8_t dbg_channel[2] = {0, 1};

static inline uint8_t dbgi2c_idx2addr(int idx)
{
	return DBGI2C_ADDR_BASE - idx;
}

int dbgi2c_i2c_write_byte(int idx, uint8_t data)
{
	pca9848_set(PCA9848_1, 1 << dbg_channel[idx]);
	return i2c_master_write_byte(DBGI2C_I2C_MASTER, dbgi2c_idx2addr(idx),
				     DBGI2C_I2C_TIMEOUT, data);
}

int dbgi2c_i2c_write_block(int idx, uint8_t *data, unsigned int len)
{
	pca9848_set(PCA9848_1, 1 << dbg_channel[idx]);
	return i2c_master_write_block(DBGI2C_I2C_MASTER, dbgi2c_idx2addr(idx),
				      DBGI2C_I2C_TIMEOUT, data, len);
}

int dbgi2c_i2c_read_block(int idx, uint8_t *data, unsigned int len)
{
	pca9848_set(PCA9848_1, 1 << dbg_channel[idx]);
	return i2c_master_read_block(DBGI2C_I2C_MASTER, dbgi2c_idx2addr(idx),
				     DBGI2C_I2C_TIMEOUT, data, len);
}

static inline uint8_t dbgi2c_addr_width(int n)
{
	return (0x20 + (n / 8) - 1);
}
static inline uint8_t dbgi2c_data_width(int n)
{
	switch (n) {
	case 8:
		return 0x10;
	case 16:
		return 0x11;
	case 32:
		return 0x12;
	default:
		return 0x18;
	}
	return 0x18;
}

static int __dbgi2c_write(int idx, int addr_width, int data_width,
			  uint64_t addr, void *data, int len)
{
	uint8_t msg[128];
	int msg_len, i, err;

	/* append data width type */
	msg[0] = dbgi2c_data_width(data_width);

	msg_len = 1;

	/* append address */
	for (i = 0; i < addr_width / 8; ++i, ++msg_len)
		msg[msg_len] = ((uint8_t *)&addr)[i];

	/* append data */
	for (i = 0; i < len; ++i, ++msg_len)
		msg[msg_len] = ((uint8_t *)data)[i];

	err = dbgi2c_i2c_write_byte(idx, dbgi2c_addr_width(addr_width));
	if (err) {
		debug("%d write set address width failed\n", idx);
		return err;
	}
	timer_udelay(10);
	err = dbgi2c_i2c_write_block(idx, msg, msg_len);
	if (err)
		debug("write address or data failed of soc%d\n", idx);

	return err;
}

static int __dbgi2c_read(int idx, int addr_width, int data_width,
			 uint64_t addr, void *data, int len)
{
	uint8_t msg[128];
	int msg_len, i, err;

	/* append data width type */
	msg[0] = dbgi2c_data_width(data_width);
	msg_len = 1;

	/* append address */
	for (i = 0; i < addr_width / 8; ++i, ++msg_len)
		msg[msg_len] = ((uint8_t *)&addr)[i];

	err = dbgi2c_i2c_write_byte(idx, dbgi2c_addr_width(addr_width));
	if (err) {
		debug("%d read set adr width failed\n", idx);
		return err;
	}

	timer_udelay(10);

	err = dbgi2c_i2c_write_block(idx, msg, msg_len);
	if (err) {
		debug("%d set addr %xl failed\n", idx, addr);
		return err;
	}

	timer_udelay(10);

	/* get data */
	err = dbgi2c_i2c_read_block(idx, data, len);
	if (err)
		debug("get data failed\n");

	return err;
}

static inline int get_addr_width(uint64_t addr)
{
	int i;

	for (i = 8; i <= 48; i += 8) {
		if ((addr & ~(((uint64_t )1 << i) - 1)) == 0)
			break;
	}

	return i;
}

int dbgi2c_write32(int idx, uint64_t addr, uint32_t value)
{
	int addr_width = get_addr_width(addr);

	return __dbgi2c_write(idx, addr_width, 32,
			      addr, &value, 4);
}

int dbgi2c_read32(int idx, uint64_t addr, uint32_t *value)
{
	int addr_width = get_addr_width(addr);

	return __dbgi2c_read(idx, addr_width, 32,
			     addr, value, 4);
}

int dbgi2c_write8(int idx, uint64_t addr, uint8_t value)
{
	int addr_width = get_addr_width(addr);

	return __dbgi2c_write(idx, addr_width, 8,
			      addr, &value, 1);
}

int dbgi2c_read8(int idx, uint64_t addr, uint8_t *value)
{
	int addr_width = get_addr_width(addr);

	return __dbgi2c_read(idx, addr_width, 8,
			     addr, value, 1);
}

int dbgi2c_read(int idx, uint64_t addr, void *data, int len)
{
	int addr_width = get_addr_width(addr);

	return __dbgi2c_read(idx, addr_width, 0,
			     addr, data, len);
}

int dbgi2c_write(int idx, uint64_t addr, void *data, int len)
{
	int addr_width = get_addr_width(addr);

	return __dbgi2c_write(idx, addr_width, 0,
			      addr, data, len);
}

// void dbgi2c_test(void)
// {
// 	int i;
// 	uint32_t value = 0;
// 	int err = 0;

// 	for (i = 0; i < SOC_NUM; ++i) {
// 		if (!chip_is_enabled()) {
// 			printf("chip slot %d not enabled\n", i);
// 			continue;
// 		}

// 		printf("chip slot %d\n", i);
// 		err = dbgi2c_read32(i, 0x50010000, &value);
// 		if (err)
// 			printf("failed on dbgi2c_test\n");
// 		else
// 			printf("chip id 0x%08lx\n", value);
// 	}
// }

// static void dbgi2c_collect(void);

/* i2c master operations */
#define DBGI2C_MCU_INFO_BASE	0x7010000000ULL
#define BOARD_TPYE_BASE 	0x70500001c8ULL
void dbgi2c_broadcast(int idx, struct dbgi2c_info *info)
{
	// int i;

	// for (i = 0; i < sizeof(struct dbgi2c_info) / 4; ++i) {
	// 	dbgi2c_write32(idx, DBGI2C_MCU_INFO_BASE + i * 4,
	// 		       ((uint32_t *)info)[i]);
	// }

	dbgi2c_write32(idx, BOARD_TPYE_BASE, BOARD_TPYE);

	// if (idx == 0)
	// 	dbgi2c_collect();

}

#define RESIZE_BAR_32M		0x1ffffffULL
#define RESIZE_BAR_128M		0x7ffffffULL
#define RESIZE_BAR_256M		0xfffffffULL
#define RESIZE_BAR_4G		0xffffffffULL
#define RESIZE_BAR_8G		0x1ffffffffULL
#define RESIZE_BAR_16G		0x3ffffffffULL
#define RESIZE_BAR_32G		0x7ffffffffULL
#define RESIZE_BAR_64G		0xfffffffffULL
#define RESIZE_BAR_128G		0x1fffffffffULL
#define RESIZE_BAR_LENGTH	RESIZE_BAR_32G

#define PCIE_C2C4_X8_DBI2	0x6C08500020ULL
#define PCIE_C2C4_X4_DBI2	0x6C08900020ULL

#define PCIE_C2C4_X8_DBI_BASE   0x6C08400000ULL
#define DBI_RO_WR_EN_REG        0x8bc

#define FUNC_NUM		2
#define FUNC1_OFFSET		16

#define MAX_RETRIES 100
#define RETRY_DELAY_US 10
/* wait pcie clk ready*/
#define PCIE_CLK_READY_TIME	50
static volatile int resize_bar_flag;

static void resize_bar(int idx)
{
	int ret;
	int retry_count;
	int func_num;
	uint64_t dbi2_base_addr;
	// uint32_t val;

	if (idx == 0) {
		dbi2_base_addr = PCIE_C2C4_X8_DBI2;
		func_num = 2;
	} else if (idx == 1) {
		dbi2_base_addr = PCIE_C2C4_X8_DBI2;
		func_num = 1;
	}
	
	
	for (int i = 0; i < func_num; i++) {
		retry_count = 0;
		while ((ret = dbgi2c_write32(idx, dbi2_base_addr + (i << FUNC1_OFFSET), 
					RESIZE_BAR_LENGTH & 0xffffffff)) != 0) {
			if (++retry_count >= MAX_RETRIES) {
				dbg_printf("chip%d func%d pcie mask reg wirte fail, ret= %d\n", idx, i, ret);
				break;
			}
			timer_udelay(RETRY_DELAY_US);
		}
		
		retry_count = 0;
		while ((ret = dbgi2c_write32(idx, dbi2_base_addr + 0x4 + (i << FUNC1_OFFSET), 
						(RESIZE_BAR_LENGTH >> 32) & 0xffffffff)) != 0) {
			if (++retry_count >= MAX_RETRIES) {
				dbg_printf("chip%d func%d pcie mask reg wirte fail, ret= %d\n", idx, i, ret);
				break;
			}
			timer_udelay(RETRY_DELAY_US); 
		}
	}

}

void resize_bar_enable(void)
{
	if (!resize_bar_flag && chip_enable() && (tick64_get() > PCIE_CLK_READY_TIME)) {
		resize_bar_flag = 1;
		dbg_printf("resize_bar start\n");
		resize_bar(0);
		dbg_printf("resize_bar end\n");
	}

	if (!chip_enable())
		resize_bar_flag = 0;
		
}