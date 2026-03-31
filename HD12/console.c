#include <gd32f4xx.h>
#include <ecdc/ecdc.h>
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
#include <multiphase.h>
#include <adc.h>
#include <project.h>
#include <power.h>
#include <dbgi2c.h>
#include <freq.h>
#include <i2c_master.h>
#include <efuse.h>

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

static const char * const cmd_volt_usage =
"volt\n"
" get voltage of chip from multiphase\n";

static void cmd_volt(void *hint, int argc, char const *argv[])
{
	int page;
	unsigned long val;

	if(argc != 1){
		dbg_printf("invalid usage\n");
		return;
	}

	/* multiphase read voltage*/
	for(page = 0; page < 2; page++){
		val = multiphase_output_voltage(0, page);
		dbg_printf("chip volt val = %ld\n", val);
	}
}

static const char * const cmd_volt_set_usage =
"volt_set [chip_num] [val]\n"
" set voltage for chip\n"
" chip num range 0~1\n";

static void cmd_volt_set(void *hint, int argc, char const *argv[])
{
	int page;
	int val;

	if(argc != 3){
		dbg_printf("invalid usage\n");
		return;
	}

	/* multiphase set voltage*/
	page = atoi(argv[1]);
	val = atoi(argv[2]);
	dbg_printf("page = %d, val = %d\n", page, val);
	multiphase_set_out_voltage(0, page, val);
}

static const char * const cmd_rdroop_usage =
"rdroop\n"
" get rdroop from multiphase\n";

static void cmd_rdroop(void *hint, int argc, char const *argv[])
{
	int page;
	unsigned long val;

	if(argc != 1){
		dbg_printf("invalid usage\n");
		return;
	}

	/* multiphase read rdroop*/
	for (page = 0; page < 2; page++) {
		val = multiphase_output_droop(0, page);
		dbg_printf("chip-%d rdroop val = %ld\n", page, val);
	}
}

static const char * const cmd_rdroop_set_usage =
"rdroop_set [chip_num] [val]\n"
" set rdroop for chip\n"
" chip num range 0~1\n";

static void cmd_rdroop_set(void *hint, int argc, char const *argv[])
{
	int page;
	int val;

	if (argc != 3) {
		dbg_printf("invalid usage\n");
		return;
	}

	/* multiphase set rdroop*/
	page = atoi(argv[1]);
	val = atoi(argv[2]);
	multiphase_set_out_droop(0, page, val);
}

static const char * const cmd_sn_usage =
"sn [sn]\n"
"    if sn is ommited, this command shows current sn\n"
"    otherwise, sn will write to eeprom\n";

static void cmd_sn(void *hint, int argc, char const *argv[])
{
	int i;
	uint8_t tmp;
	uint8_t flash_data[SN_PAGE_SIZE] = {0};
	volatile uint8_t *p_sn = (uint8_t *) SN_BASE;

	if (argc > 2)
		dbg_printf("invalid usage\n");
	else if (argc == 1) {
		/* read sn out */
		for (i = 0; i < EEPROM_CELL_SIZE; ++i) {
			tmp = p_sn[i];
			if (tmp) {
				if (isprint(tmp))
					dbg_printf("%c", tmp);
				else
					dbg_printf(".");
			} else
				break;
		}
		dbg_printf("\n");
	}
	else {
			for (i = 0; i < FLASH_PAGE_SIZE; i++){
				flash_data[i] = p_sn[i];
			}

			/* write sn to eeprom */
			for (i = 0; i < SN_CELL_SIZE - 1; ++i) {
				tmp = argv[1][i];

				if (tmp){
					flash_data[SN_CELL_OFFSET(0) + i] = tmp;
				}
				else
					break;
			}
			/* zero terminated */
			flash_data[SN_CELL_OFFSET(0) + i] = 0;
			flash_program_page(SN_BASE, flash_data, SN_PAGE_SIZE);
	}
}

#if 0
uint32_t sys_rst_pin_list[4][2] = {
	{SYS_RST_X_H_BM0_PORT, SYS_RST_X_H_BM0_PIN},
	{SYS_RST_X_H_BM1_PORT, SYS_RST_X_H_BM1_PIN},
};
static void cmd_low(void *hint, int argc, char const *argv[])
{
	uint32_t pin = 0;
	if (argc != 2) {
		dbg_printf("error cmd\n");
		return;
	}

	pin = strtol(argv[1], NULL, 0);
	gpio_clear(sys_rst_pin_list[pin][0], sys_rst_pin_list[pin][1]);

	dbg_printf("set chip%lu low\n", pin);

}

static void cmd_high(void *hint, int argc, char const *argv[])
{
	uint32_t pin = 0;
	if (argc != 2) {
		dbg_printf("error cmd\n");
		return;
	}

	pin = strtol(argv[1], NULL, 0);
	gpio_set(sys_rst_pin_list[pin][0], sys_rst_pin_list[pin][1]);

	dbg_printf("set chip%lu high\n", pin);
}
#endif


static const char * const cmd_power_usage =
"power\n"
	"get power of atx 12v, pcie 12v & 3v3\n";

static void cmd_power(void *hint, int argc, char const *argv[])
{
	if(argc != 1){
		dbg_printf("invalid usage\n");
	}

	unsigned long atx_12v_power;

	atx_12v_power = adc_read_sys_i12v() * 12;

	dbg_printf("atx 12v power: %lu mW\n",atx_12v_power);
}

static const char * const cmd_info_usage =
"info\n"
"    get information about board and mcu\n";

static void cmd_info(void *hint, int argc, char const *argv[])
{
	dbg_printf("firmware build time:%s-%s\n", __DATE__, __TIME__);
	dbg_printf("PCB Version: %d\n", get_pcb_ver());
	dbg_printf("MCU_SW_VER: %d\n", MCU_SW_VER);
}

static const char * const cmd_temp_usage =
"temp\n"
"    temp soc&board\n";
static void cmd_temp(void *hint, int argc, char const *argv[])
{
	int boardtemp0, soctemp0, boardtemp1, soctemp1;


	boardtemp0 = get_board_temp(0);
	soctemp0 = get_soc_temp(0);
	boardtemp1 = get_board_temp(1);
	soctemp1 = get_soc_temp(1);
	if (argc == 1){
		dbg_printf("CHIP0: soc temp = %d(C)\tboard temp = %d(C)\n", soctemp0, boardtemp0);
		dbg_printf("CHIP1: soc temp = %d(C)\tboard temp = %d(C)\n", soctemp1, boardtemp1);
	}else {
		dbg_printf(cmd_temp_usage);
	}
}

static const char * const cmd_location_usage =
"location\n"
"    get location of board\n";
static void cmd_location(void *hint, int argc, char const *argv[])
{
	if (argc == 1){
		dbg_printf("Board location: %d\n", get_module_id());
	}else {
		dbg_printf(cmd_location_usage);
	}
}


static const char * const cmd_multiphase_usage =
"multiphase [page] [reg]\n"
"    get multiphase reg value\n";
static void cmd_multiphase(void *hint, int argc, char const *argv[])
{
	int page;
	int reg;
	unsigned long val;

	if(argc != 3){
		dbg_printf("invalid usage\n");
		return;
	}
	page = atoi(argv[1]);
	reg = strtol(argv[2], NULL, 0);
	val = multiphase_read_reg(0, page, reg);
	dbg_printf("multiphase reg value: %0lx\n", val);
}

static const char * const cmd_dbgi2c_usage =
"dbgi2c [idx] [pll_mode] [freq]\n"
"    debug i2c write freq to chip\n";
static void cmd_dbgi2c(void *hint, int argc, char const *argv[])
{
#define TPU_MPLL		2
#define K2K_MPLL		3
#define PARENT_FREQ		25 * MHZ
	int idx;
	int freq;
	int ret;
	int pll_mode;

	if(argc != 4){
		dbg_printf("invalid usage\n");
		return;
	}

	idx = atoi(argv[1]);
	pll_mode = atoi(argv[2]);
	freq = atoi(argv[3]);

	if (pll_mode != TPU_MPLL && pll_mode != K2K_MPLL) {
		dbg_printf("invalid pll_mode, should be 2 for tpu mpll or 3 for k2k mpll\n");
		return;
	}

	if (freq < 500 || freq > 2000) {
		dbg_printf("invalid freq, should be between 500 and 2000\n");
		return;
	} else {
		ret = sg2044_clk_pll_set_rate(idx, pll_mode, freq * MHZ, PARENT_FREQ);
		if (ret != 0) {
			dbg_printf("device chip tpu freq cfg fail, %d\n", ret);
			return;
		}
	}

	dbg_printf("idx: %d, pll_mode: %d, freq: %d\n", idx, pll_mode, freq);
}

static const char * const cmd_efuse_usage =
"efuse <chip>\n"
"    read or write efuse\n";
static void cmd_efuse(void *hint, int argc, char const *argv[])
{
	uint32_t value;
	if (argc == 1) {
		value = efuse_read32(0, 0);
		if (value == 0xdeadbeef)
			dbg_printf("read efuse fail\n");
		else
			dbg_printf("chip-0 value: %08lx\n", value);

		value = efuse_read32(1, 0);
		if (value == 0xdeadbeef)
			dbg_printf("read efuse fail\n");
		else
			dbg_printf("chip-1 value: %08lx\n", value);
	} else if (argc == 2) {
		int chip = atoi(argv[1]);
		value = 0x414b16c3; // vendor id && product id
		if (efuse_write32(chip, 0, value)) {
			dbg_printf("write efuse fail\n");
		} else {
			dbg_printf("write efuse success\n");
		}
	}
}

struct command {
	const char *name, *alias, *usage;
	ecdc_callback_fn fn;
};

static void cmd_help(void *hint, int argc, char const *argv[]);

static struct command command_list[] = {
	{"help", NULL , NULL, cmd_help},
	{"info", NULL, cmd_info_usage, cmd_info},
	{"temp", NULL, cmd_temp_usage, cmd_temp},
	{"volt", NULL, cmd_volt_usage, cmd_volt},
	{"volt_set", NULL, cmd_volt_set_usage, cmd_volt_set},
	{"rdroop", NULL, cmd_rdroop_usage, cmd_rdroop},
	{"rdroop_set", NULL, cmd_rdroop_set_usage, cmd_rdroop_set},
	{"sn", NULL, cmd_sn_usage, cmd_sn},
	{"power", NULL, cmd_power_usage, cmd_power},
	// {"low", NULL, NULL, cmd_low},
	// {"high", NULL, NULL, cmd_high},
	{"location", NULL, cmd_location_usage, cmd_location},
	{"multiphase", NULL, cmd_multiphase_usage, cmd_multiphase},
	{"dbgi2c", NULL, cmd_dbgi2c_usage, cmd_dbgi2c},
	{"efuse", NULL, cmd_efuse_usage, cmd_efuse},
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
			dbg_printf("\'%s\' not found\n", argv[1]);
	} else {
		dbg_printf("invalid usage\n");
		dbg_printf("help [command]");
	}
}

int console_init(void)
{
	int i;

	console = ecdc_alloc_console(NULL, console_getc, console_putc, 128, 4);
	if (console == NULL) {
		dbg_printf("create console failed\n");
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
