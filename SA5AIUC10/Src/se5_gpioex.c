#include <assert.h>
#include "stm32l0xx_hal.h"
#include "i2c.h"
#include "i2c_slave.h"

/* port 0 */
#define ALL_RESET_PORT      0
#define ALL_RESET_PIN       0
#define ALL_RESET_DEFAULT   1

#define PCB_VERSION0_PORT    0
#define PCB_VERSION0_PIN     1

#define PCB_VERSION1_PORT    0
#define PCB_VERSION1_PIN     2

#define LED1_PORT       0
#define LED1_PIN        3
#define LED1_DEFAULT    0

#define LED2_PORT       0
#define LED2_PIN        4
#define LED2_DEFAULT    0

#define SATA_INT_PORT   0
#define SATA_INT_PIN    5

#define CLK_EN_PORT     0
#define CLK_EN_PIN      6
#define CLK_EN_DEFAULT  0

#define LTE_RST_PORT    0
#define LTE_RST_PIN     7
#define LTE_RST_DEFAULT 0

#define P0_MASK		((1 << ALL_RESET_PIN) | \
			 (1 << PCB_VERSION0_PIN) | \
			 (1 << PCB_VERSION1_PIN) | \
			 (1 << LED1_PIN) | \
			 (1 << LED2_PIN) | \
			 (1 << SATA_INT_PIN) | \
			 (1 << CLK_EN_PIN) | \
			 (1 << LTE_RST_PIN))

#define P0_CFG		((1 << PCB_VERSION0_PIN) | \
			 (1 << PCB_VERSION1_PIN) | \
			 (1 << SATA_INT_PIN))

#define P0_OUT		((ALL_RESET_DEFAULT << ALL_RESET_PIN) | \
			 (LED1_DEFAULT << LED1_PIN) | \
			 (LED2_DEFAULT << LED2_PIN) | \
			 (CLK_EN_DEFAULT << CLK_EN_PIN) | \
			 (LTE_RST_DEFAULT << LTE_RST_PIN))

#define P0_POL		0

/* port 1 */
#define LTE_PWR_PORT	1
#define LTE_PWR_PIN	0
#define LTE_PWR_DEFAULT	0

#define PCIE_INT_PORT	1
#define	PCIE_INT_PIN	1

#define SATA_EN_PORT	1
#define SATA_EN_PIN	2
#define SATA_EN_DEFAULT	0

#define HEALTHY_LED_PORT	1
#define HEALTHY_LED_PIN		3
#define HEALTHY_LED_DEFAULT	0

#define ETH_CTL_PORT		1
#define ETH_CTL_PIN		4
#define ETH_CTL_DEFAULT		0

#define FL2K_RST_PORT		1
#define FL2K_RST_PIN		5
#define FL2K_RST_DEFAULT	0

#define RST_KEY_RST_PORT	1
#define RST_KEY_RST_PIN		6
#define RST_KEY_RST_DEFAULT	0

#define ASM3142_POR_PORT	1
#define ASM3142_POR_PIN		7
#define ASM3142_POR_DEFAULT	0

#define P1_MASK			((1 << LTE_PWR_PIN) | \
				 (1 << PCIE_INT_PIN) | \
				 (1 << SATA_EN_PIN) | \
				 (1 << HEALTHY_LED_PIN) | \
				 (1 << ETH_CTL_PIN) | \
				 (1 << FL2K_RST_PIN) | \
				 (1 << RST_KEY_RST_PIN) | \
				 (1 << ASM3142_POR_PIN))

#define P1_OUT			((1 << LTE_PWR_DEFAULT) | \
				 (1 << SATA_EN_DEFAULT) | \
				 (1 << HEALTHY_LED_DEFAULT) | \
				 (1 << ETH_CTL_DEFAULT) | \
				 (1 << FL2K_RST_DEFAULT) | \
				 (1 << RST_KEY_RST_DEFAULT) | \
				 (1 << ASM3142_POR_DEFAULT))

#define P1_CFG		(1 << PCIE_INT_PIN)
#define P1_POL		0

#define TCA6416A_ADDR	(0x20 << 1)

/* smbus timeout, count in ms */
#define SMBTO		        2

#define TCA6416A_P0_IN		0
#define TCA6416A_P1_IN		1
#define TCA6416A_P0_OUT		2
#define TCA6416A_P1_OUT		3
#define TCA6416A_P0_POL		4
#define TCA6416A_P1_POL		5
#define TCA6416A_P0_CFG		6
#define TCA6416A_P1_CFG		7
#define TCA6416A_REG_MASK	7

/* PIC function definition */
#define PIC_ADDR		(0x24 << 1)
#define PIC_REG_BOARD_TYPE	0
#define PIC_REG_SW_VERSION	1
#define PIC_REG_HW_VERSION	2
#define PIC_REG_CTRL		3
#define PIC_REG_SOC_TEMP	4

#define PIC_BOARD_TYPE_SE5	3
#define PIC_CMD_REBOOT		7

static int is_pic_available;

static struct {
	int set_idx;
	int idx;
} gpioex_ctx;

static inline int pic_write(uint8_t reg, uint8_t val)
{
	int err;

	err = HAL_I2C_Mem_Write(&hi2c1, PIC_ADDR, reg, 1, &val, 1, SMBTO);

	return err == HAL_OK ? 0 : -1;
}

static inline int pic_read(uint8_t reg)
{
	uint8_t tmp;
	int err;

	err = HAL_I2C_Mem_Read(&hi2c1, PIC_ADDR, reg, 1, &tmp, 1, SMBTO);

	if (err == HAL_OK)
		return tmp;
	else
		return -1;
}

int __attribute__((noinline)) tca6416a_write(uint8_t reg, uint8_t val)
{
    int err;

	err = HAL_I2C_Mem_Write(&hi2c1, TCA6416A_ADDR, reg, 1, &val, 1, SMBTO);
    return err;
}

static inline int tca6416a_read(uint8_t reg)
{
	uint8_t tmp;
    int err;

	err = HAL_I2C_Mem_Read(&hi2c1, TCA6416A_ADDR, reg, 1, &tmp, 1, SMBTO);
    if (err == HAL_OK)
        return tmp;
    else
        return -1;
}

static inline void tca6416a_set(uint8_t port, uint8_t pin)
{
	uint8_t reg;
	uint8_t val;

	reg = TCA6416A_P0_OUT + (port ? 1 : 0);
	val = tca6416a_read(reg);
	val |= (1 << pin);
	tca6416a_write(reg, val);
}

static inline void tca6416a_clr(uint8_t port, uint8_t pin)
{
	uint8_t reg;
	uint8_t val;

	reg = TCA6416A_P0_OUT + (port ? 1 : 0);
	val = tca6416a_read(reg);
	val &= ~(1 << pin);
	tca6416a_write(reg, val);
}

static void se5_gpioex_match(void *priv, int dir)
{
	if (dir == I2C_SLAVE_WRITE)
		gpioex_ctx.set_idx = 1;
}

static void se5_gpioex_write(void *priv, uint8_t data)
{
	if (gpioex_ctx.set_idx) {
		gpioex_ctx.idx = data & TCA6416A_REG_MASK;
		gpioex_ctx.set_idx = 0;
		return;
	}

	switch (gpioex_ctx.idx) {
	case TCA6416A_P0_IN:
	case TCA6416A_P1_IN:
	case TCA6416A_P0_POL:
	case TCA6416A_P1_POL:
	case TCA6416A_P0_CFG:
	case TCA6416A_P1_CFG:
		/* no effect and donot change pin config */
		return;
	case TCA6416A_P0_OUT:
	case TCA6416A_P1_OUT:
		/* output value is free for user */
		break;
	default:
		return;
	}

	tca6416a_write(gpioex_ctx.idx, data);

	gpioex_ctx.idx = (gpioex_ctx.idx + 1) & TCA6416A_REG_MASK;
}

static uint8_t se5_gpioex_read(void *priv)
{
	int err;

	err = tca6416a_read(gpioex_ctx.idx);
	gpioex_ctx.idx = (gpioex_ctx.idx + 1) % 8;
    return err < 0 ? 0 : err;
}

static struct i2c_slave_op slave = {
	.addr = 0x6C,
	.match	= se5_gpioex_match,
	.write	= se5_gpioex_write,
	.read	= se5_gpioex_read,
};

int se5_gpioex_init(void)
{
	tca6416a_write(TCA6416A_P0_OUT, P0_OUT);
	tca6416a_write(TCA6416A_P1_OUT, P1_OUT);
	tca6416a_write(TCA6416A_P0_CFG, P0_CFG);
	tca6416a_write(TCA6416A_P1_CFG, P1_CFG);
	tca6416a_write(TCA6416A_P0_POL, P0_POL);
	tca6416a_write(TCA6416A_P1_POL, P1_POL);

	is_pic_available = pic_read(PIC_REG_BOARD_TYPE) == PIC_BOARD_TYPE_SE5;

	i2c_slave_register(&i2c_ctx3,&slave);
	return 0;
}

void se5_reset_board(void)
{
    HAL_I2C_DeInit(&hi2c3);
    PowerDOWN();
    HAL_Delay(50);
    if (is_pic_available)
        pic_write(PIC_REG_CTRL, PIC_CMD_REBOOT);
    else
        tca6416a_clr(ALL_RESET_PORT, ALL_RESET_PIN);
    /* wait the end of the world */
    __disable_irq();
    while (1)
        ;
}

void se5_report_temp(int temp)
{
	if (is_pic_available)
		pic_write(PIC_REG_SOC_TEMP, temp);
}
