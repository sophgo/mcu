#include <pin.h>
#include <pic.h>
#include <tca6416a.h>
#include <power.h>
#include <keyboard.h>
#include <tick.h>
#include <loop.h>
#include <common.h>

/* virtual key define */
#define POWER_KEY_PORT		0
#define POWER_KEY		(1 << 0)

#define REBOOT_KEY_PORT		0
#define REBOOT_KEY		(1 << 1)

#define FACTORY_RESET_KEY_PORT	0
#define FACTORY_RESET_KEY	(1 << 2)

#define SMB_ALERT_PORT	MCU_INT_PORT
#define SMB_ALERT_PIN	MCU_INT_PIN

/* gpio expandtion(tca6416a) config */
/* port 0 */
#define ALL_RESET_PORT      0
#define ALL_RESET_PIN       0
#define ALL_RESET_DEFAULT   1

#define PCB_VERSION0_PORT    0
#define PCB_VERSION0_PIN     1

#define PCB_VERSION1_PORT    0
#define PCB_VERSION1_PIN     2

#define LED0_PORT       0
#define LED0_PIN        3
#define LED0_DEFAULT    0

#define LED1_PORT       0
#define LED1_PIN        4
#define LED1_DEFAULT    0

#define SATA_INT_PORT   0
#define SATA_INT_PIN    5

#define CLK_EN_PORT     0
#define CLK_EN_PIN      6
#define CLK_EN_DEFAULT  0

#define LTE_RST_PORT    0
#define LTE_RST_PIN     7
#define LTE_RST_DEFAULT 0

#define P0_CFG		((1 << PCB_VERSION0_PIN) | \
			 (1 << PCB_VERSION1_PIN) | \
			 (1 << SATA_INT_PIN))

#define P0_OUT		((ALL_RESET_DEFAULT << ALL_RESET_PIN) | \
			 (LED0_DEFAULT << LED0_PIN) | \
			 (LED1_DEFAULT << LED1_PIN) | \
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

#define P1_OUT			((1 << LTE_PWR_DEFAULT) | \
				 (1 << SATA_EN_DEFAULT) | \
				 (1 << HEALTHY_LED_DEFAULT) | \
				 (1 << ETH_CTL_DEFAULT) | \
				 (1 << FL2K_RST_DEFAULT) | \
				 (1 << RST_KEY_RST_DEFAULT) | \
				 (1 << ASM3142_POR_DEFAULT))

#define P1_CFG		(1 << PCIE_INT_PIN)
#define P1_POL		0


static inline int is_smb_alert(void)
{
	return gpio_get(SMB_ALERT_PORT, SMB_ALERT_PIN) == 0;
}

void se5_reset_board(void)
{
	nvic_disable_irq(MCU_SOC_I2C_IRQ);
	power_off();
	if (pic_available())
		pic_write(PIC_REG_CTRL, PIC_CMD_REBOOT);
	else
		tca6416a_clr(ALL_RESET_PORT, ALL_RESET_PIN);
	/* wait the end of the world */
	while (1)
		;
}

void se5_power_off_board(void)
{
	if (!pic_available())
		return;

	nvic_disable_irq(MCU_SOC_I2C_IRQ);
	power_off();
	pic_write(PIC_REG_CTRL, PIC_CMD_POWER_OFF);
	/* wait the end of the world */
	while (1)
		;
}

static int is_heater_on;

static inline void heater_on(void)
{
	if (!is_heater_on) {
		is_heater_on = 1;
		pic_write(PIC_REG_HEATER_CTRL, 1);
	}
}

static inline void heater_off(void)
{
	if (is_heater_on) {
		is_heater_on = 0;
		pic_write(PIC_REG_HEATER_CTRL, 0);
	}
}

void se5_heater_ctrl(int temp)
{
	if (!pic_available())
		return;

	/* this function is called every 2 seconds
	 * so it is not called so frequently
	 */
	if (temp < 3)
		heater_on();
	else
		heater_off();
}

static void se5_smb_alert(void)
{
	int err;

	if (!is_smb_alert())
		return;

	if (pic_available()) {
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

void se5_error_led_on(void)
{
	tca6416a_set(LED1_PORT, LED1_PIN);
}

void se5_error_led_off(void)
{
	tca6416a_clr(LED1_PORT, LED1_PIN);
}

static inline void se5_error_led_flicker(void)
{
	if (tick_get() & (1 << 10))
		se5_error_led_on();
	else
		se5_error_led_off();
}

void se5_led_ctrl(void)
{
	return;
}

static void se5_process(void)
{
	se5_smb_alert();
}

void se5_init(void)
{
	/* reconfig PA6 from cpld err to smbus alert */
	gpio_init(SMB_ALERT_PORT, GPIO_MODE_IPU,
		  GPIO_OSPEED_2MHZ, SMB_ALERT_PIN);

	tca6416a_write(TCA6416A_P0_OUT, P0_OUT);
	tca6416a_write(TCA6416A_P1_OUT, P1_OUT);
	tca6416a_write(TCA6416A_P0_CFG, P0_CFG);
	tca6416a_write(TCA6416A_P1_CFG, P1_CFG);
	tca6416a_write(TCA6416A_P0_POL, P0_POL);
	tca6416a_write(TCA6416A_P1_POL, P1_POL);
	loop_add(se5_process);
}

