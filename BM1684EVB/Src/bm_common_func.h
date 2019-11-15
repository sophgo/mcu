/*
 * bm_common_func.h
 *
 *  Created on: 2019Äê11ÔÂ15ÈÕ
 *      Author: taiqiang.cao
 */

#ifndef BM_COMMON_FUNC_H_
#define BM_COMMON_FUNC_H_

#include "main.h"
#include "adc.h"
#include "i2c.h"
#include "lptim.h"
#include "rtc.h"
#include "gpio.h"

void HAL_Delay(uint32_t Delay);
void clean_pmic(void);
void PowerON(void);
void PowerDOWN(void);
void BM1684_RST(void);
void BM1684_REBOOT(void);
void Clean_ERR_INT(void);
void Scan_Cuerrent(void);
void Scan_Voltage(void);
void Set_HW_Ver(void);
void Factory_Info_get(void);
void Convert_sysrst_gpio(int io);
void led_all_on(void);
void clean_update_flag(void);
void nct80_init(void);
void poll_pcie_rst(void);
void config_regs(void);
void module_init(void);
void cmd_process(void);


#endif /* BM_COMMON_FUNC_H_ */
