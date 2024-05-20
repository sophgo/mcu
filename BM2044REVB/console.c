#include <ecdc/ecdc.h>
#include <stdio.h>
#include <ctype.h>
#include <system.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <common.h>
#include <upgrade.h>
#include <timer.h>
#include <pin.h>
#include <power.h>
#include <mcu.h>
#include <chip.h>
#include <slt.h>
#include <i2c_master.h>

static struct ecdc_console *console;
extern int power_is_on;
extern int is_print_enabled;

uint32_t sys_rst_pin_list[1][2] = {
	{CPU_SYS_RST_H_PORT, CPU_SYS_RST_H_PIN},
};

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
	printf("Hello! BM2044R EVB USER 0v0\n");
}

static const char * const cmd_poweron_usage =
"poweron\n"
"    power on the bm2044revb\n";

static void cmd_poweron(void *hint, int argc, char const *argv[])
{

	gpio_set(GPIOC, GPIO_PIN_12);

	if(power_is_on == true)
		chip_enable();

	printf("BM2044REVB OK\n");
}

static const char * const cmd_poweroff_usage =
"poweroff\n"
"    power off the bm2044revb\n";

static void cmd_poweroff(void *hint, int argc, char const *argv[])
{
	gpio_clear(GPIOC, GPIO_PIN_12);

	if(power_is_on == false)
		chip_disable();

	timer_mdelay(500);

	printf("BM2044REVB OFF\n");
}

static const char * const cmd_getmcutype_usage =
"getmcutype\n"
"    get mcu's type\n";

static void cmd_getmcutype(void *hint, int argc, char const *argv[])
{
	printf("BM2044REVB\n");
}

static const char * const cmd_current_usage =
"current\n"
"    output current one time\n";

static void cmd_current(void *hint, int argc, char const *argv[])
{
	current_print_func();
}

static const char * const cmd_temp_usage =
"temp\n"
"    temp soc&board\n";

static void cmd_temp(void *hint, int argc, char const *argv[])
{
	int boardtemp, soctemp;

	boardtemp = get_board_temp();
	soctemp = get_soc_temp();
	if (argc == 1) {
		printf("soc temp = %d(C)\tboard temp = %d(C)\n", soctemp, boardtemp);
	} else if (argc == 2) {
		if (strcmp(argv[1], "soc") == 0){
			printf("soc temp = %d(C)\n", soctemp);
		} else if (strcmp(argv[1], "board") == 0){
			printf("board temp = %d(C)\n", boardtemp);
		}
		else
			printf("get %s temp failed\n", argv[1]);
	} else {
		printf(cmd_temp_usage);
	}
}

static const char * const cmd_query_usage =
"query\n"
"    query slt reg result\n"
"    query (reg_num)\n";

static void cmd_query(void *hint, int argc, char const *argv[])
{
	int reg = 0;
	if (argc == 1) {
		for (reg = 0; reg < 6; reg += 2){
			uint16_t result = get_slt_result(reg);
			printf("reg%d&reg%d = 0x%04x\n", (reg + 1), reg, result);
		}
	} else if (argc == 2) {
		reg = atoi(argv[1]);
		if (reg >= 64 || reg < 0) {
			printf("reg%d inexist\n",reg);
		} else {
			uint16_t result = get_slt_result(reg);
			printf("reg%d&reg%d = 0x%04x\n", (reg + 1), reg, result);
		}
	}
	else
		printf(cmd_query_usage);
}

static const char * const cmd_i2cget_usage =
"i2cget\n"
"    I2C2: i2cget <device_addr> <reg_addr> <data_num:1/2>\n";

static void cmd_i2cget(void *hint, int argc, char const *argv[])
{
	uint8_t device_addr;
	uint8_t reg_addr;
	int ret;
	uint8_t tmp1;
	uint16_t tmp2;

	tmp1 = tmp2 = 0;

	if (argc == 4) {
		device_addr = atoi(argv[1]);
		reg_addr = atoi(argv[2]);
		printf("addr = 0x%02x 0x%02x\n", device_addr, reg_addr);
		if (strcmp(argv[3], "1") == 0) {
			ret = i2c2_master_smbus_read_byte(I2C2, device_addr, 1, (unsigned char)reg_addr, &tmp1);
			if (ret != 0)
				return;
			printf("info = 0x%02x\n", tmp1);
		} else {
			ret = i2c2_master_smbus_read_word(I2C2, device_addr, 1, (unsigned char)reg_addr, &tmp2);
			if (ret != 0)
				return;
			printf("info = 0x%04x\n", tmp2);
		}
	}else {
		printf(cmd_i2cget_usage);
	}
}

static const char * const cmd_i2cset_usage =
"i2cset\n"
"    I2C2: i2cset <device_addr> <reg_addr> <1 byte data>\n";

static void cmd_i2cset(void *hint, int argc, char const *argv[])
{
	uint8_t device_addr;
	uint8_t reg_addr;
	uint8_t tmp1;
	int ret;

	tmp1 = 0;

	if (argc == 4) {
		device_addr = atoi(argv[1]);
		reg_addr = atoi(argv[2]);
		tmp1 = atoi(argv[3]);
		ret = i2c2_master_smbus_write_byte(I2C2, device_addr, 1, (unsigned char)reg_addr, tmp1);
		if (ret != 0)
			return;
		printf("addr = 0x%02x 0x%02x\n", device_addr, reg_addr);
	} else
		printf(cmd_i2cget_usage);
}

static const char * const cmd_enprint_usage =
"enprint\n"
"    enprint 0/1; 1:output current every second\n";

static void cmd_enprint(void *hint, int argc, char const *argv[])
{
	if (argc == 1) {
		is_print_enabled = 1;
	}else if (argc == 2) {
		if (strcmp(argv[1], "1") == 0) {
			is_print_enabled = 1;
		} else if (strcmp(argv[1], "0") == 0) {
			is_print_enabled = 0;
		}
		else
			printf("set enprint 0&1\n");
	}else {
		printf(cmd_enprint_usage);
	}
}

static const char * const cmd_sysrst_usage =
"sysrst\n"
"    sysrst 0/1\n"
"    1: pull up sysrst pin\n"
"    0: pull down sysrst pin\n";

static void cmd_sysrst(void *hint, int argc, char const *argv[])
{
	if (argc != 2)
		printf(cmd_sysrst_usage);
	else {
		if (strcmp(argv[1], "0") == 0) {
			gpio_clear(sys_rst_pin_list[0][0], sys_rst_pin_list[0][1]);
			printf("chip down\n");
		} else if (strcmp(argv[1], "1") == 0) {
			gpio_set(sys_rst_pin_list[0][0], sys_rst_pin_list[0][1]);
			printf("chip up\n");
		} else
			printf("sysret 0&1\n");
	}
}

static const char * const cmd_upgrade_usage =
"upgrade\n"
"    enter uart upgrade mode\n";

static void cmd_upgrade(void *hint, int argc, char const *argv[])
{
	if (argc != 1) {
		printf("invalid usage\n");
		return;
	}

	printf("entering uart upgrade mode\n");
	uart_upgrade_start();
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
	{"getmcutype", NULL, cmd_getmcutype_usage, cmd_getmcutype},
	{"temp", NULL, cmd_temp_usage, cmd_temp},
	{"query", NULL, cmd_query_usage, cmd_query},
	{"i2cget", NULL, cmd_i2cget_usage, cmd_i2cget},
	{"i2cset", NULL, cmd_i2cset_usage, cmd_i2cset},
	{"enprint", NULL, cmd_enprint_usage, cmd_enprint},
	{"current", NULL, cmd_current_usage, cmd_current},
	{"sysrst", NULL, cmd_sysrst_usage, cmd_sysrst},
	{"upgrade", NULL, cmd_upgrade_usage, cmd_upgrade},
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
			printf("\'%s\' not found\n", argv[1]);
	} else {
		printf("invalid usage\n");
		printf("help [command]");
	}
}

int console_init(void)
{
	int i;

	console = ecdc_alloc_console(NULL, console_getc, console_putc, 128, 4);
	if (console == NULL) {
		printf("create console failed\n");
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
	ecdc_pump_console(console);
}

void console_test(void)
{
	while (1)
		console_poll();
}
