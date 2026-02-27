#include <gd32e50x_gpio.h>
#include <gd32e50x_misc.h>
#include <tick.h>
#include <system.h>
#include <i2c_slave.h>
#include <debug.h>
#include <timer.h>
#include <power.h>
#include <pin.h>
#include <common.h>
#include <project.h>
#include <console.h>
#include <slave.h>
#include <chip.h>
#include <loop.h>
#include <board_power_impl.h>
#include <pcie.h>
#include <i2c-slaves/ct7451.h>
#include <isl68224.h>
#include <ddr.h>


int main(void)
{
	/* MCU初始化 */
	system_init();
	dbg_printf("\nBM1690E EVB\n");
	dbg_printf("firmware build time:%s-%s\n", __DATE__, __TIME__);
	/* 开启led，显示mcu已准备好 */
	led_init();

	mdelay(40);
	/* 设置板类型 */
	board_init();
	/* 启动上电时序 */
	power_on();
	/* 初始化传感器 */
	ct7451_init();

	/* 启动芯片up计时 */
	chip_init();

	/* 设置IIC1下挂载的两个slave */
	slave_init();
	/* 检测pcie复位信号，为1690e解复位 */
	pcie_init();

	/* 初始化 RAA */
	isl68224_init();
	/* 配置串口调试 */
	console_add();

	ddr_size_init(DDR_SIZE_2R_64G);

	/* never return */
	/* 持续性任务循环执行 */
	/* 1. 持续获取设备状态，功耗，电压，电流，温度 */
	/* 1.1 根据当前状态决定下一步状态 */
	/* 2. 串口接收命令 */
	/* 3. 做从设备接收主芯片指令 */
	loop_start();

	return 0;
}
