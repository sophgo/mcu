/*
 * ecdc.c:
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
#include <adc/adc.h>
#include <pin.h>
#include <chip/chip.h>
#include <power/power.h>
#include <i2c-slaves/mcu/mcu.h>
#include <i2c-slaves/slt/slt.h>
#include <loop/loop.h>
#include <gpio/gpio.h>
#include <project.h>

static struct ecdc_console *console;
extern int is_print_enabled;

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
	printf("Hello! Athena2 ACP User @v@\n");
}

static const char * const cmd_poweron_usage =
	"poweron\n"
	"    Power ON the Athena2 ACP\n";

static void cmd_poweron(void *hint, int argc, char const *argv[])
{
	power_on();
	printf("Athena2 ACP Power ON\n");
}

static const char * const cmd_poweroff_usage =
	"poweroff\n"
	"    Power OFF the Athena2 ACP\n";

static void cmd_poweroff(void *hint, int argc, char const *argv[])
{
	power_off();
	timer_delay_ms(500);
	printf("Athena2 ACP Power OFF\n");
}

static const char * const cmd_reboot_usage =
	"reboot\n"
	"    Reboot the Athena2 ACP\n";

static void cmd_reboot(void *hint, int argc, char const *argv[])
{
	power_off();
	timer_delay_ms(500);
	power_on();
	chip_enable();
	printf("Athena2 ACP REBOOT\n");
}

static const char * const cmd_info_usage =
	"info\n"
	"    Get Information about the Board and MCU\n";

static void cmd_info(void *hint, int argc, char const *argv[])
{
	printf("Chip Type: Athena2\n");
	printf("PCB Version: %d\n", get_pcb_version());
	printf("BOM Version: %d\n", get_bom_version());
	printf("Board Type: %s\n", get_board_type_name());
	printf("MCU_SW_VER: %d\n", MCU_SW_VER);
}

static const char * const cmd_temp_usage =
	"temp\n"
	"    Temperature of SoC & Board\n";

static void cmd_temp(void *hint, int argc, char const *argv[])
{
	if (argc == 1) {
		printf("SoC: %d(C)\nBoard: %d(C)\nNTC1: %d(C)\nNTC2 ="
		       " %d(C)\n", get_soc_temp(), get_board_temp(),
		       get_board_temp_ntc1(), get_board_temp_ntc2());
	} else if (argc == 2) {
		if (strcmp(argv[1], "soc") == 0) {
			printf("SoC: %d(C)\n", get_soc_temp());
		} else if (strcmp(argv[1], "board") == 0) {
			printf("Board: %d(C)\n", get_board_temp());
		} else if (strcmp(argv[1], "board_ntc1") == 0) {
			printf("Board NTC1: %d(C)\n",
			       get_board_temp_ntc1());
		} else if (strcmp(argv[1], "board_ntc2") == 0) {
			printf("Board NTC2: %d(C)\n",
			       get_board_temp_ntc2());
		}
		else {
			printf("Get %s Temp Failed\n", argv[1]);
		}
	} else {
		printf(cmd_temp_usage);
	}
}

static const char * const cmd_query_usage =
	"query\n"
	"    Query SLT Reg Result\n"
	"    Query (reg_num)\n";

static void cmd_query(void *hint, int argc, char const *argv[])
{
	int reg = 0;
	if (argc == 1){
		for (reg = 0; reg < 6; reg += 2){
			uint16_t result = get_slt_result(reg);
			printf("reg%d&reg%d = 0x%04x\n", (reg + 1), reg, result);
		}
	}else if (argc == 2){
		reg = atoi(argv[1]);
		if (reg >= 64 || reg < 0){
			printf("reg%d inexist\n",reg);
		}else {
			uint16_t result = get_slt_result(reg);
			printf("reg%d&reg%d = 0x%04x\n", (reg + 1), reg, result);
		}
	}
	else
		printf(cmd_query_usage);
}

static const char * const cmd_current_usage =
	"current\n"
	"    Output Current Once\n";

static void cmd_current(void *hint, int argc, char const *argv[])
{
	current_print();
}

static const char * const cmd_status_usage =
	"status\n"
	"    Output Status\n";

static void cmd_status(void *hint, int argc, char const *argv[])
{
	printf("Core Power Status: %d\n", gpio_input(core_power_status_signal));
}

static const char * const cmd_adc_usage =
	"adc\n"
	"    Output ADC Original Value\n";

static void cmd_adc(void *hint, int argc, char const *argv[])
{
	adc_print();
}

static const char * const cmd_enprint_usage =
	"enprint\n"
	"    enprint 0/1; 1: Output Current Every Second\n";

static void cmd_enprint(void *hint, int argc, char const *argv[])
{
	if (argc == 1) {
		is_print_enabled = 1;
	} else if (argc == 2) {
		if (strcmp(argv[1], "1") == 0) {
			is_print_enabled = 1;
		} else if (strcmp(argv[1], "0") == 0) {
			is_print_enabled = 0;
		}
		else
			printf("Set enprint 0&1\n");
	} else {
		printf(cmd_enprint_usage);
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
	{"current", NULL, cmd_current_usage, cmd_current},
	{"status", NULL, cmd_status_usage, cmd_status},
	{"adc", NULL, cmd_adc_usage, cmd_adc},
	{"temp", NULL, cmd_temp_usage, cmd_temp},
	{"query", NULL, cmd_query_usage, cmd_query},
	{"enprint", NULL, cmd_enprint_usage, cmd_enprint},
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
		printf("Help [Command]\n");
	}
}

int console_init(void)
{
	int i;

	console = ecdc_alloc_console(NULL, console_getc, console_putc, 128, 4);
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
