/* Sophgo Technology. Created by Donghao on 7/13/23. */


#include <ecdc.h>
#include <stdio.h>
#include <ctype.h>
#include <system/system.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <common.h>
#include <upgrade.h>
#include <timer.h>
#include <adc.h>
#include <pin.h>
#include <chip.h>
#include <power.h>
#include <mcu.h>
#include <slt.h>
#include <loop.h>
#include <signal.h>
#include <project.h>


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
//	debug("[%3lu] %-25s :  %-20s :  %4d\n", chip_uptime(), __FUNCTION__, __FILE__, __LINE__);
	printf("Hello! Athena2 USER @v@\n");
}

static const char * const cmd_poweron_usage =
"poweron\n"
"    power on the Athena2EVB\n";

static void cmd_poweron(void *hint, int argc, char const *argv[])
{
//	debug("[%3lu] %-25s :  %-20s :  %4d\n", chip_uptime(), __FUNCTION__, __FILE__, __LINE__);
	if (get_board_type() == ATHENA2EVB) {
		power_on();
		printf("Athena2EVB POWER ON\n");
	}
}

static const char * const cmd_poweroff_usage =
"poweroff\n"
"    power off the Athena2EVB\n";

static void cmd_poweroff(void *hint, int argc, char const *argv[])
{
//	debug("[%3lu] %-25s :  %-20s :  %4d\n", chip_uptime(), __FUNCTION__, __FILE__, __LINE__);
	if (get_board_type() == ATHENA2EVB) {
		power_off();
		timer_delay_ms(500);
		printf("Athena2EVB POWER OFF\n");
	}
}

static const char * const cmd_reboot_usage =
"reboot\n"
"    reboot the Athena2EVB\n";

static void cmd_reboot(void *hint, int argc, char const *argv[])
{
//	debug("[%3lu] %-25s :  %-20s :  %4d\n", chip_uptime(), __FUNCTION__, __FILE__, __LINE__);
	power_off();
	timer_delay_ms(500);
	power_on();
	chip_enable();
	if (get_board_type() == ATHENA2EVB) {
		printf("Athena2EVB REBOOT\n");
	}
}

static const char * const cmd_info_usage =
"info\n"
"    get information about board and mcu\n";

static void cmd_info(void *hint, int argc, char const *argv[])
{
//	debug("[%3lu] %-25s :  %-20s :  %4d\n", chip_uptime(), __FUNCTION__, __FILE__, __LINE__);
	printf("Chip type: Athena2\n");
	printf("PCB Version: %d\n", get_pcb_version());
    printf("BOM Version: %d\n", get_bom_version());
	printf("Board type: %s\n", get_board_type_name());
	printf("MCU_SW_VER: %d\n", MCU_SW_VER);
}

static const char * const cmd_temp_usage =
		"temp\n"
		"    temp soc&board\n";

static void cmd_temp(void *hint, int argc, char const *argv[])
{
//	debug("[%3lu] %-25s :  %-20s :  %4d\n", chip_uptime(), __FUNCTION__, __FILE__, __LINE__);

	if (get_board_type() == ATHENA2ACP) {
		int soc_temp_cmd, board_temp_ntc1_cmd, board_temp_ntc2_cmd;

		soc_temp_cmd = get_soc_temp();
		board_temp_ntc1_cmd = get_board_temp_ntc1();
		board_temp_ntc2_cmd = get_board_temp_ntc2();

		if (argc == 1) {
			printf("soc temp = %d(C)\tboard temp ntc1 = %d(C)\tboard temp ntc2 = %d(C)\n",
				   soc_temp_cmd, board_temp_ntc1_cmd, board_temp_ntc2_cmd);
		} else if (argc == 2) {
			if (strcmp(argv[1], "soc") == 0) {
				printf("soc temp = %d(C)\n", soc_temp_cmd);
			} else if (strcmp(argv[1], "board_ntc1") == 0) {
				printf("board temp ntc1 = %d(C)\n", board_temp_ntc1_cmd);
			} else if (strcmp(argv[1], "board_ntc2") == 0) {
				printf("board temp ntc2 = %d(C)\n", board_temp_ntc2_cmd);
			}
			else {
				printf("get %s temp failed\n", argv[1]);
			}
		} else {
			printf(cmd_temp_usage);
		}
	} else if (get_board_type() == ATHENA2EVB) {
		printf("ACP command only, current board: %s\n", get_board_type_name());
	} else {
		printf("Unknown board type: %d\n", get_board_type());
	}
}

static const char * const cmd_query_usage =
"query\n"
"    query slt reg result\n"
"    query (reg_num)\n";

static void cmd_query(void *hint, int argc, char const *argv[])
{
//	debug("[%3lu] %-25s :  %-20s :  %4d\n", chip_uptime(), __FUNCTION__, __FILE__, __LINE__);
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
		"    output current one time\n";

static void cmd_current(void *hint, int argc, char const *argv[])
{
//	debug("[%3lu] %-25s :  %-20s :  %4d\n", chip_uptime(), __FUNCTION__, __FILE__, __LINE__);
	if (get_board_type() == ATHENA2ACP) {
		current_print_func();
	}
	else if (get_board_type() == ATHENA2EVB) {
		printf("ACP command only, current board: %s\n", get_board_type_name());
	} else {
		printf("Unknown board type: %d\n", get_board_type());
	}
}

static const char * const cmd_status_usage =
		"status\n"
		"    output status\n";

static void cmd_status(void *hint, int argc, char const *argv[])
{
//	debug("[%3lu] %-25s :  %-20s :  %4d\n", chip_uptime(), __FUNCTION__, __FILE__, __LINE__);
	printf("core_power_status: %d\n", board_input_signal(a2_core_power_status));
	printf("mcu_boot1: %d\n", board_input_signal(a2_mcu_boot1));
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
	{"temp", NULL, cmd_temp_usage, cmd_temp},
	{"query", NULL, cmd_query_usage, cmd_query},
};

void print_usage(struct command *cmd)
{
//	debug("[%3lu] %-25s :  %-20s :  %4d\n", chip_uptime(), __FUNCTION__, __FILE__, __LINE__);
	if (cmd->usage)
		uart_puts(cmd->usage);
}

struct command *find_command(const char *name)
{
//	debug("[%3lu] %-25s :  %-20s :  %4d\n", chip_uptime(), __FUNCTION__, __FILE__, __LINE__);
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
//	debug("[%3lu] %-25s :  %-20s :  %4d\n", chip_uptime(), __FUNCTION__, __FILE__, __LINE__);
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
		printf("help [command]\n");
	}
}

int console_init(void)
{
//	debug("[%3lu] %-25s :  %-20s :  %4d\n", chip_uptime(), __FUNCTION__, __FILE__, __LINE__);
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
	/* Main loop query */
//	debug("[%3lu] %-25s :  %-20s :  %4d\n", chip_uptime(), __FUNCTION__, __FILE__, __LINE__);
	ecdc_pump_console(console);
}

void console_add(void)
{
//	debug("[%3lu] %-25s :  %-20s :  %4d\n", chip_uptime(), __FUNCTION__, __FILE__, __LINE__);
	console_init();
	loop_add(console_poll);
}

void console_test(void)
{
//	debug("[%3lu] %-25s :  %-20s :  %4d\n", chip_uptime(), __FUNCTION__, __FILE__, __LINE__);
	while (1)
		console_poll();
}
