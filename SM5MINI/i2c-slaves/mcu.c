#include <libopencm3/cm3/nvic.h>
#include <i2c_master.h>
#include <string.h>
#include <project.h>
#include <i2c_slave.h>
#include <common.h>
#include <mon.h>
#include <upgrade.h>
#include <loop.h>
#include <chip.h>
#include <mcu-e2prom.h>
#include <eeprom.h>
#include <power.h>
#include <se5.h>
#include <se6.h>
#include <wdt.h>
#include <mcu.h>
#include <pcie.h>
#include <at24c128c-e2prom.h>

#define EXT_LED_MAX		8

#define REG_BOARD_TYPE		0x00
#define REG_SW_VER		0x01
#define REG_HW_VER		0x02
#define REG_CMD			0x03
#define REG_INT_STATUS		0x06
#define REG_TEST_INTR_EN	0x07
#define REG_INT_MASK		0x08
#define REG_SOC_RST_TIMES	0x0a
#define REG_UPTIME_LO		0x0b
#define REG_UPTIME_HI		0x0c

#define REG_GP0			0x14
#define REG_DDR			0x15
#define REG_PWR_GOOD		0x16
#define REG_MODE_FLAG		0x17

#define REG_TPU_POWER_CONTROL	0x22	/* tpu power control 0: off 1: on */
#define REG_POWER_LO		0x24
#define REG_POWER_HI		0x25
#define REG_VOLTAGE_LO		0x26
#define REG_VOLTAGE_HI		0x27
#define REG_CURRENT_LO		0x28
#define REG_CURRENT_HI		0x29

#define REG_LED_CTRL		0x30

#define REG_STAGE		0x3c
#define REG_SE6_BOARD_ID	0x3d	/* 8bit for se6 board id*/
#define REG_EEPROM_OFFSET_LO	0x3e	/* 16bit eeprom address, low 8bits */
#define REG_EEPROM_OFFSET_HI	0x3f	/* 16bit eeprom address, high 8bits */
#define REG_EEPROM_DATA		0x40	/* eeprom data */
#define REG_EEPROM_LOCK		0x60	/* eeprom write lock */
#define REG_SE6_BOARD_IP_0	0x61    /* IP address 1th 8 bits */
#define REG_SE6_BOARD_IP_1	0x62    /* IP address 2th 8 bits */
#define REG_SE6_BOARD_IP_2	0x63    /* IP address 3th 8 bits */
#define REG_SE6_BOARD_IP_3	0x64    /* IP address 4th 8 bits */

#define MCU_REG_MAX		0x65

#define MCU_EEPROM_DATA_MAX	0x20

struct mcu_ctx {
	int set_idx;
	int idx;
	int cmd_tmp;
	uint8_t cmd;
	uint8_t int_status;
	uint8_t gp0;
	uint8_t eeprom_offset_l, eeprom_offset_h;
	int test_mode;

	uint16_t tmp;
	uint8_t brd_ip[4];  /* for se6 aiub board ip */
	uint8_t brd_id;	    /* for se6 aiub board id */
	bool is_se6_aiucore:1;
};

static struct mcu_ctx mcu_ctx;

static const char *tpu_powers[] = {
		"VDD-TPU",
		"ACK-TPU",
		"VDD-TPU-MEM",
		"ACK-TPU_MEM"
};

static void tpu_power_setup(unsigned char enable)
{
	if (enable)
		power_nodes_on(tpu_powers, ARRAY_SIZE(tpu_powers));
	else
		power_nodes_off(tpu_powers, ARRAY_SIZE(tpu_powers));
}

static int tpu_get_power_status(void)
{
	return power_nodes_status(tpu_powers, ARRAY_SIZE(tpu_powers));
}

int mcu_get_test_mode(void)
{
	return mcu_ctx.test_mode;
}

static void mcu_set_se6_aiucore(void)
{
	pcie_init();//open pciee reset
	mcu_ctx.is_se6_aiucore = true;
}

/*
 * if is aiu core return ture else false
 */
bool mcu_get_se6_aiucore(void)
{
	return mcu_ctx.is_se6_aiucore;
}

void mcu_set_test_mode(int mode)
{
	mcu_ctx.test_mode = mode;
}

void mcu_raise_interrupt(uint8_t interrupts)
{
	mcu_ctx.int_status |= interrupts;
	if (mcu_ctx.int_status)
		gpio_set(MCU_INT_PORT, MCU_INT_PIN);
}

void mcu_clear_interrupt(uint8_t interrupts)
{
	mcu_ctx.int_status &= ~interrupts;
	if (mcu_ctx.int_status == 0)
		gpio_clear(MCU_INT_PORT, MCU_INT_PIN);
}

static struct {
	int num;
	struct {
		int port, pin;
	} pins[EXT_LED_MAX];
} ext_led;

int register_ext_led(int port, int pin)
{
	if (ext_led.num >= ARRAY_SIZE(ext_led.pins))
		return -1;

	ext_led.pins[ext_led.num].port = port;
	ext_led.pins[ext_led.num].pin = pin;

	gpio_clear(port, pin);
	gpio_mode_setup(port, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLDOWN, pin);

	++ext_led.num;

	return 0;
}

static void set_ext_led_status(uint8_t data)
{
	int i;

	for (i = 0; i < ext_led.num; ++i) {
		if (data & (1 << i))
			gpio_set(ext_led.pins[i].port, ext_led.pins[i].pin);
		else
			gpio_clear(ext_led.pins[i].port, ext_led.pins[i].pin);
	}
}

static uint8_t get_ext_led_status(void)
{
	int tmp, i;
	uint8_t status = 0xff;

	for (i = 0; i < ext_led.num; ++i) {
		tmp = gpio_get(ext_led.pins[i].port, ext_led.pins[i].pin);
		status &= ~((tmp ? 0 : 1) << i);
	}

	return status;
}

void mcu_set_gp0(uint8_t data)
{
	mcu_ctx.gp0 = data;
}

static inline void idx_set(struct mcu_ctx *ctx, uint8_t idx)
{
	ctx->idx = idx % MCU_REG_MAX;
}

static inline void idx_inc(struct mcu_ctx *ctx)
{
	ctx->idx = (ctx->idx + 1) % MCU_REG_MAX;
}

static void mcu_match(void *priv, int dir)
{
	struct mcu_ctx *ctx = priv;

	if (dir == I2C_SLAVE_WRITE)
		ctx->set_idx = 1;
}

#define CMD_POWER_OFF			0x02

#define CMD_RESET				0x03	// drag reset pin
#define CMD_REBOOT				0x07	// power off - power on
#define CMD_UPDATE				0x08

#define CMD_SE6_AIU_POWER_ON	0x09	/* exit test mode for sm5mini */


void mcu_process(void)
{
	if (mcu_ctx.cmd == 0)
		return;

	i2c_peripheral_disable(I2C1);
	nvic_disable_irq(NVIC_I2C1_IRQ);
	switch (mcu_ctx.cmd) {
	case CMD_SE6_AIU_POWER_ON:
		mcu_set_test_mode(false);
		mcu_set_se6_aiucore();
		break;
	case CMD_POWER_OFF:
		mcu_eeprom_power_off_reason(EEPROM_POWER_OFF_REASON_POWER_OFF);
		if (get_board_type() == SM5ME)
			se5_power_off_board();
		else
			power_off();
		wdt_reset();
		break;
	case CMD_RESET:
		mcu_eeprom_power_off_reason(EEPROM_POWER_OFF_REASON_RESET);
		chip_reset();
		wdt_reset();
		break;
	case CMD_REBOOT:
		mcu_eeprom_power_off_reason(EEPROM_POWER_OFF_REASON_REBOOT);
		if (get_board_type() == SM5ME)
			se5_reset_board();
		if (get_board_type() == SM5SE6M){
			se6ctrl_clean_restart();
			chip_popd_reset();
		}
		else
			chip_popd_reset();
		wdt_reset();
		break;
	case CMD_UPDATE:
		nvic_enable_irq(NVIC_I2C1_IRQ);
		i2c_upgrade_start();
		break;
	default:
		break;
	}
	mcu_ctx.cmd = 0;
	mcu_ctx.cmd_tmp = 0;
	i2c_peripheral_enable(I2C1);
	nvic_enable_irq(NVIC_I2C1_IRQ);
}

static inline uint16_t eeprom_offset(struct mcu_ctx *ctx)
{
	uint16_t offset_base = ctx->eeprom_offset_l |
		(ctx->eeprom_offset_h << 8);
	uint16_t offset_off =
		ctx->idx - REG_EEPROM_DATA;
	return offset_base + offset_off;
}

static void mcu_write(void *priv, volatile uint8_t data)
{
	struct mcu_ctx *ctx = priv;

	if (ctx->set_idx) {
		idx_set(ctx, data);
		ctx->set_idx = 0;
		return;
	}

	switch (ctx->idx) {
	case REG_CMD:
		ctx->cmd_tmp = data;
		break;
	case REG_INT_STATUS:
		mcu_clear_interrupt(data);
		break;
	case REG_TEST_INTR_EN:
		if (data == 0x5a)
			mcu_raise_interrupt(MCU_INT_TEST_INTR);
		break;
	case REG_GP0:
		ctx->gp0 = data;
		break;
	case REG_LED_CTRL:
		set_ext_led_status(data);
		break;
	case REG_EEPROM_OFFSET_LO:
		ctx->eeprom_offset_l = data;
		break;
	case REG_EEPROM_OFFSET_HI:
		ctx->eeprom_offset_h = data;
		break;
	case REG_EEPROM_DATA ...
		(REG_EEPROM_DATA + MCU_EEPROM_DATA_MAX - 1):
		mcu_eeprom_write_byte_protected(eeprom_offset(ctx), data);
		break;
	case REG_EEPROM_LOCK:
		eeprom_lock_code(data);
		break;
	case REG_TPU_POWER_CONTROL:
		tpu_power_setup(data);
		break;
	case REG_SE6_BOARD_ID:
		ctx->brd_id = data;
		break;
	case REG_SE6_BOARD_IP_0:
		ctx->brd_ip[0] = data;
		break;
	case REG_SE6_BOARD_IP_1:
		ctx->brd_ip[1] = data;
		break;
	case REG_SE6_BOARD_IP_2:
		ctx->brd_ip[2] = data;
		break;
	case REG_SE6_BOARD_IP_3:
		ctx->brd_ip[3] = data;
		break;
	default:
		break;
	}

	idx_inc(ctx);
}

static uint8_t mcu_read(void *priv)
{
	struct mcu_ctx *ctx = priv;
	uint8_t ret = 0;

	switch (ctx->idx) {
	case REG_BOARD_TYPE:
		ret = get_board_type();
		break;
	case REG_SW_VER:
		ret = get_firmware_version();
		break;
	case REG_HW_VER:
		ret = get_hardware_version();
		break;
	case REG_CMD:
		ret = 0;
		break;
	case REG_INT_STATUS:
		ret = ctx->int_status;
		break;
	case REG_INT_MASK:
		/* mask is not supported */
		ret = 0;
		break;
	case REG_SOC_RST_TIMES:
		ret = chip_reset_times() & 0xff;
		break;
	case REG_UPTIME_LO:
		ctx->tmp = chip_uptime();
		ret = ctx->tmp & 0xff;
		break;
	case REG_UPTIME_HI:
		ret = (ctx->tmp >> 8) & 0xff;
		break;
	case REG_GP0:
		ret = ctx->gp0;
		break;
	case REG_DDR:
		ret = get_ddr_type();
		break;
	case REG_PWR_GOOD:
		ret = 1;
		break;
	case REG_MODE_FLAG:
		ret = get_work_mode();
		break;
	case REG_POWER_LO:
		ctx->tmp = get_current() * 12;
		ret = ctx->tmp & 0xff;
		break;
	case REG_POWER_HI:
		ret = (ctx->tmp >> 8) & 0xff;
		break;
	case REG_CURRENT_LO:
		ctx->tmp = get_current();
		ret = ctx->tmp & 0xff;
		break;
	case REG_CURRENT_HI:
		ret = (ctx->tmp >> 8) & 0xff;
		break;
	case REG_LED_CTRL:
		ret = get_ext_led_status();
		break;
	case REG_STAGE:
		ret = get_stage();
		break;
	case REG_EEPROM_OFFSET_LO:
		ret = ctx->eeprom_offset_l;
		break;
	case REG_EEPROM_OFFSET_HI:
		ret = ctx->eeprom_offset_h;
		break;
	case REG_EEPROM_DATA ...
		(REG_EEPROM_DATA + MCU_EEPROM_DATA_MAX - 1):
		ret = mcu_eeprom_read_byte(NULL, eeprom_offset(ctx));
		break;
	case REG_EEPROM_LOCK:
		ret = eeprom_get_lock_status();
		break;
	case REG_TPU_POWER_CONTROL:
		ret = tpu_get_power_status();
		break;
	case REG_SE6_BOARD_ID:
		ret = ctx->brd_id;
		break;
	case REG_SE6_BOARD_IP_0:
		ret = ctx->brd_ip[0];
		break;
	case REG_SE6_BOARD_IP_1:
		ret = ctx->brd_ip[1];
		break;
	case REG_SE6_BOARD_IP_2:
		ret = ctx->brd_ip[2];
		break;
	case REG_SE6_BOARD_IP_3:
		ret = ctx->brd_ip[3];
		break;
	default:
		ret = 0xff;
		break;
	}

	idx_inc(ctx);
	return ret;
}

static void mcu_stop(void *priv)
{
	struct mcu_ctx *ctx = priv;
	ctx->cmd = ctx->cmd_tmp;
}

static void mcu_reset(void *priv)
{
	struct mcu_ctx *ctx = priv;
	memset(ctx, 0x00, sizeof(*ctx));
}

static struct i2c_slave_op slave = {
	.addr = 0x17,	/* mcu common slave address */
	.mask = 0x00,
	.match = mcu_match,
	.write = mcu_write,
	.read = mcu_read,
	.stop = mcu_stop,
	.reset = mcu_reset,
	.priv = &mcu_ctx,
};

void mcu_init(struct i2c_slave_ctx *i2c_slave_ctx)
{
	loop_add(mcu_process);
	slave.addr = 0x17;
	i2c_slave_register(i2c_slave_ctx, &slave);
}

void mcu_test_init(struct i2c_slave_ctx *i2c_slave_ctx)
{
	slave.addr = 0x38;
	i2c_slave_register(i2c_slave_ctx, &slave);
}

