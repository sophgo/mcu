#include <ecdc/ecdc.h>
#include <stdio.h>
#include <ctype.h>
#include <system.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <common.h>
#include <mon.h>
#include <upgrade.h>
#include <flash.h>
#include <timer.h>
#include <pin.h>
#include <mp5475.h>
#include <isl68224.h>
#include <power.h>


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
	printf("Hello BITMAIN SOPHON\n");
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

static const char * const cmd_show_usage =
"show\n"
"    show collected information, voltage, current, power etc.\n";

static void cmd_show(void *hint, int argc, char const *argv[])
{
	if (argc != 1) {
		printf("invalid usage\n");
		return;
	}

	mon_put_text();
}

static const char * const cmd_monmode_usage =
"monmode [clear|normal|verbose]\n"
"    switch mon collection mode between clear, normal and verbose\n"
"    if on or off ommited, this command shows current verbose mode\n"
"    valid collection modes are:\n"
"    -----------------------------------------------------------------------\n"
"    clear: MCU never collect information from MP5475 and ISL68224\n"
"    normal: MCU only collect TPU voltage, current and power from ISL68224\n"
"    verbose: MCU collect all information from both MP5475 and ISL68224\n";

static void cmd_monmode(void *hint, int argc, char const *argv[])
{
	if (argc > 2)
		printf("invalid usage\n");
	else if (argc == 1)
		printf("current collect mode %s\n",
		       mon_get_mode() ? "on" : "off");
	else if (strcmp("clear", argv[1]) == 0)
		mon_set_mode(MON_MODE_CLEAR);
	else if (strcmp("normal", argv[1]) == 0)
		mon_set_mode(MON_MODE_NORMAL);
	else if (strcmp("verbose", argv[1]) == 0)
		mon_set_mode(MON_MODE_VERBOSE);
	else
		printf("invalid verbose mode \'%s\'", argv[1]);
}

static const char * const cmd_sn_usage =
"sn [sn]\n"
"    if sn is ommited, this command shows current sn\n"
"    otherwise, sn will write to eeprom\n";

static void cmd_sn(void *hint, int argc, char const *argv[])
{
	int i;
	uint8_t tmp;
	uint8_t flash_data[FLASH_PAGE_SIZE] = {0};
	volatile uint8_t *p_eeprom = (uint8_t *) EEPROM_BASE;

	if (argc > 2)
		printf("invalid usage\n");
	else if (argc == 1) {
		/* read sn out */
		for (i = 0; i < EEPROM_CELL_SIZE; ++i) {
			tmp = eeprom_read_byte(EEPROM_SN_OFFSET + i);
			if (tmp) {
				if (isprint(tmp))
					printf("%c", tmp);
				else
					printf(".");
			} else
				break;
		}
		printf("\n");
	}
	else {
			for (i = 0; i < FLASH_PAGE_SIZE; i++){
				flash_data[i] = p_eeprom[i];
			}

			/* write sn to eeprom */
			for (i = 0; i < EEPROM_CELL_SIZE - 1; ++i) {
				tmp = argv[1][i];

				if (tmp){
					flash_data[EEPROM_SN_OFFSET + i] = tmp;
				}
				else
					break;
			}
			/* zero terminated */
			flash_data[EEPROM_SN_OFFSET + i] = 0;
			flash_program_page(EEPROM_BASE, flash_data, FLASH_PAGE_SIZE);
	}
}


static const char * const cmd_download_usage =
"download [offset]\n"
"    download file from host to program flash through xmodem\n"
"    if offset ommitted, program downloaded file from the beginning of program flash\n";

static void cmd_download(void *hint, int argc, char const *argv[])
{
	long int offset = 0;

	if (argc > 2) {
		printf("invalid usage\n");
		return;
	}

	if (argc == 2) {
		offset = strtol(argv[1], NULL, 0);
		if (offset == LONG_MIN || offset == LONG_MAX) {
			printf("invalid argument\n");
			return;
		}
	}

	printf("download to 0x%lx though xmodem\n", offset);

	if (download_to_flash(offset) < 0)
		printf("download failed\n");
	else
		printf("download success\n");
}

static const char * const cmd_burn_usage =
"burn\n"
"    burn firmware to program flash through xmodem\n";

static void cmd_burn(void *hint, int argc, char const *argv[])
{
	long int firmware_size;
	int err;

	if (argc != 1) {
		printf("invalid usage\n");
		return;
	}

	printf("download firmware though xmodem\n");

	firmware_size = download_to_flash(0);
	if (firmware_size < 0) {
		printf("download failed\n");
		return;
	}

	err = check_firmware(firmware_size);

	while (uart_getc() != '\r') {
		printf("firmware upgrade %s, press enter return to console\n",
		       err ? "failed" : "success");
		timer_mdelay(200);
	}
}

uint32_t sys_rst_pin_list[7][2] = {
	{B1_SYS_RST_N_PORT, B1_SYS_RST_N_PIN},
	{B2_SYS_RST_N_PORT, B2_SYS_RST_N_PIN},
	{B3_SYS_RST_N_PORT, B3_SYS_RST_N_PIN},
	{B4_SYS_RST_N_PORT, B4_SYS_RST_N_PIN},
	{B5_SYS_RST_N_PORT, B5_SYS_RST_N_PIN},
	{B6_SYS_RST_N_PORT, B6_SYS_RST_N_PIN},
	//{B7_SYS_RST_N_PORT, B7_SYS_RST_N_PIN},
	//{B8_SYS_RST_N_PORT, B8_SYS_RST_N_PIN},
	{GPIOB, GPIO_PIN_0},
};
static void cmd_low(void *hint, int argc, char const *argv[])
{
	uint32_t pin = 0;
	if (argc != 2) {
		printf("error cmd\n");
		return;
	}

	pin = strtol(argv[1], NULL, 0);
	gpio_clear(sys_rst_pin_list[pin][0], sys_rst_pin_list[pin][1]);

	printf("set chip%lu low\n", pin);

}

static void cmd_high(void *hint, int argc, char const *argv[])
{
	uint32_t pin = 0;
	if (argc != 2) {
		printf("error cmd\n");
		return;
	}

	pin = strtol(argv[1], NULL, 0);
	gpio_set(sys_rst_pin_list[pin][0], sys_rst_pin_list[pin][1]);

	printf("set chip%lu high\n", pin);
}

static void cmd_close_pmic(void *hint, int argc, char const *argv[])
{
	int port = -1;
	int bunk = -1;
	int i = 0;

	if (argc == 2) {
		port = strtol(argv[1], NULL, 0);
	} else if (argc == 3) {
		port = strtol(argv[1], NULL, 0);
		bunk = strtol(argv[2], NULL, 0);
	} else {
		printf("invaild argument\n");
		return;
	}

	if (port >= 0 && bunk >= 0) {
		mp5475_buck_off(port, bunk);
		printf("close 5475-%d bunk%d\n", port, bunk);
		return;
	}

	if (port >= 0 && bunk == -1) {
		for (i = 0; i < 4; i++) {
			mp5475_buck_off(port, i);
		}
		printf("mp5475-%d close all bunk\n", port);
		return;
	}
}

static const char * const cmd_setisl68224_usage =
"isl68224 [idx page voltage]\n"
"    idx: 0-5, there are 6 isl68224 in sc7fp150\n"
"    page: 0-5\n"
"    voltage: the value of voltage, unit is mV\n";

static void cmd_setisl68224_vout(void *hint, int argc, char const *argv[])
{
	int idx = 0;
	int page = 0;
	int voltage = 0;

	if (argc == 4) {
		idx = strtol(argv[1], NULL, 0);
		page = strtol(argv[2], NULL, 0);
		voltage = strtol(argv[3], NULL, 0);
		isl68224_set_out_voltage(idx, page, voltage);
	} else {
		printf("%s", cmd_setisl68224_usage);
	}

	return;
}

static const char * const cmd_rdrop_usage =
"rdrop [idx page  resistance]\n"
"    idx: 0-5, there are 6 isl68224 in sc7fp150\n"
"    page: 0-2 0:tpu 1:vddc 2:phy\n"
"    resistance: range from 0 to 1600,  unit is uV/A\n";

static void cmd_rdrop_vout(void *hint, int argc, char const *argv[])
{
	int idx = 0;
	int page = 0;
	int resistance = 0;
	//unsigned long val = 0;

	if (argc == 4 ) {
		idx = strtol(argv[1], NULL, 0);
		page = strtol(argv[2], NULL, 0);
		resistance = strtol(argv[3], NULL, 0);
		isl68224_set_out_droop(idx, page, resistance);
	}else{
		printf("%s", cmd_rdrop_usage);
	}
	return;
}
static const char * const cmd_tpu_rdrop_usage =
"tpu_rdrop  [resistance]\n"
"resistance: range from 0 to 1600,  unit is uV/A\n";
static void cmd_tpu_rdrop_vout(void *hint, int argc, char const *argv[]){
	int value;
	int i;

	if (argc != 2) {
		printf("%s", cmd_tpu_rdrop_usage);
		return;
	}

	value = strtol(argv[1], NULL, 0);
	for (i = 0; i < 6; i++) {
		isl68224_set_out_droop(i, 0, value);
	}
	return;
}

static const char * const cmd_tpu_usage =
"tpu [voltage]\n"
"    voltage: this value will be set in all isl68224,unit is mV\n";
static void cmd_tpu(void *hint, int argc, char const *argv[])
{
	int value;
	int i;

	if (argc != 2) {
		printf("%s", cmd_tpu_usage);
		return;
	}

	value = strtol(argv[1], NULL, 0);
	for (i = 0; i < 6; i++) {
		isl68224_set_out_voltage(i, 0, value);
	}

	return;
}

static void cmd_vddc(void *hint, int argc, char const *argv[])
{
	int value;
	int i;

	if (argc != 2) {
		printf("%s", cmd_tpu_usage);
		return;
	}

	value = strtol(argv[1], NULL, 0);
	for (i = 0; i < 6; i++) {
		isl68224_set_out_voltage(i, 1, value);
	}

	return;
}

static const char * const cmd_poweroff_usage =
"poweroff\n"
"    poweroff sc7fp150\n";

static void cmd_poweroff(void *hint, int argc, char const *argv[])
{
	power_off();
}

struct command {
	const char *name, *alias, *usage;
	ecdc_callback_fn fn;
};

static void cmd_help(void *hint, int argc, char const *argv[]);

static struct command command_list[] = {
	{"help", NULL , NULL, cmd_help},
	{"hello", NULL , NULL, cmd_hello},
	{"upgrade", NULL, cmd_upgrade_usage, cmd_upgrade},
	{"show", NULL, cmd_show_usage, cmd_show},
	{"monmode", NULL, cmd_monmode_usage, cmd_monmode},
	{"sn", NULL, cmd_sn_usage, cmd_sn},
	{"download", NULL, cmd_download_usage, cmd_download},
	{"burn", NULL, cmd_burn_usage, cmd_burn},
	{"low", NULL, cmd_burn_usage, cmd_low},
	{"high", NULL, cmd_burn_usage, cmd_high},
	{"closepmic", NULL, NULL, cmd_close_pmic},
	{"tpu", NULL, cmd_tpu_usage, cmd_tpu},
	{"vddc", NULL, NULL, cmd_vddc},
	{"isl68224", NULL, cmd_setisl68224_usage, cmd_setisl68224_vout},
        {"rdrop", NULL, cmd_rdrop_usage, cmd_rdrop_vout},
	{"tpu_rdrop", NULL, cmd_tpu_rdrop_usage, cmd_tpu_rdrop_vout},
	{"poweroff", NULL, cmd_poweroff_usage, cmd_poweroff},
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

	for (i = 0; i < sizeof(command_list) / sizeof(command_list[0]); ++i) {
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
