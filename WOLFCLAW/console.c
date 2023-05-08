#include <ecdc/ecdc.h>
#include <stdio.h>
#include <ctype.h>
#include <system.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <common.h>
#include <timer.h>
#include <pin.h>
#include <isl68224.h>

static struct ecdc_console *console;

static struct I2C_DEVICE
{
	int num;
	char* name;
} slaves_info[] = {
	{0, "ISL68224"},
	{1, "ISL68127"},
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
	printf("Hello! Wolfclaw User!\n");
}

static const char * const cmd_mcu_info_usage =
"mcuinfo\n"
"    get mcu's info\n";

static void cmd_mcu_info(void *hint, int argc, char const *argv[])
{
	printf("Board type: WOLFCALW\n");
	printf("MCU_SW_VERSION: %d\n", MCU_SW_VER);
	printf("Default i2c device name: %s\n", slaves_info[get_default_device()].name);
	printf("Default i2c device address: 0x%02x\n", get_default_addr());
	printf("Default filename: %s\n", get_default_filename());
}

static const char * const cmd_device_info_usage =
"deviceinfo\n"
"    get device's info\n";

static void cmd_device_info(void *hint, int argc, char const *argv[])
{
	isl68224_get_nvm_slot_num();
	isl68224_get_device_id();
	isl68224_get_reversion_id();
}

static const char * const cmd_led_usage =
"led\n"
"    led [led_num] [led_status]\n"
"    control led on/off\n";

static void cmd_led(void *hint, int argc, char const *argv[])
{
	int led_num;

	if (argc != 3){
		printf("%s", cmd_led_usage);
		return;
	}

	led_num = strtol(argv[1], NULL, 0);
	switch (led_num)
	{
	case 1:
		if(strcmp("on", argv[2]) == 0){
			led_1_0_on();
			printf("turn on led 1\n");
		}else if(strcmp("off", argv[2]) == 0){
			led_1_1_on();
			printf("turn off led 1\n");
		}else
			printf("error input status\n");
		break;
	case 2:
		if(strcmp("on", argv[2]) == 0){
			led_2_0_on();
			printf("turn on led 2\n");
		}else if(strcmp("off", argv[2]) == 0){
			led_2_1_on();
			printf("turn off led 2\n");
		}else
			printf("error input status\n");
		break;
	case 3:
		if(strcmp("on", argv[2]) == 0){
			led_3_0_on();
			printf("turn on led 3\n");
		}else if(strcmp("off", argv[2]) == 0){
			led_3_1_on();
			printf("turn off led 3\n");
		}else
			printf("error input status\n");
		break;

	case 4:
		if(strcmp("on", argv[2]) == 0){
			led_4_0_on();
			printf("turn on led 4\n");
		}else if(strcmp("off", argv[2]) == 0){
			led_4_1_on();
			printf("turn off led 4\n");
		}else
			printf("error input status\n");
		break;
	default:
		break;
	}
}

static const char * const cmd_default_addr_usage =
"addr\n"
"    addr <the address>\n"
"    set the address as default address\n";

static void cmd_default_addr(void *hint, int argc, char const *argv[])
{
	uint8_t addr;

	if (argc != 2){
		printf("%s", cmd_default_addr_usage);
		return;
	}
	addr = strtol(argv[1], NULL, 0);
	set_default_addr(addr);
	printf("default addr = 0x%02x\n", addr);
}

static const char * const cmd_default_device_usage =
"device\n"
"    device {ISL68224} 'or' {ISL68127}\n"
"    set the device as default device\n";

static void cmd_default_device(void *hint, int argc, char const *argv[])
{
	if (argc != 2){
		printf("%s", cmd_default_device_usage);
		return;
	}

	if (strcasecmp("ISL68224", argv[1]) == 0)
		set_default_device(ISL68224);
	else if (strcasecmp("ISL68127", argv[1]) == 0)
		set_default_device(ISL68127);
	else
		printf("%s", cmd_default_device_usage);
}

static const char * const cmd_filename_usage =
"filename\n"
"    filename <0:otherfilename.hex>\n"
"    set otherfilename as default filename\n"
"    default filename = '0:firmware.hex' at first\n";

static void cmd_filename(void *hint, int argc, char const *argv[])
{
	int ret;

	if (argc != 2) {
		printf("%s", cmd_filename_usage);
		return;
	}

	ret = strncmp("0:", argv[1], 2);
	if (ret != 0) {
		printf("%s", cmd_filename_usage);
		return;
	}

	set_default_file_name((char* )argv[1]);
	printf("default filename = '%s'\n", argv[1]);
}

static const char * const cmd_burn_usage =
"burn\n"
"    burn from sd card\n";

static void cmd_burn(void *hint, int argc, char const *argv[])
{
	if (argc != 1)
		printf("%s", cmd_burn_usage);
	else {
		led_4_0_on();
		printf("burn %s to device\n", get_default_filename());
		isl68224_program(get_default_filename());
		led_4_1_on();
	}
}

static const char * const cmd_flash_burn_usage =
"flashburn\n"
"    flashburn <file address in flash>\n"
"    burn from flash\n";

static void cmd_flash_burn(void *hint, int argc, char const *argv[])
{
	uint32_t file_addr;
	if (argc != 2)
		printf("%s", cmd_flash_burn_usage);
	else {
		led_4_0_on();
		file_addr = strtol(argv[1], NULL, 0);
		isl68224_program_from_mcuflash(file_addr);
		led_4_1_on();
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
	{"mcuinfo", NULL, cmd_mcu_info_usage, cmd_mcu_info},
	{"deviceinfo", NULL, cmd_device_info_usage, cmd_device_info},
	{"led", NULL, cmd_led_usage, cmd_led},
	{"addr", NULL, cmd_default_addr_usage, cmd_default_addr},
	{"device", NULL, cmd_default_device_usage, cmd_default_device},
	{"filename", NULL, cmd_filename_usage, cmd_filename},
	{"burn", NULL, cmd_burn_usage, cmd_burn},
	{"flashburn", NULL, cmd_flash_burn_usage, cmd_flash_burn},
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
		printf("help [command]\n");
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
