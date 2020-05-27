#include <assert.h>
#include "stm32l0xx_hal.h"
#include "i2c.h"
#include "i2c_slave.h"
#include <keyboard.h>
#include <tca6416a.h>
#include <pic.h>

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

/* virtual key define */
#define POWER_KEY_PORT		0
#define POWER_KEY		(1 << 0)

#define REBOOT_KEY_PORT		0
#define REBOOT_KEY		(1 << 1)

#define FACTORY_RESET_KEY_PORT	0
#define FACTORY_RESET_KEY	(1 << 2)

static struct {
	int set_idx;
	int idx;
} gpioex_ctx;

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

#define SMB_ALERT_PORT	MCU_CPLD_ERR_GPIO_Port
#define SMB_ALERT_PIN	MCU_CPLD_ERR_Pin

static inline int is_smb_alert(void)
{
	return HAL_GPIO_ReadPin(SMB_ALERT_PORT, SMB_ALERT_PIN)
		== GPIO_PIN_RESET;
}

int se5_gpioex_init(void)
{
	/* reconfig cpld error pin from output to input */
	/* on sm5, this pin is defined as smbus alert */
	/* effect low */
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = SMB_ALERT_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(SMB_ALERT_PORT, &GPIO_InitStruct);


	tca6416a_write(TCA6416A_P0_OUT, P0_OUT);
	tca6416a_write(TCA6416A_P1_OUT, P1_OUT);
	tca6416a_write(TCA6416A_P0_CFG, P0_CFG);
	tca6416a_write(TCA6416A_P1_CFG, P1_CFG);
	tca6416a_write(TCA6416A_P0_POL, P0_POL);
	tca6416a_write(TCA6416A_P1_POL, P1_POL);

	i2c_slave_register(&i2c_ctx3,&slave);

	kbd_init();
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

static int is_heater_on;

static inline void heater_on(void)
{
	if (!is_heater_on)
		pic_write(PIC_REG_HEATER_CTRL, 1);
}

static inline void heater_off(void)
{
	if (is_heater_on)
		pic_write(PIC_REG_HEATER_CTRL, 0);
}

void se5_heater_ctrl(int temp)
{
	if (!is_pic_available)
		return;

	/* this function is called every 2 seconds
	 * so it is not called so frequently
	 */
	if (temp < 3)
		heater_on();
	else
		heater_off();
}

void se5_smb_alert(void)
{
	int err;

	if (!is_smb_alert())
		return;

	if (is_pic_available) {
		/* get request */
		err = pic_read(PIC_REG_REQUEST);
		switch (err) {
		case PIC_REQ_POWER_OFF:
			kbd_set(POWER_KEY_PORT, POWER_KEY);
			break;
		case PIC_REQ_REBOOT:
			kbd_set(REBOOT_KEY_PORT, REBOOT_KEY);
			break;
		case PIC_REQ_FACTORY_RESET:
			kbd_set(FACTORY_RESET_KEY_PORT, FACTORY_RESET_KEY);
			break;
		default:
			break;
		}
		if (err > 0) {
			pic_write(PIC_REG_REQUEST, err);
			return;
		}
	}

	/* process tca6416a */
	/* this should enough to clear 6416a interrupt */
	tca6416a_read(TCA6416A_P0_IN);
	tca6416a_read(TCA6416A_P1_IN);
}
