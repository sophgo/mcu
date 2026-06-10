/*
 * console.c: CV84X6EVB 串口命令行交互
 *
 * 命令列表 (wishlist):
 *   pintest  -w func_name 0/1   GPIO 引脚控制
 *   powerseq -u/-d/-w d1..d5    上电时序控制
 *   iictest  -f/-r/-w/-d        I2C 设备读写
 *   pmbus    -r/-w val          PMBus 电压读写
 *   gett     [-t s] [-c n] [-v] 温度读取
 *   setval   [频率] [占空比]      PWM 调压
 *   hello / info / help          基础命令
 */

#include <console/ecdc/ecdc.h>
#include <stdio.h>
#include <ctype.h>
#include <system/system.h>
#include <string.h>
#include <stdlib.h>
#include <common/common.h>
#include <pin.h>
#include <loop/loop.h>
#include <gpio/gpio.h>
#include <project.h>
#include <tick/tick.h>
#include <power_seq/power_seq.h>
#include <ct7451/ct7451.h>
#include <pmbus/pmbus.h>
#include <pwm/pwm.h>
#include <i2c/i2c_master/i2c_master.h>

static struct ecdc_console *console;

/* ================================================================
 * 引脚名字查找表 — 按 wishlist 中的 func_name
 * ================================================================ */
static struct gpio_node *all_output_nodes[] = {
	&pcie_sel, &en_12v0, &poweren1, &poweren2, &poweren3,
	&sys_rstn_h, &pwr_rstn_h, &pwr_button1_h, &pwr_on_h, &pwr_wakeup_h,
};

static struct gpio_node *find_output_by_name(const char *name)
{
	int i;
	for (i = 0; i < ARRAY_SIZE(all_output_nodes); ++i) {
		if (strcasecmp(all_output_nodes[i]->name, name) == 0)
			return all_output_nodes[i];
	}
	return NULL;
}

/* 控制台轮询 (在 gett 等待期间使用, 避免看门狗) */
void console_poll(void);

/* ================================================================
 * 控制台底层 I/O
 * ================================================================ */
static int console_getc(void *console_hint)
{
	int err = uart_getc();
	return err >= 0 ? err : ECDC_GETC_EOF;
}
static void console_putc(void *console_hint, char c)
{
	uart_putc(c);
}

/* ================================================================
 * pintest -w <func_name> <0|1>
 * ================================================================ */
static const char cmd_pintest_usage[] =
	"pintest -w <func_name> <0|1>\n"
	"  Set output GPIO high(1) or low(0)\n"
	"  func_name: PCIE_SEL EN_12V0 POWEREN1 POWEREN2 POWEREN3\n"
	"             SYS_RSTN_H PWR_RSTN_H PWR_BUTTON1_H PWR_ON_H PWR_WAKEUP_H\n";

static void cmd_pintest(void *hint, int argc, char const *argv[])
{
	struct gpio_node *node;

	if (argc != 4 || strcmp(argv[1], "-w") != 0) {
		printf("%s", cmd_pintest_usage);
		return;
	}
	node = find_output_by_name(argv[2]);
	if (!node) {
		printf("Unknown pin: %s\n", argv[2]);
		return;
	}
	if (atoi(argv[3]))
		gpio_set(node);
	else
		gpio_clear(node);
	printf("%s = %s\n", node->name, atoi(argv[3]) ? "HIGH" : "LOW");
}

/* ================================================================
 * powerseq -u / -d / -w d1 d2 d3 d4 d5
 * ================================================================ */
static const char cmd_powerseq_usage[] =
	"powerseq -u                     all pins UP with current delays\n"
	"powerseq -d                     all pins DOWN\n"
	"powerseq -w d1 d2 d3 d4 d5      set 5 gap delays (ms), then UP\n";

static void cmd_powerseq(void *hint, int argc, char const *argv[])
{
	unsigned int delays[POWER_SEQ_NUM - 1];
	int i;

	if (argc < 2) {
		printf("%s", cmd_powerseq_usage);
		return;
	}

	if (strcmp(argv[1], "-d") == 0) {
		power_seq_off();
		printf("Power DOWN done\n");
	} else if (strcmp(argv[1], "-u") == 0) {
		power_seq_on();
		printf("Power UP done, delay=%ums\n", power_seq_get_delay_ms());
	} else if (strcmp(argv[1], "-w") == 0) {
		if (argc != 7) {
			printf("Usage: powerseq -w d1 d2 d3 d4 d5\n");
			return;
		}
		for (i = 0; i < POWER_SEQ_NUM - 1; ++i)
			delays[i] = (unsigned int)atoi(argv[i + 2]);
		power_seq_on_with_delays(delays);
		printf("Power UP done with delays:");
		for (i = 0; i < POWER_SEQ_NUM - 1; ++i)
			printf(" %u", delays[i]);
		printf("\n");
	} else {
		printf("%s", cmd_powerseq_usage);
	}
}

/* ================================================================
 * iictest -f / -w addr reg val / -r addr reg / -d addr
 *
 * 扫描 Bus 0 即可 (I2C0)
 * ================================================================ */
static const char cmd_iictest_usage[] =
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
		/* 扫描 I2C0 总线 */
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
			printf("I2C[0x%02X] reg 0x%02X = 0x%02X (%u)\n", addr, reg, val, val);
		else
			printf("I2C read failed\n");
	} else if (strcmp(argv[1], "-d") == 0) {
		if (argc != 3) {
			printf("Usage: iictest -d <addr>\n");
			return;
		}
		addr = (uint8_t)strtol(argv[2], NULL, 0);
		printf("Dump I2C[0x%02X]:\n", addr);
		for (reg = 0; reg < 256; ++reg) {
			rv = i2c_master_smbus_read_byte(I2C0, addr, 5, reg, &val);
			if (rv == 0 && val != 0xFF) {
				printf("  [0x%02X] = 0x%02X\n", reg, val);
			}
		}
		printf("dump done.\n");
	} else {
		printf("%s", cmd_iictest_usage);
	}
}

/* ================================================================
 * pmbus -r / -w val
 *
 * 目标 PMBus 电源芯片地址在 pmbus 模块配置
 * ================================================================ */
static const char cmd_pmbus_usage[] =
	"pmbus -r         read output voltage\n"
	"pmbus -w <val>   set output voltage (mV)\n";

static void cmd_pmbus(void *hint, int argc, char const *argv[])
{
	unsigned int mv;

	if (argc < 2) {
		printf("%s", cmd_pmbus_usage);
		return;
	}

	if (strcmp(argv[1], "-r") == 0) {
		mv = pmbus_get_voltage();
		if (mv)
			printf("VOUT = %u mV\n", mv);
		else
			printf("PMBus read failed\n");
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

/* ================================================================
 * gett [-t s] [-c n] [-v]
 *
 * 温度读取: 本地(local)+远程(remote)
 * -t s  间隔 s 秒
 * -c n  读取 n 次
 * -v    显示寄存器原始值
 * ================================================================ */
static const char cmd_gett_usage[] =
	"gett                  read temperature once\n"
	"gett -t <s>           read every <s> seconds\n"
	"gett -t <s> -c <n>   read <n> times, every <s> seconds\n"
	"gett -t <s> -c <n> -v read raw register values\n"
	"gett -v               read once, show raw values\n";

static void gett_read_raw(void)
{
	uint8_t data;

	printf("CT7451 Raw Registers:\n");
	if (i2c_master_smbus_read_byte(I2C0, CT7451_SLAVE_ADDR, 5, 0x00, &data) == 0)
		printf("  Local  [0x00] = 0x%02X (%d)\n", data, data);
	if (i2c_master_smbus_read_byte(I2C0, CT7451_SLAVE_ADDR, 5, 0x01, &data) == 0)
		printf("  Remote [0x01] = 0x%02X (%d)\n", data, data);
}

static void cmd_gett(void *hint, int argc, char const *argv[])
{
	int interval_s = 0, count = 1, show_raw = 0;
	int i, idx = 1;
	unsigned long t0;

	/* 解析参数 */
	while (idx < argc) {
		if (strcmp(argv[idx], "-t") == 0 && idx + 1 < argc) {
			interval_s = atoi(argv[idx + 1]);
			idx += 2;
		} else if (strcmp(argv[idx], "-c") == 0 && idx + 1 < argc) {
			count = atoi(argv[idx + 1]);
			idx += 2;
		} else if (strcmp(argv[idx], "-v") == 0) {
			show_raw = 1;
			idx++;
		} else {
			idx++;
		}
	}

	/* 先让 ct7451_process 更新一次缓存 */
	ct7451_process();

	for (i = 0; i < count; ++i) {
		if (i > 0) {
			/* 等待 interval_s 秒 */
			t0 = tick_get();
			while (tick_get() - t0 < (unsigned long)interval_s * 1000) {
				/* 轮询 console 防止看门狗/超时 */
				console_poll();
			}
		}

		if (show_raw) {
			gett_read_raw();
		}
		printf("Local  Temperature: %d C\n", ct7451_local_temp);
		printf("Remote Temperature: %d C\n", ct7451_remote_temp);
	}
}

/* ================================================================
 * pwm [频率Hz] [占空比0-1000]
 * 高电平有效, 默认 50000Hz
 * ================================================================ */
static const char cmd_pwm_usage[] =
	"pwm [freq_hz] [duty_permil]\n"
	"  Set PWM frequency and duty cycle\n"
	"  freq: 1~50000 Hz,  duty: 0~1000 (0%%~100%%)\n"
	"  Example: pwm 50000 500  -> 50kHz, 50%% duty\n";

static void cmd_pwm(void *hint, int argc, char const *argv[])
{
	unsigned int freq = pwm_get_freq();
	unsigned int duty = pwm_get_duty();

	if (argc >= 2)
		freq = (unsigned int)atoi(argv[1]);
	if (argc >= 3)
		duty = (unsigned int)atoi(argv[2]);

	pwm_set(freq, duty);
	printf("PWM: %u Hz, duty %u/1000 (%u%%)\n",
	       pwm_get_freq(), pwm_get_duty(),
	       pwm_get_duty() * 100 / 1000);
}

/* ================================================================
 * hello / info
 * ================================================================ */
static void cmd_hello(void *hint, int argc, char const *argv[])
{
	printf("Hello! CV84X6EVB User @v@\n");
}
static void cmd_info(void *hint, int argc, char const *argv[])
{
	printf("Board Name: CV84X6EVB\n");
	printf("Board Type: 0x%02x\n", get_board_type());
	printf("MCU Firmware: %d\n", get_firmware_version());
	printf("PCB Version: %d\n", get_pcb_version());
	printf("BOM Version: %d\n", get_bom_version());
	printf("MCU Family: GD32E50x\n");
}

/* ================================================================
 * 命令表与帮助
 * ================================================================ */
struct command {
	const char *name, *usage;
	ecdc_callback_fn fn;
};

static void cmd_help(void *hint, int argc, char const *argv[]);

static struct command command_list[] = {
	{"help",     NULL,               cmd_help},
	{"hello",    NULL,               cmd_hello},
	{"info",     NULL,               cmd_info},
	{"pintest",  cmd_pintest_usage,  cmd_pintest},
	{"powerseq", cmd_powerseq_usage, cmd_powerseq},
	{"iictest",  cmd_iictest_usage,  cmd_iictest},
	{"pmbus",    cmd_pmbus_usage,    cmd_pmbus},
	{"gett",     cmd_gett_usage,     cmd_gett},
	{"pwm",      cmd_pwm_usage,      cmd_pwm},
};

static void print_usage(struct command *cmd)
{
	if (cmd->usage)
		printf("%s", cmd->usage);
	else
		printf("%s\n", cmd->name);
}

static struct command *find_command(const char *name)
{
	int i;
	for (i = 0; i < ARRAY_SIZE(command_list); ++i)
		if (strcmp(command_list[i].name, name) == 0)
			return &command_list[i];
	return NULL;
}

static void cmd_help(void *hint, int argc, char const *argv[])
{
	int i;
	struct command *cmd;

	if (argc == 1) {
		printf("CV84X6EVB Console Commands:\n");
		for (i = 0; i < ARRAY_SIZE(command_list); ++i) {
			cmd = &command_list[i];
			printf("  %-10s", cmd->name);
			if (cmd->usage)
				printf(" - %s", strchr(cmd->usage, '\n') ?
				       (strchr(cmd->usage, '\n') + 1) : cmd->usage);
			printf("\n");
		}
	} else if (argc == 2) {
		cmd = find_command(argv[1]);
		if (cmd)
			print_usage(cmd);
		else
			printf("'%s' Not Found\n", argv[1]);
	} else {
		printf("Usage: help [command]\n");
	}
}

/* ================================================================
 * 控制台初始化
 * ================================================================ */
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
void console_add(void)
{
	console_init();
	loop_add(console_poll);
}
void console_test(void)
{
	while (1) console_poll();
}