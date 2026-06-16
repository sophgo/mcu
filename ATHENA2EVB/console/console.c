/*
 * console.c:
 *
 * (C) Copyright 2023 Sophgo Technology
 * Author: Donghao Zhao <donghao.zhao@sophgo.com>
 */

#include <console/ecdc/ecdc.h>
#include <stdio.h>
#include <ctype.h>
#include <system/system.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <common/common.h>
#include <upgrade/upgrade.h>
#include <timer/timer.h>
#include <pin.h>
#include <chip/chip.h>
#include <power/power.h>
#include <i2c-slaves/mcu/mcu.h>
#include <i2c-slaves/slt/slt.h>
#include <loop/loop.h>
#include <gpio/gpio.h>
#include <gpio_ctrl/gpio_ctrl.h>
#include <power_seq/power_seq.h>
#include <project.h>
#include <strings.h>
#include <i2c-slaves/ds1307/ds1307.h>
#include <pmbus/pmbus.h>
#include <i2c/i2c_master/i2c_master.h>
#include <gd32e50x_i2c.h>

/* iictest -w 需 5 个参数，powerseq -w 需 7 个；ecdc 默认 4 不够 */
#define CONSOLE_MAX_ARGC	10

static struct ecdc_console *console;

static int console_getc(void *console_hint)
{
	int err;

	err = uart_getc();
	return err >= 0 ? err : ECDC_GETC_EOF;
}

static void console_putc(void *console_hint, char c)
{
	uart_putc(c);
}

static void cmd_hello(void *hint, int argc, char const *argv[])
{
	printf("Hello! Athena2 EVB User @v@\n");
}

static const char * const cmd_poweron_usage =
	"poweron\n"
	"    Power ON the Athena2EVB\n";

static void cmd_poweron(void *hint, int argc, char const *argv[])
{
	power_on();
	printf("Athena2EVB Power ON\n");
}

static const char * const cmd_poweroff_usage =
	"poweroff\n"
	"    Power OFF the Athena2EVB\n";

static void cmd_poweroff(void *hint, int argc, char const *argv[])
{
	power_off();
	timer_delay_ms(500);
	printf("Athena2EVB Power OFF\n");
}

static const char * const cmd_reboot_usage =
	"reboot\n"
	"    Reboot the Athena2EVB\n";

static void cmd_reboot(void *hint, int argc, char const *argv[])
{
	power_off();
	timer_delay_ms(500);
	power_on();
	printf("Athena2EVB Reboot\n");
}

static const char * const cmd_info_usage =
	"info\n"
	"    Get Information about the Board and MCU\n";

static void cmd_info(void *hint, int argc, char const *argv[])
{
	printf("Chip Type: Athena2\n");
	printf("Board Name: %s\n", get_board_type_name());
	printf("DDR Type: %s\n", get_ddr_type_name());
	printf("Board Type: %#x\n", get_board_type());
	printf("MCU Firmware: %d\n", get_firmware_version());
	printf("PCB Version: %d\n", get_pcb_version());
	printf("BOM Version: %d\n", get_bom_version());
}

static const char * const cmd_query_usage =
	"query\n"
	"    Query SLT Register Result\n"
	"    Query (reg_num)\n";

static void cmd_query(void *hint, int argc, char const *argv[])
{
	int reg = 0;
	if (argc == 1){
		for (reg = 0; reg < 6; reg += 2){
			uint16_t result = get_slt_result(reg);
			printf("reg%d&reg%d = 0x%04x\n", (reg + 1), reg, result);
		}
	} else if (argc == 2){
		reg = atoi(argv[1]);
		if (reg >= 64 || reg < 0){
			printf("reg%d inexist\n",reg);
		} else {
			uint16_t result = get_slt_result(reg);
			printf("reg%d&reg%d = 0x%04x\n", (reg + 1), reg, result);
		}
	}
	else
		printf(cmd_query_usage);
}

static const char * const cmd_status_usage =
	"status\n"
	"    Output Status of the Board\n";

static void cmd_status(void *hint, int argc, char const *argv[])
{
	printf("core_power_status: %d\n", gpio_input(core_power_status_signal));
}

static const char * const cmd_time_usage =
	"time\n"
	"    Output time get from RTC\n";

static void cmd_time(void *hint, int argc, char const *argv[])
{
	ds1307_get_time();
	printf("Time: %s\n", time);
}

static const char * const cmd_pintest_usage =
	"pintest -w <func_name> <0|1>  set GPIO high(1) or low(0)\n"
	"pintest -r <func_name>        read GPIO (OCTL + pin level)\n"
	"pintest -l                    list all test GPIOs and state\n"
	"  func_name: PCIE_SEL EN_12V0 POWEREN1 POWEREN2 POWEREN3\n"
	"             SYS_RSTN_H PWR_RSTN_H PWR_BUTTON1_H PWR_ON_H PWR_WAKEUP_H\n";

static void pintest_print_state(struct gpio_test_node *node)
{
	bool octl = gpio_ctrl_read_output(node);
	bool pin = gpio_ctrl_read_pin(node);

	printf("  %-14s OCTL=%d ISTAT=%d (%s)\n",
	       node->name, octl ? 1 : 0, pin ? 1 : 0,
	       pin ? "HIGH" : "LOW");
}

static void cmd_pintest(void *hint, int argc, char const *argv[])
{
	struct gpio_test_node *node;

	if (argc < 2) {
		printf("%s", cmd_pintest_usage);
		return;
	}

	if (strcmp(argv[1], "-l") == 0) {
		static struct gpio_test_node *nodes[] = {
			&pcie_sel, &en_12v0, &poweren1, &poweren2, &poweren3,
			&sys_rstn_h, &pwr_rstn_h, &pwr_button1_h,
			&pwr_on_h, &pwr_wakeup_h,
		};
		int i;

		printf("GPIO test nodes:\n");
		for (i = 0; i < ARRAY_SIZE(nodes); ++i)
			pintest_print_state(nodes[i]);
		return;
	}

	if (argc != 4 && !(argc == 3 && strcmp(argv[1], "-r") == 0)) {
		printf("%s", cmd_pintest_usage);
		return;
	}

	node = gpio_ctrl_find(argv[2]);
	if (!node) {
		printf("Unknown pin: %s\n", argv[2]);
		return;
	}

	if (strcmp(argv[1], "-w") == 0) {
		if (atoi(argv[3]))
			gpio_ctrl_set(node);
		else
			gpio_ctrl_clear(node);
		printf("%s = %s\n", node->name,
		       atoi(argv[3]) ? "HIGH" : "LOW");
		pintest_print_state(node);
	} else if (strcmp(argv[1], "-r") == 0) {
		pintest_print_state(node);
	} else {
		printf("%s", cmd_pintest_usage);
	}
}

static const char * const cmd_powerseq_usage =
	"powerseq -u                     step UP, gap(us) before each pin\n"
	"powerseq -d                     all seq pins DOWN immediately\n"
	"powerseq -w d1 d2 d3 d4 d5      set 5 gaps (us), then run -u\n";

static void powerseq_print_gaps(void)
{
	unsigned int gaps[POWER_SEQ_GAP_NUM];
	int i;

	power_seq_get_gaps_us(gaps);
	printf("gaps (us):");
	for (i = 0; i < POWER_SEQ_GAP_NUM; ++i)
		printf(" %u", gaps[i]);
	printf("\n");
}

static void cmd_powerseq(void *hint, int argc, char const *argv[])
{
	unsigned int gaps[POWER_SEQ_GAP_NUM];
	int i;

	if (argc < 2) {
		printf("%s", cmd_powerseq_usage);
		powerseq_print_gaps();
		return;
	}

	if (strcmp(argv[1], "-d") == 0) {
		power_seq_off();
		printf("Power DOWN done (6 seq pins)\n");
		printf("  EN_12V0 POWEREN1~3 SYS_RSTN_H PWR_RSTN_H -> LOW\n");
		printf("  use poweroff for full board shutdown\n");
	} else if (strcmp(argv[1], "-u") == 0) {
		power_seq_on();
		printf("Power UP done\n");
		powerseq_print_gaps();
	} else if (strcmp(argv[1], "-w") == 0) {
		if (argc != 7) {
			printf("Usage: powerseq -w d1 d2 d3 d4 d5  (us)\n");
			return;
		}
		for (i = 0; i < POWER_SEQ_GAP_NUM; ++i)
			gaps[i] = (unsigned int)strtoul(argv[2 + i], NULL, 0);
		power_seq_set_gaps_us(gaps);
		printf("Gaps set (us), run powerseq -u to apply\n");
		powerseq_print_gaps();
	} else {
		printf("%s", cmd_powerseq_usage);
	}
}

static const char * const cmd_iictest_usage =
	"iictest -f                     scan I2C0 bus for devices\n"
	"iictest -w <addr> <reg> <val>  write byte to device register\n"
	"iictest -r <addr> <reg>        read byte from device register\n"
	"iictest -d <addr>              dump device registers 0x00-0xFF\n";

static void cmd_iictest(void *hint, int argc, char const *argv[])
{
	uint8_t addr, reg, val;
	int rv;

	if (argc < 2) {
		printf("%s", cmd_iictest_usage);
		return;
	}

	if (strcmp(argv[1], "-f") == 0) {
		printf("I2C0 bus scan:\n");
		for (addr = 0x08; addr < 0x78; addr += 2) {
			rv = i2c_master_write_byte(I2C0, addr, 5, 0);
			if (rv == 0)
				printf("  found device at 0x%02X\n", addr);
		}
		printf("scan done.\n");
	} else if (strcmp(argv[1], "-w") == 0) {
		if (argc != 5) {
			printf("Usage: iictest -w <addr> <reg> <val>\n");
			return;
		}
		addr = (uint8_t)strtol(argv[2], NULL, 0);
		reg  = (uint8_t)strtol(argv[3], NULL, 0);
		val  = (uint8_t)strtol(argv[4], NULL, 0);
		rv = i2c_master_smbus_write_byte(I2C0, addr, 5, reg, val);
		if (rv == 0)
			printf("I2C[0x%02X] reg 0x%02X <- 0x%02X\n", addr, reg, val);
		else
			printf("I2C write failed\n");
	} else if (strcmp(argv[1], "-r") == 0) {
		if (argc != 4) {
			printf("Usage: iictest -r <addr> <reg>\n");
			return;
		}
		addr = (uint8_t)strtol(argv[2], NULL, 0);
		reg  = (uint8_t)strtol(argv[3], NULL, 0);
		rv = i2c_master_smbus_read_byte(I2C0, addr, 5, reg, &val);
		if (rv == 0)
			printf("I2C[0x%02X] reg 0x%02X = 0x%02X (%u)\n",
			       addr, reg, val, val);
		else
			printf("I2C read failed\n");
	} else if (strcmp(argv[1], "-d") == 0) {
		if (argc != 3) {
			printf("Usage: iictest -d <addr>\n");
			return;
		}
		addr = (uint8_t)strtol(argv[2], NULL, 0);
		printf("Dump I2C[0x%02X]:\n", addr);
		/* reg 不可用 uint8_t 做 0..255 循环，++ 后会回绕死循环 */
		for (unsigned int i = 0; i < 256; ++i) {
			reg = (uint8_t)i;
			rv = i2c_master_smbus_read_byte(I2C0, addr, 5, reg, &val);
			if (rv == 0 && val != 0xFF)
				printf("  [0x%02X] = 0x%02X\n", reg, val);
		}
		printf("dump done.\n");
	} else {
		printf("%s", cmd_iictest_usage);
	}
}

static const char * const cmd_pmbus_usage =
	"pmbus -r         read output voltage\n"
	"pmbus -w <val>   set output voltage (mV)\n";

static void cmd_pmbus(void *hint, int argc, char const *argv[])
{
	unsigned int mv;
	uint16_t raw;
	uint8_t mode;

	if (argc < 2) {
		printf("%s", cmd_pmbus_usage);
		return;
	}

	if (strcmp(argv[1], "-r") == 0) {
		if (pmbus_get_voltage_mv(&mv) == 0) {
			printf("VOUT = %u mV\n", mv);
			if (pmbus_read_vout_raw(&raw) == 0)
				printf("  raw=0x%04x addr=0x%02x\n",
				       raw, pmbus_get_addr());
		} else {
			printf("PMBus read failed (addr=0x%02x)\n",
			       pmbus_get_addr());
			if (i2c_master_smbus_read_byte(I2C0, pmbus_get_addr(),
						       50, PMBUS_VOUT_MODE,
						       &mode) == 0)
				printf("  VOUT_MODE=0x%02x (I2C OK)\n", mode);
			else
				printf("  VOUT_MODE read failed (check I2C0/addr)\n");
		}
	} else if (strcmp(argv[1], "-w") == 0) {
		if (argc != 3) {
			printf("Usage: pmbus -w <mV>\n");
			return;
		}
		mv = (unsigned int)atoi(argv[2]);
		pmbus_set_voltage(mv);
		printf("VOUT set to %u mV\n", mv);
	} else {
		printf("%s", cmd_pmbus_usage);
	}
}

struct command {
	const char *name, *alias, *usage;
	ecdc_callback_fn fn;
};

static void cmd_help(void *hint, int argc, char const *argv[]);

static struct command command_list[] = {
	{"help", NULL, NULL, cmd_help},
	{"hello", NULL, NULL, cmd_hello},
	{"poweron", NULL, cmd_poweron_usage, cmd_poweron},
	{"poweroff", NULL, cmd_poweroff_usage, cmd_poweroff},
	{"reboot", NULL, cmd_reboot_usage, cmd_reboot},
	{"info", NULL, cmd_info_usage, cmd_info},
	{"status", NULL, cmd_status_usage, cmd_status},
	{"time", NULL, cmd_time_usage, cmd_time},
	{"query", NULL, cmd_query_usage, cmd_query},
	{"pintest", NULL, cmd_pintest_usage, cmd_pintest},
	{"powerseq", NULL, cmd_powerseq_usage, cmd_powerseq},
	{"iictest", NULL, cmd_iictest_usage, cmd_iictest},
	{"pmbus", NULL, cmd_pmbus_usage, cmd_pmbus},
};

void print_usage(struct command *cmd)
{
	if (cmd->usage)
		uart_puts(cmd->usage);
}

struct command *find_command(const char *name)
{
	int i;
	struct command *cmd;

	for (i = 0; i < ARRAY_SIZE(command_list); ++i) {
		cmd = &command_list[i];
		if (strcmp(cmd->name, name) == 0)
			return cmd;
	}

	return NULL;
}

static void cmd_help(void *hint, int argc, char const *argv[])
{
	int i;
	struct command *cmd;

	if (argc == 1) {
		for (i = 0; i < ARRAY_SIZE(command_list); ++i)
			print_usage(&command_list[i]);
	} else if (argc == 2) {
		cmd = find_command(argv[1]);
		if (cmd)
			print_usage(cmd);
		else
			printf("\'%s\' Not Found\n", argv[1]);
	} else {
		printf("Invalid Usage\n");
		printf("help [command]\n");
	}
}

int console_init(void)
{
	int i;

	console = ecdc_alloc_console(NULL, console_getc, console_putc, 128,
				     CONSOLE_MAX_ARGC);
	if (console == NULL) {
		printf("Create Console Failed\n");
		return -1;
	}
	ecdc_configure_console(console, ECDC_MODE_ANSI, ECDC_SET_LOCAL_ECHO);

	for (i = 0; i < ARRAY_SIZE(command_list); ++i) {
		if (command_list[i].name)
			ecdc_alloc_command(NULL, console,
					   command_list[i].name,
					   command_list[i].fn);
		if (command_list[i].alias)
			ecdc_alloc_command(NULL, console,
					   command_list[i].alias,
					   command_list[i].fn);
	}

	ecdc_alloc_list_command(console, "list");

	console_putc(NULL, '\r');
	console_putc(NULL, '\n');

	return 0;
}

void console_poll(void)
{
	/* Main loop query */
	ecdc_pump_console(console);
}

void console_add(void)
{
	console_init();
	loop_add(console_poll);
}

void console_test(void)
{
	while (1)
		console_poll();
}
